---
title: Ride the PWM
author: Michael Kwaśnicki
date: {{ date }}
tags: [new, post]
photos:
    - image.png
    - /images/dummy2.png
---

An alternative approach to drive WS2811 LEDs with an 8MHz AVR.


<!-- more -->


## What ##

Driving WS2811 LEDs (also known as NeoPixel) at 800kHz with an AVR microcontroller at only 8MHz is a challenge.
But it has been done before by [Just in Time][1] and also the [Adafruit NeoPixel library for Arduino][2] supports ATmega and ATtiny devices with 8MHz.
Those implementations have in common that they drive the LEDs through bit banging.
They are also both written in assembly because of the tight timing of only 10 clock cycles for each bit to be transmitted.

Now I wanted to try a different approach and leverage the PWM hardware feature of shaping signals.



## Why ##

First of all I wanted to drive WS2811 LEDs at 800kHz with an ATmega328p that is using just its internal clock and no external crystal oscillator.
Because of the tight timing it was a necessity to learn AVR assembler, which I wanted to do for a long time but had no real excuse to do so until now.
By using hardware capabilities this approach is more flexible than bit banging.
The generated waveform can be altered easily.
This might become handy because the timing for 0-bit and 1-bit differ between [WS2811][8] (the timing shows the 400kHz communication - so has to be halved), [WS2812][9], [WS2812B][A], [APA104][B] (the timings don't match the claimed 800kHz and also the addition TH+TL doesn't add up), [APA106][C] (the timings don't match the claimed 800kHz), [PL9823][D] (the timings don't match the claimed 800kHz) and [SK6812][E] with its variant [SK6812-RGBW][F].

You just have to set the duty cycle once per bit and the timer used in fast PWM mode does the shaping of the output waveform.
This allows to use different bit timings in each transmission.

!!!! BILD: verschiedene duty cycles mit nur ein mal setzen

In contrast you have to bit bang the pin twice per bit when using the other approach.

!!!! BILD: verschiedene duty cycles mit zwei mal setzen.



## How ##

The basic idea is to start the a PWM signal in fast PWM mode and run assembly code in parallel and synchronously.

For development I used an Arduino Uno with 16MHz crystal oscillator where I set the clock prescaler to a "clock division factor" of 16.

```
				            // timed sequence:
	CLKPR = _BV(CLKPCE);	// enable clock prescaler changes
	CLKPR = _BV(CLKPS2);	// set clock prescaler
```

This way it was much easier to measure timings and count clock cycles as one clock cycle corresponds to 1µs instead of 125ns at 8MHz.
Also it does not matter at which speed the you are developing as long as you stay within 10 clock cycles to get the timings right.

I used Timer/Counter0 as I have no other code it might interfere with.
If you are using other Arduino libraries or built in timing functions like `delay()`, you may want to use a different Timer/Counter.
To set it to fast PWM the Timer/Counter Control Register A needs to be set accordingly:

```
	TCCR0A = _BV(COM0B1) | _BV(WGM01) |  _BV(WGM00);
```
The desired frequency is 800kHz.
At 8MHz cpu clock this means that we need a PWM period of 10 clock cycles.
In fast PWM the Output Compare Register A sets the period.
It counts from 0 to OCR0A inclusive.
For 10 clock cycles we need 0…9 so we set OCR0A accordingly.
```
	OCR0A = 9
``` 
The the duty cycle is controlled with Output Compare Register B.
??Where a duty cycle of 0 corresponds to a duty cycle of 1/10 (1µs).??
```
	OCR0B = x; // x = 0...9 ?
```
Because fast PWM uses both Output Compare Registers only one pin can be used.
Most Arduino tutorials get this wrong but I found one that describes it well called [Fast PWM on ATmega328, up to 8MHz][5]




Now comes the fun part.
To shape the waveform with the data that we want to transmit we need assembly code that runs in sync with the Timer/Counter 0.

There are four parts to consider:

1. How to start the transmission and transition into 2.
2. Transmit each bit in a byte.
3. Load the next byte and transition into 2.
4. Stop the timer at the end of all bytes.


### Entry ###

The waveform is produced the next clock cycle as soon as we start the Timer/Counter by setting the Clock Select Bit in Timer/Counter Control Register B.
If we set the duty cycle after that, we are already off by one clock cycle.
So we have to set the duty cycle for the first bit prior to starting the clock.
Before that we also need to load the first byte and look at the first bit.
Luckily there is an instruction to load a byte from memory into a register and also advance the pointer to the memory location at once.
There is no need to advance the memory pointer by hand.
To look at the first bit (and any successive bit) the easiest way is to move the highest bit into the carry flag of the Status Register (SREG).
With the bit sitting in the carry flag you can use the conditional branching instructions.
So the entry sequence is this:
```
	LDI %[lo], 1			; store the duty cycle for 0-bit in register lo
	LDI %[hi], 7			; store the duty cycle for 1-bit in register hi
	LDI %[start], 0x9		; store the selected clock source in register start
	LDI %[stop], 0x0		; store no clock source in register stop
	LD  %[byte], %a[ptr]+		; load first byte and advance pointer
	LSL %[byte]			; shift the highest bit into the carry flag
	XXX				; do something according to the carry flag
	OUT %[OCR0B], %[(lo|hi)]	; set the Output Control Register B to the desired duty cycle
	OUT %[TCCR0B], %[start]	; start the timer by setting the Timer/Counter Control Register B
```
We don't need to count clock cycles until we start the Timer/Counter.
At the last line we start counting the used clock cycles.
In good tradition we use zero based counting.


### Bit by bit ###

Now we have 9 clock cycles to prepare the next bit.
In that time we have just to shift in the next bit into the carry flag and set the duty cycle accordingly.
```
	LSL %[byte]			; shift the next bit into the carry flag
	XXX				; do something according to the carry flag
	OUT %[OCR0B], %[(lo|hi)]	; set the Output Control Register B to the desired duty cycle
```
Now we have to think about how we can choose the right duty cycle.
One possibility would be to use another register and store the duty cycle there and use it then.
```
next_bit:
	LSL %[byte]			; shift the next bit into the carry flag
	BRCS bit_n_one			; look at the carry flag and jump to bit_n_one if it is set
	MOV %[duty] %[lo]		; copy the lo register
	RJMP set_duty			; go to set duty cycle
	
bit_n_one:
	MOV %[duty] %[hi]		; copy the hi register

set_duty:
	OUT %[OCR0B], %[duty]		; set the Output Control Register B to the desired duty cycle
```
Not every instruction takes one clock cycle.
For this [ATmega328P megaAVR Data Sheet][3] and [AVR Instruction Set Manual][4] are the source of truth.
Noteworthy is that branch and jump instructions use 1 clock cycle if the jump is not made and 2 clock cycles otherwise.
So the cycle counting looks like this:
```
					; 1-bit	0-bit clock cycle
bit_n:
	XXX				; 1…4	1…4
	LSL %[byte]			; 5	5
	BRCS bit_n_one			; 6/7	6
	MOV %[duty] %[lo]		; 	7
	RJMP set_duty			; 	8/9
	
bit_n_one:
	NOP				; 8
	MOV %[duty] %[hi]		; 9

set_duty:
	OUT %[OCR0B], %[duty]		; 0	0
```
We see that 6 clock cycles are required to set the duty cycle depending on the bit value.
To stay in sync one case has to be delayed by one clock cycle with a NOP.
So we have 4 clock cycles spare in each bit.
Instead of wasting those cycles we can use it to count down the bits, loop over that section and jump out for for fetching the next byte.
```
	LDI %[bits], 0x8				; set the bit counter to 8

bit_n:
	DEC %[bits]			; 3	3	; decrement the bits
	BREQ next_byte			; 4	4	; jump not next byte section if the counter reaches zero
	LSL %[byte]			; 5	5	; shift the next bit into the carry flag
	BRCS bit_n_one			; 6	6/7	; look at the carry flag and jump to bit_n_one if it is set
	MOV %[duty] %[lo]		; 7		; copy the lo register
	RJMP set_duty			; 8/9		; go to set duty cycle

bit_n_one:
	NOP				;	8	; synchronize both cases
	MOV %[duty] %[hi]		;	9	;copy the hi register

set_duty:
	OUT %[OCR0B], %[duty]		; 0	0	; set the Output Control Register B to the desired duty cycle
	RJMP bit_n			; 1/2	1/2	; process the next bit

next_byte:
```
Great, we can process each bit in a byte with a loop and waste to cycles.


### Fetch next byte ###

This is the tricky part.
While bit 8 is being processed, we have to decrement the size and see if we are done, load the next byte if not so, set the bit counter back to 7 and set the duty cycle to the right value and stay in sync.
Easy, huh?
Let's look at the required instructions and the clock cycles:
```
set_duty:

next_byte:
	DEC %[size]		; 1	1	; decrement the count of bytes to process
	BREQ end		; 2	2	; jump to the end if the size reaches zero 2, else 1
	LD %[byte]		; 3/4	3/4	; load the next byte
	LDI %[bits], 0x8	; 5	5	; set the bit counter back to 8
	LSL %[byte]		; 6	6	; shift the next bit into the carry flag
	BRCS bit_hi		; 7	7/8	; branch to the hi-bit case
	MOV %[duty] %[lo]	; 8		; copy the lo register
	RJMP set_duty		; 9/10		; go to set duty cycle

bit_hi:	
	MOV %[duty] %[lo]	; 	9	; copy the lo register
	RJMP set_duty		; 	10/11	; go to set duty cycle

end:
```
Well, no. We are off-by-one in the case that ne first bit of the next byte is a zero.
And even off-by-two in case of a one-bit.
And we didn't even take into account that we enter `next_byte` at clock cycle 6.
The solution I came up with also uses the clock cycles were used to prepare bit 8 and while presenting bit 8 to the output.
```
	LDI %[bits], 0x7

bit_n:
	DEC %[bits]
	BREQ bit_8
	LSL %[byte]
	BRCS bit_n_one	
	MOV %[duty] %[lo]
	RJMP set_duty	

bit_n_one:
	NOP			
	MOV %[duty] %[hi]

set_duty:
	OUT %[OCR0B], %[duty]
	RJMP bit_n

bit8:
	LDI %[bits], 0x7
	LSL %[byte]
	BRCS bit8_one
	NOP

bit8_zero:
	OUT %[OCR0B], %[lo]
	DEC %[size]
	BREQ end
	LD %[byte]
	LSL %[byte]
	BRCS bit_n_one
	MOV %[duty] %[lo]
	RJMP set_duty	

bit8_one:	
	OUT %[OCR0B], %[hi]
	DEC %[size]
	BREQ end
	LD %[byte]
	LSL %[byte]
	BRCS bit_n_one
	MOV %[duty] %[lo]
	RJMP set_duty	

end:
```
This time I intentionally omitted the clock cycle count and make it an exercise for the reader.
Now that works but I don't like that OUT %[OCR0B] is sometimes set by the %[duty] register and sometimes directly with the %[hi] and %[lo] registers.
Also there is much code duplication because I could not afford a branch or jump instruction.
But we will clean that up afterwards.
Note that the %[bits] now only counts 7 bits.


### End of sequence ###

Here is not much work to do.
You just have to wait until the clock count reaches 9 and then you stop the timer.
```
end:
	RJMP .+0    		; NOP NOP
	RJMP .+0    		; NOP NOP
	RJMP .+0    		; NOP NOP
	OUT %[TCCR0B], %[stop]	; stop the timer by setting the Timer/Counter Control Register B
```
Here we are using a cheat to delay two clock cycles but use only one instruction and thus save some bytes of program space.



## Final result ##






## Conclusion ##

The assembly provided here is not faster or smaller than the assembly of [Just in Time][1].
Actually it must not be faster as it has to be cycle accurate.
I am pretty sure someone more experienced can do this with less code.
But the assembly is much easier to understand.
This benefits me especially, as I am new to writing assembly in general and for AVR in particular.
The generated waveform can be changed easily without rewriting the entire assembly.
???? With a slight modification to the assembly it is also possible to generate manchester code. ????


Of course the simplicity comes with some cost.
Using the hardware capabilities binds some hardware resources, a Timer/Counter in this case.
?????? This also limits the pins that can be used to one PWM pin as the other PWM pin that is attached to the Timer/Counter is just the inverted output.????




## Follow up ##

I am pretty sure it is possible to transmit data even faster as 10 clock cycles.
[Just in Time][1] was able to bit bang the data which requires to toggle the pin twice a bit.
the fast PWM approach only needs to set a register once per bit.
It seems obvious that 9 clock cycles per bit should be feasible.
That is why in the next article I will explore how fast we can get with serial data transmission.



## Acknowledgement ##

During the development I stumbled over weird compiler optimizations.
Every time the named inputs used the same value, they got mapped to the same physical register.
This is clearly not what I wanted because one named input gets modified while the other does not.
I reached out to the [avr-gcc mailing list][6] because I did not understand why it was happening and I thought it was a defect in the compiler optimization as there was no issue when compiling with `-O0`.
In the [thread][7] the nice people Joseph C. Sible and David Brown helped me to understand that values are passed in by value and there need not be a correlation between the C variables and registers in the assembly.
Using variables as input AND output helps establish some barrier between the values and registers such that they don't get mixed up.


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



### Header 3 (; keep two spaces to top) ###


Separating paragraphs. Paragraph 1.

Paragraph 2.


[Link](http://example.com/)


** Strong **


_ Emphasized _ 


~~Strike through~~ (must not have a gap!)


Unordered list:
* Item 1
* Item 2

    with second paragraph
* Item 3
        with code block
* Item 4
* Item 5


Inline code: `printf("%d\n", 42)`


Code block (without any syntax):
    #include <stdio.h>
    #include <string.h>


Code syntax block:
``` objc title http://kwasi-ich.de kwasi-ich
[rectangle setX: 10 y: 10 width: 20 height: 20];
```
supports bahs, objc, js, css and so on


Image: ![img](image.png)

Image: ![img](/images/dummy2.png)

Image: {% asset_img image.png %}

Image: {% img /images/dummy2.png %}


Asset path: {% asset_path image.png %}
