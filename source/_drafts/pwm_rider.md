---
title: Ride the PWM
author: Michael Kwaśnicki
date: 2019-11-15
tags: [Arduino, NeoPixel, PWM]
photos: surf_lem.svg
---

Driving NeoPixel LEDs (and compatibles) with 800kHz on 8MHz AVR microcontrollers is a challenge.
Here I present an alternative approach to bit banging.
By using the PWM hardware it is more versatile than previous solutions and can be used for other purposes as well.


<!-- more -->



## What ##

Driving WS2811 LEDs (also known as NeoPixel) at 800kHz with AVR microcontrollers with no additional hardware has been done before by [Just in Time][1].
Also the [Adafruit NeoPixel library for Arduino][2] supports ATmega and ATtiny devices at 8MHz.
Those implementations have in common that they transmit the control signal through bit banging.
They are also both written in AVR assembly because of the tight timing of only 10 clock cycles for each bit to be transmitted.

Now I want to try a different approach and leverage the PWM hardware feature for shaping signals.



## Why ##

First of all I want to drive WS2811 LEDs at 800kHz with an ATmega328P that is using just its internal clock and no external crystal oscillator.
Because of the tight timing it is a necessity to learn AVR assembler, which I wanted to do for a long time but had no real excuse to do so until now.
By using hardware capabilities this approach is more flexible than bit banging.
The generated waveform can be altered easily.
This might become handy because the timing for 0-bit and 1-bit differ between various addressable LEDs:

* [WS2811][8] (the timing shows the 400kHz communication - so has to be halved)
* [WS2812][9]
* [WS2812B][A]
* [APA104][B] (the timings don't match the claimed 800kHz and also the addition TH+TL doesn't add up)
* [APA106][C] (the timings don't match the claimed 800kHz)
* [PL9823][D] (the timings don't match the claimed 800kHz)
* [SK6812][E]
* [SK6812-RGBW][F]

You just have to set the duty cycle once per bit and the timer used in Fast PWM Mode does the shaping of the output waveform.
This enables the use of different bit timings for each transmission.

<figure>
    {% asset_img duty_pwm.svg %}
    <figcaption>Different duty cycles can be achieved with the same AVR assembly</figcaption>
</figure>

In contrast you have to bit bang the pin twice per bit when using the old approach.
And you will need completely different AVR assembler code for different timings.

<figure>
    {% asset_img duty_bang.svg %}
    <figcaption>Different duty cycle require a realignment of the AVR assembly</figcaption>
</figure>



## How ##

The basic idea is to start a PWM signal in Fast PWM Mode and run AVR assembly code in parallel and synchronously.

For development I used an Arduino Uno with 16MHz crystal oscillator where I set the clock prescaler to a "clock division factor" of 16.

``` c
                            // only for development purpose set clock speed to 1MHz
                            // timed sequence:
    CLKPR = _BV(CLKPCE);    // enable clock prescaler changes
    CLKPR = _BV(CLKPS2);    // set clock prescaler
```

This way it is much easier to measure timings and count clock cycles as one clock cycle corresponds to 1µs instead of 125ns at 8MHz.
Also it does not matter at which speed you are developing as long as you stay within 10 clock cycles to get the timings right.
You just need to remember to set the right clock speed when trying to actually control the LEDs.

There are a few things to consider before we can start.
Most important is the selection of the Timer/Counter (TC).
Only Timer/Counter0 (TC0) can be accessed in one (1) cycle via I/O-Registers by using `OUT`.
But all TCs can be accessed in two (2) cycles via SFR (memory mapped Special Function Registers) by using `STS`.

| Timer/Counter |        Pins         |            Access Time          |             Conflicts             |
|--------------:|--------------------:|--------------------------------:|:---------------------------------:|
|       0       |        5 (PD5)      | 1 cycle (OUT) or 2 cycles (STS) |  Arduino `delay()` and `millis()` |
|       1       | 9 (PB1) or 10 (PB2) |                  2 cycles (STS) |                *)                 |
|       2       |        3 (PD3)      |                  2 cycles (STS) |                *)                 |

*): Conflict with third party libraries possible

I use TC2 as I have no other code it might interfere with.
If you are using other Arduino libraries that also use TC2, you may want to make sure it is okay or use a different TC instead.  
The TC is used only during transmission and can be used freely otherwise.  
Using the 16-bit TC1 is a bit of waste as we are only going to count 10 clock cycles.  
Using TC0 will interfere with the [Arduino built-in functions][G] like `millis()` and `delay()`.

To set TC2 to Fast PWM Mode with TOP set to `OCR2A`, the Timer/Counter Control Register (`TCCR2`) needs to be set accordingly:

``` c
    DDRD   = _BV(PD3);                                  // set pin 3 as output (PD3)
    TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);     // enable Fast PWM Mode on pin 3 (OC2B)
    TCCR2B = _BV(WGM22);
```

The desired frequency is 800kHz.
At 8MHz cpu clock speed this means that we need a PWM period of 10 clock cycles.
In Fast PWM Mode the Output Compare Register A sets the period.
It counts from 0 to `OCR2A` inclusive.
For 10 clock cycles we need 0…9 so we set `OCR2A` accordingly.

``` c
    OCR2A = 9;  // set TOP for Fast PWM MODE and only count from 0 to 9 (10 clock cycles)
```

The duty cycle is controlled with Output Compare Register B (`OCR2B`).
A value of 0 corresponds to a duty cycle of 1/10 (1µs), while value of 9 corresponds to a duty cycle of 10/10 (always high).

``` c
    OCR2B = x;  // x = 0...9 the duty cycle (duty = (OCR2B + 1) / OCR2A)
```

Since Fast PWM Mode uses both Output Compare Registers, only one pin can be used.
Most Arduino tutorials get this wrong but I found one that describes it well, called [Fast PWM on ATmega328, up to 8MHz][5].
This applies also for TC0 where you also can only use the PWM pin labelled OC0B.
TC1 is different.
The same limitation applies when using `OCR1A` as TOP.
But when using `ICR1` as TOP, both output pins OC1A and OC1B can be selected.



## Hands on ##

Now comes the fun part.
To shape the waveform with the data that we want to transmit, we need AVR assembly code that runs in sync with the TC2.  
There are four parts to consider:

1. How to start the transmission and transition to step 2.
2. Transmit each bit in a byte.
3. Load the next byte and transition to step 2 or step 4.
4. Stop the timer at the end.



### Entry ###

The waveform is generated the next clock cycle as soon as we start the TC2 by setting the Clock Select Bit in `TCCRB`.
If we set the duty cycle after that, we are already off by one clock cycle.
So we have to set the duty cycle for the first bit prior to starting the clock.
Before that we also need to load the first byte and look at the first bit.
Luckily there is an instruction to load a byte from memory into a register and also post-increment the pointer to the next memory location at once.
There is no need to move the memory pointer by hand.
To look at the first bit (and any successive bit) the easiest way is to move the highest bit into the carry flag of the Status Register (SREG).
With the bit sitting in the carry flag you can use the conditional branching instructions.
So the entry sequence is this:

``` avrasm
    LDI %[lo], 1                ; (constant) store the duty cycle for 0-bit in register lo
    LDI %[hi], 7                ; (constant) store the duty cycle for 1-bit in register hi
    LDI %[start], 0x9           ; (constant) store the selected clock source in register start
                                ;            (That is _BV(WGM22) | _BV(CS20))
    LDI %[stop], 0x0            ; (constant) store no clock source in register stop
    LDI %[bytes], 0x??          ; the number of bytes remaining
    LDI %[bits], 0x7            ; the number of bits remaining
    LD  %[byte], %a[dataptr]+   ; load first byte and post-increment pointer
    LSL %[byte]                 ; shift the highest bit into the carry flag
    XXX                         ; do something according to the carry flag
    STS %[OCR2B], %[(lo|hi)]    ; set the Output Control Register B to the desired duty cycle
    STS %[TCCR2B], %[start]     ; start the timer by setting the Timer/Counter Control Register B
```

I use named registers here to let the compiler decide which register to use for each variable.
We don't need to count clock cycles until we start the TC2.
At the last line we start counting the used clock cycles.
In good tradition we use zero based counting.


### Bit by bit ###

Now we have 9 clock cycles to prepare the next bit.
In that time we have just to shift in the next bit into the carry flag and set the duty cycle accordingly.

``` avrasm
    LSL %[byte]                 ; shift the next bit into the carry flag
    XXX                         ; do something according to the carry flag
    STS %[OCR2B], %[(lo|hi)]    ; set the Output Control Register B to the desired duty cycle
```

Now we have to think about how we can choose the right duty cycle.
One possibility would be to use another register and store the duty cycle there and use it from that.
But that would waste one clock cycle.
Because we have to account for different duty cycles we have to branch anyway.
So instead of using the branch to copy a register with `MOV`, we can just branch to different `STS` instructions

``` avrasm
    bit:        LSL  %[byte]            ; shift the next bit into the carry flag
                BRCS bit_1              ; look at the carry flag and jump to bit_1 if it is set
                NOP                     ; keep both branches in sync

    bit_0:      STS  %[OCR2B], %[lo]    ; set the `OCR2B` to the desired duty cycle
                RJMP next_bit           ; process the next bit

    bit_1:      STS  %[OCR2B], %[hi]    ; set the `OCR2B` to the desired duty cycle
                RJMP next_bit           ; process the next bit
```

One thing to consider when writing AVR assembly is the clock count for each instruction.
AVR microcontrollers are extremely efficient and execute most instructions in just one clock cycle, given the values are already in one of the general purpose registers.
For this [ATmega328P megaAVR Data Sheet][3] and [AVR Instruction Set Manual][4] are the source of truth.
Noteworthy is that branch instructions use 1 clock cycle if the jump is not made and 2 clock cycles otherwise.
Since we need to set the duty cycle at a specific clock cycle, it is easiest to start with the `STS` instruction (which is clock cycle 9 and 0) and count backwards from there.
So the cycle counting looks like this:

``` avrasm
                                        ;    clock cycle
                                        ;   1-bit   0-bit
    bit:        XXX                     ;   3…5     3…5
                LSL  %[byte]            ;   6       6
                BRCS bit_1              ;   7/8     7
                NOP                     ;           8

    bit_0:      STS  %[OCR2B], %[lo]    ;           9/0
                RJMP next_bit           ;           1/2

    bit_1:      STS  %[OCR2B], %[hi]    ;   9/0
                RJMP next_bit           ;   1/2
```

We see that 7 clock cycles are required to set the duty cycle depending on the bit value.
To stay in sync one case has to be delayed by one clock cycle with a NOP.
So we have 3 clock cycles spare in each bit.
Instead of wasting those cycles we can use it to count down the bits, loop over that section and jump out for fetching the next byte.

``` avrasm
                                        ;    clock cycle
                                        ;   1-bit   0-bit
                LDI  %[bits], 0x7                           ; set the bit counter to 7 (count down from 7 to 0)

    next_bit:   NOP                     ;   3       3       ; fill that one missing clock cycle
                LSL  %[byte]            ;   4       4       ; shift the next bit into the carry flag
                DEC  %[bits]            ;   5       5       ; decrement the bit count
                BREQ next_byte          ;   6       6       ; jump to next_byte section if the counter reaches zero
                BRCS bit_1              ;   7/8     7       ; look at the carry flag and jump to bit_n_one if it is set
                NOP                     ;           8       ; synchronize both cases

    bit_0:      STS  %[OCR2B], %[lo]    ;           9/0     ; set the duty cycle to lo
                RJMP next_bit           ;           1/2     ; process the next bit

    bit_1:      STS  %[OCR2B], %[hi]    ;   9/0             ; set the duty cycle to hi
                RJMP next_bit           ;   1/2             ; process the next bit

    next_byte:
```

Notice that we make use of the fact that `DEC` does not alter the carry flag set by `LSL`.
Great, we can process each bit in a byte with a loop and waste only one (1) cycle.


### Fetch next byte ###

This is the tricky part.
While bit 8 is being processed, we have to decrement the byte count and see if we are done. If not so, load the next byte, set the bit counter back to 7 and set the duty cycle to the right value while staying in sync.
Easy, huh?
Let's look at the required instructions and the clock cycles:

``` avrasm
    next_byte:  DEC  %[bytes]           ;   1       1       ; decrement the count of bytes to process
                BREQ end                ;   2       2       ; jump to the end if the size reaches zero
                LDI  %[bits], 0x7       ;   3       5       ; set the bit counter back to 7
                LD   %[byte], %a[ptr]+  ;   4/5     4/5     ; load the next byte
                LSL  %[byte]            ;   6       6       ; shift the next bit into the carry flag
                BRCS bit8_1             ;   7       7/8     ; branch to the hi-bit case if carry is set
                NOP                     ;   8               ; keep both branches in sync

    bit8_0:     STS  %[OCR2B], %[lo]    ;   9/0             ; set the duty cycle to lo
                RJMP next_bit           ;   1/2             ; process the next bit

    bit8_1:     STS  %[OCR2B], %[hi]    ;           9/0     ; set the duty cycle to hi
                RJMP next_bit           ;           1/2     ; process the next bit

    end:
```

Well, no.
We didn't take into account that we enter `next_byte` at clock cycle 7.
This is the busiest part of the whole loop of byte processing.
We have to get into this part as quick as possible and not as late as clock cycle 7.
There is no way to do all necessary work during the last bit, if we have to jump into this section.
We are two clock cycles short.
(Some AVR microcontrollers have an internal clock of 9.6 MHz. There we have 12 clock cycles to reach the 800kHz goal - what a perfect match!)  
We have to split the work across bit 8 and the first bit of the next byte.
Processing the first bit of the next byte is almost exactly the same as the entry.
The only difference is that we do not have to start TC2 as it is already running.

``` avrasm
    next_byte:  LDI  %[bits], 0x7       ;   3       3       ; set the bit counter back to 7
                LD   %[byte], %a[ptr]+  ;   4/5     4/5     ; load the next byte
                LSL  %[byte]            ;   6       6       ; shift the next bit into the carry flag
                BRCS byte_1             ;   7       7/8     ; branch to the hi-bit case if carry is set
                NOP                     ;   8               ; keep both branches in sync

    byte_0:     STS  %[OCR2B], %[lo]    ;   9/0             ; set the duty cycle to lo
                RJMP next_bit           ;   1/2             ; process the next bit

    byte_1:     STS  %[OCR2B], %[hi]    ;           9/0     ; set the duty cycle to hi
                RJMP next_bit           ;           1/2     ; process the next bit
```

Those instructions fit into 8 clock cycles.
This is perfect, because we can jump into this section (which takes 2 clock cycles).
Jumps taken are expensive, jumps not taken are cheap.
The solution I came up with is to always jump into the part for bit 8 but leave it in case we are not processing the last bit.

``` avrasm
    nextbit:    LSL %[byte]             ; shift the next bit into the carry flag
                DEC %[bits]             ; decrement the bit count
                BRNE bit                ; jump to processing of bit 2…7
                DEC %[bytes]            ; decrement the count of bytes to process
                BRCS last_1             ; branch to the hi-bit case if carry is set
                NOP                     ; keep both branches in sync

    last_0:     STS %[OCR2B], %[lo]     ; set the duty cycle to lo
                BRNE nextbyte           ; process the next byte if the count of bytes is not zero
                RJMP end                ; jump to the end otherwise

    last_1:     STS %[OCR2B], %[hi]     ; set the duty cycle to hi
                BRNE nextbyte           ; process the next byte if the count of bytes is not zero
                RJMP end                ; jump to the end otherwise

    bit:        BRCS bit_1              ; branch to the hi-bit case if carry is set
                NOP                     ; keep both branches in sync

    bit_0:      STS %[OCR2B], %[lo]     ; set the duty cycle to lo
                RJMP nextbit            ; process the next bit

    bit_1:      STS %[OCR2B], %[hi]     ; set the duty cycle to hi
                RJMP nextbit            ; process the next bit

    nextbyte:   LDI %[bits], 0x7        ; reset the bit count to 7
                LD  %[byte], %a[ptr]+   ; load the next byte
                LSL %[byte]             ; shift the next bit into the carry flag
                BRCS byte_1             ; branch to the hi-bit case if carry is set
                NOP                     ; keep both branches in sync

    byte_0:     STS %[OCR2B], %[lo]     ; set the duty cycle to lo
                RJMP nextbit            ; process the next bit

    byte_1:     STS %[OCR2B], %[hi]     ; set the duty cycle to hi
                RJMP nextbit            ; process the next bit

    end:
```

This time I intentionally omit the clock cycle count and make it an exercise for the reader.
And again we make multiple times use of the fact that `DEC` does not alter the carry flag set by `LSL`.

There is still room for optimization as the sections labelled `bit_0` and `bit_1` are identical to those labelled `byte_0` and `byte_1`.
Also the `bit` section is identical to a subset of `nextbyte`.
This gives us the opportunity to reduce the code size by 6 instructions (12 bytes) and all we have to do is move the `bit` label to the end of the `nextbyte` section.


### End of sequence ###

Here is not much work to do.
You just have to wait until the clock count reaches 8 and then you stop the timer.

``` avrasm
    end:        RJMP .+0                ;   4/5     ; same as NOP NOP
                RJMP .+0                ;   6/7     ; same as NOP NOP
                NOP                     ;   8
                STS  %[TCCR2B], %[stop] ;   9/0
```

We enter this section at clock cycle 4, so we have to wait 5 more clock cycles before we have to stop TC2 and end the sequence.
Here we use a trick to delay two (2) clock cycles but use only one instruction and thus save some bytes of program space.
`RJMP` does a jump relative to the address of the next instruction, thus moving forward to the next instruction in two (2) clock cycles instead of one (1) as the jump is executed unconditionally.
With a zero (0) as its argument you will just land on that next instruction.
(`RJMP .-2` does an infinite loop.)



## Final result ##

In the end the combined assembly code looks like this:

``` avrasm
                ; assuming all input and constant values are declared and set before
    start:      LDI  %[bits], 0x7
                LD   %[byte], %a[ptr]+
                LSL  %[byte]
                BRCS start_1
    start_0:    STS  %[OCR2B], %[lo]
                STS  %[TCCR2B], %[start]
                RJMP nextbit
    start_1:    STS  %[OCR2B], %[hi]
                STS  %[TCCR2B], %[start]
                RJMP nextbit
    nextbyte:   LDI  %[bits], 0x7
                LD   %[byte], %a[ptr]+
                LSL  %[byte]
    bit:        BRCS bit_1
                NOP
    bit_0:      STS  %[OCR2B], %[lo]
                RJMP nextbit
    bit_1:      STS  %[OCR2B], %[hi]
                RJMP nextbit
    nextbit:    LSL  %[byte]
                DEC  %[bits]
                BRNE bit
                DEC  %[bytes]
                BRCS last_1
                NOP
    last_0:     STS  %[OCR2B], %[lo]
                BRNE nextbyte
                RJMP end
    last_1:     STS  %[OCR2B], %[hi]
                BRNE nextbyte
                RJMP end
    end:        RJMP .+0
                RJMP .+0
                NOP
                STS  %[TCCR2B], %[start]
```

Just wonderful.
35 AVR assembly instructions and not a single clock cycle wasted.
Even I can hardly imagine this, but I bet experienced AVR assembly programmers can do this even tighter.


### But there is more… ###

Setting the timings for 0-bits and 1-bits individually for each transmission is not enough.
Instead of changing the duty cycle for each bit, we can invert the waveform instead and thus generate [Manchester code][H] at the same rate.
How cool is that!

<figure>
    {% asset_img manchester_code.svg %}
    <figcaption>Manchester Code generated from inverted and non-inverted PWM signal.</figcaption>
</figure>

We have to prepare two registers for the value of `TCCR2A` instead of `OCR2B`.
Those would be:

``` c
    uint8_t hi = _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);                 // non-inverting waveform
    uint8_t lo = _BV(COM0B1) | _BV(COM0B0) | _BV(WGM01) | _BV(WGM00);   // inverting waveform
    OCR2A = 9;                                                          // count 10 clock cycles
    OCR2B = 4;                                                          // 50% duty cycle
```

And in AVR assembly `%[OCR2B]` needs to be replaced with `%[TCCR2A]`.
That's all.



## Conclusion ##

The AVR assembly provided here is not faster or smaller than the AVR assembly of [Just in Time][1].
Actually it must not be faster as it has to be cycle accurate.
But the AVR assembly is much easier to understand as I explained the steps taken during its creation in detail.
This benefits me especially, as I am new to writing assembly in general and for AVR in particular.

The generated waveform can be changed easily without rewriting the entire AVR assembly.
Something that cannot be achieved with bit banging, where you have to write different code for different bit timings.
With the PWM approach we can even produce Manchester code.

Of course the simplicity comes with some cost.
Using the hardware capabilities binds some hardware resources, a Timer/Counter in this case.
This also limits the pins that can be used to just one (1) of the TCs PWM pin (except for TC1).



## Follow up ##

For fun and profit I'd like to see how fast we can transmit serial data over a single pin.
[Just in Time][1] was able to transmit the data at 10 clock cycles per bit using bit banging, which requires to toggle the pin twice a bit.
The Fast PWM Mode approach only needs to set a register once per bit.
As long as we stick to TC0, we can replace the `STS` instructions with `OUT` instructions and have saved one clock cycle per bit already.
That is why in the next article I will explore how fast we can get with serial data transmission.



## Acknowledgement ##

During the development I stumbled over weird compiler optimizations.
Every time the named inputs used the same value, they got mapped to the same physical register.
This is clearly not what I wanted because one named input gets modified while the other does not.
I reached out to the [avr-gcc mailing list][6] because I did not understand why it was happening and I thought it was a defect in the compiler optimization as there was no issue when compiling with `-O0`.
In the [thread][7] the nice people Joseph C. Sible and David Brown helped me to understand that values are passed in by value and there is no requirement for a correlation between the C variables and registers in the AVR assembly.
Using variables as input AND output helps to establish some barrier between the values and registers, so that they don't get mixed up.


[1]: https://rurandom.org/justintime/w/Driving_the_WS2811_at_800_kHz_with_an_8_MHz_AVR
[2]: https://github.com/adafruit/Adafruit_NeoPixel
[3]: https://www.microchip.com/wwwproducts/en/ATMEGA328P
[4]: https://www.microchip.com/webdoc/avrassembler/
[5]: https://withinspecifications.30ohm.com/2014/02/20/Fast-PWM-on-AtMega328/
[6]: https://lists.nongnu.org/mailman/listinfo/avr-gcc-list
[7]: https://lists.nongnu.org/archive/html/avr-gcc-list/2019-07/threads.html
[8]: https://cdn-shop.adafruit.com/datasheets/WS2811.pdf
[9]: https://cdn-shop.adafruit.com/datasheets/WS2812.pdf
[A]: https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf
[B]: https://www.ledlightinghut.com/files/APA104%20Datasheet.pdf
[C]: https://cdn.sparkfun.com/datasheets/Components/LED/COM-12877.pdf
[D]: https://cdn.instructables.com/ORIG/FW0/YN1X/IHDUL683/FW0YN1XIHDUL683.pdf
[E]: https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf
[F]: https://cdn-shop.adafruit.com/product-files/2757/p2757_SK6812RGBW_REV01.pdf
[G]: https://www.arduino.cc/en/Tutorial/SecretsOfArduinoPWM
[H]: https://en.wikipedia.org/wiki/Manchester_code
