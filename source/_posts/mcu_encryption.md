---
title: Strong encryption for small payloads on Arduino
author: Michael Kwaśnicki
date: 2018-03-05
tags: [Arduino, Encryption]
photos: rc4d.svg
---

Most time, the wireless communication between embedded devices like the Arduino happens in plain text. While block ciphers like AES are too bulky in terms of speed, payload and object code size, stream ciphers like RC4 do not offer a high level of security through the lack of diffusion. In this post I will describe a new method to use a modified stream cipher that offers diffusion and also works with no overhead on small payloads, that are common in such devices.


<!-- more -->


## Objective ##

In some of my projects I want to use wireless communication between multiple embedded devices. Some applications like smart-home might contain sensitive data or might be susceptible to attackers that might gain control over every device. So I would like to encrypt the messages in a simple but secure way. Currently AES is state of the art but is not simple and comes along with some constraints. Yet it offers a high degree of diffusion. RC4 on the other side is very simple and fast but does not offer the same level of security. For example, there is no diffusion at all.

What I want is an encryption that is effective and offers a high diffusion like AES but works on payloads of arbitrary length without extra cost and ais simple and fast like RC4. But in my research I found no cipher that offers everything I want.

What is this diffusion anyway?

[Diffusion][wiki_diffusion] describes how the output ciphertext differs if only one bit/byte in the input plaintext changes. So the change might affect only this one bit/byte or also its neighbors, its containing block or the entire ciphertext. The higher the diffusion the more unpredictable is the ciphertext.


### Why not AES ###

The [Advanced Encryption Standard][wiki_AES] (AES) is a block cipher. That means that the cipher operates on a block of plaintext with a fixed size. For AES, that is 128 bits or 16 bytes. AES has three sub types where the key length is 128, 192 or 256 bits but the block size stays the same. AES scatters the plaintext within that block and scrambles it in such a way that changing only 1 bit results in a completely different resulting block of ciphertext.

<figure>
    {% asset_img aes.svg %}
    <figcaption>A small change to the plaintext results in a completely different ciphertext in AES.</figcaption>
</figure>

For data whose length is not a multiple of a block, the last block needs to be padded. The padded block needs to be transferred completely along with the information how many bytes have been padded. So there is an overhead that needs to be transferred in order to obtain the original plaintext. Also the algorithm is so complex that it takes up much of the scarce space on the embedded device and also takes more time to process the data.


### Why not RC4 ###

[Rivest Cipher 4][wiki_RC4] (RC4) is a stream cipher. This category of ciphers is intended for use with streams of arbitrary length. A stream cipher generates a pseudo random stream from a key of 40 – 2048 bits, that is XOR'ed with the plaintext. This is simple and fast but also has its flaws. There is no diffusion. Changes to the plaintext are local. So when using the same key all over again, the content will be successfully attacked on the long run.

<figure>
    {% asset_img rc4.svg %}
    <figcaption>A small change to the plaintext results in a change to the same bit in the ciphertext in RC4.</figcaption>
</figure>

But changing the key after every message bears dangers as the sender and receiver have to agree for the key to use. Getting out of sync leads to unreadable messages. Beside that RC4 is considered broken and was removed from all official recommendations. Yet it is so easy to implement and so fast that it is perfectly suitable for embedded devices.



## A new hope for RC4 ##

Block ciphers know different [operation modes][wiki_modes] in order to prevent attacks on long payloads. There are also operation modes that allow block ciphers to be used like a stream cipher. The block cipher generates just a key stream. When looking at the [Electronic Codebook][wiki_modes_ECB] (ECB) operation mode, we see analogies to RC4. Each block of plaintext is being encrypted individually with the same key, as in RC4 each byte is being encrypted with a stream generated by the key.

The only difference is that RC4 decryption is also just the RC4 encryption applied a second time.

    C(i) = K(i) ⊕ P(i)  ⇒  P(i) = C(i) ⊕ K(i)

In order to generate diffusion we just have to look at an operation mode for block ciphers that works as a stream cipher and propagates changes in the plaintext throughout the ciphertext. And indeed [Cipher Feedback][wiki_modes_CFB] (CFB) does exactly that. The plaintext is XOR'ed with a key stream that depends on the previous block of the ciphertext. Bringing it over to RC4 that means that the current byte of plaintext is XOR'ed with a key that depends on the previous byte of ciphertext. So instead of one bit flip at one position, the bit flip is propagated through the end of the ciphertext.

    C(0) = K(0) ⊕ IV ⊕ P(0)
    C(i) = K(i) ⊕ C(i-1) ⊕ P(i)   (i > 0)

`IV` denotes the [initialization vector][wiki_IV] which is just one hand picked constant byte that is used in lack of a ciphertext byte at position -1.

Chaining successive bytes that way breaks the symmetry between encryption and decryption of RC4. Before we could simply swap the ciphertext with the plaintext. Now we have a dependency to the previous byte of the ciphertext.

    C(i) = K(i) ⊕ C(i-1) ⊕ P(i)  ⇒  P(i) = C(i) ⊕ C(i-1) ⊕ K(i)

But this is not a big deal as one just has to carry the ciphertext from the previous iteration.

While the key stream is completely different, this modification is still not good enough. One bit flip in the plaintext is reflected as exactly the same bit flipped in every single successive byte of the ciphertext. To overcome this, we use the constantly changing S-Box of RC4 to mix it up even more.

    C(0) = K(0) ⊕ S[IV] ⊕ P(0)
    C(i) = K(i) ⊕ S[C(i-i)] ⊕ P(i)   (i > 0)

That way the ciphertext after that one bit flip changes in a way that cannot be overseen. But there is still a flaw. The ciphertext before the bit flip still did not change. To also improve this, we just reverse the ciphertext and encrypt it again using the same algorithm. This way a single bit flip is propagated forth and back and changes the entire ciphertext which is a level of diffusion that is higher than in block ciphers. So the high level encryption algorithm looks like this.

    key = "secret"
    plaintext = "Hello World!"
    intermediate = rc4_with_cfb_encrypt(plaintext, key)
    intermediate = reverse(intermediate)
    ciphertext   = re4_with_cfb_encrypt(intermediate, key)

While the decryption does the same in the reverse order using the same key but also using `rc4_with_cfb_decrypt` instead of `_encrypt`.

Compared to the original RC4 the code changes in the proposed RC4 encryption with CFB are pretty small.

    RC4 with CFB encryption:                                    RC4:

    s[256]                                                      s[256]
    s[i] := i ∀ i ∈ [0…255]                                     s[i] := i ∀ i ∈ [0…255]

    j := 0                                                      j := 0

    for i from 0 to 255:                                        for i from 0 to 255:
        j := (j + s[i] + key[i mod keyLength]) mod 256              j := (j + s[i] + key[i mod keyLength]) mod 256
        swap s[i] s[j]                                              swap s[i] s[j]
    end                                                         end

    i := 0                                                      i := 0
    j := 0                                                      j := 0

    for a from 0 to inputLength:                                for a from 0 to inputLength:
        i := (i + 1) mod 256                                        i := (i + 1) mod 256
        j := (j + s[i]) mod 256                                     j := (j + s[i]) mod 256
        swap s[i] s[j]                                              swap s[i] s[j]
        K := s[(s[i] + s[j]) mod 256];                              K := s[(s[i] + s[j]) mod 256];

        if a > 0:
            output[a] := K XOR s[output[a-1]] XOR input[a]          output[a] := K XOR input[a]
        else:
            output[a] := K XOR s[IV] XOR input[a]
        end
    end                                                         end

But because of the broken symmetry we need another function for the decryption. It is almost the same as for the encryption with the following small changes.

    RC4 with CFB encryption:                                    RC4 with CFB decryption:

    if a > 0:                                                   if a > 0:
        output[a] := K XOR s[output[a-1]] XOR input[a]              output[a] := K XOR s[input[a-1]] XOR input[a]
    else:                                                       else:
        output[a] := K XOR s[IV] XOR input[a]                       output[a] := K XOR s[IV] XOR input[a]
    end                                                         end

The Rivest Cipher 4 with Diffusion (RC4D) algorithm can still be optimized for use with embedded devices.

* A fixed key size with the length to the power of two (like in AES) replaces the `mod` operation with an `bitwise_and` during the S-Box initialization. (That also applies to all the other `mod` operations.)
* Using an in-place version saves memory.
* The whole code takes up as little as 492 bytes of flash memory.



### Comparison ###

Now we will compare the different ciphers in both execution time and used flash memory on an Arduino UNO R3. The lines annotated with "(opt)" show versions that have to some degree been optimized for the Arduino as opposed to naïve implementations. For comparable results the limitations of AES have been taken as the base. So the key is 128 bits long, while the plaintext with its length of 32 bytes spans two AES blocks. The source code can be found at [GitHub][github].

As part of optimizations, the RC4 and RC4D cipher have been imposed with some limitations from AES to make them both faster and smaller. This also makes the different cipher algorithms more comparable.

Implementations with a __k__ have a fixed key length of 16 bytes (128 bits). This lead to a more then 2x increase in speed and a decent size reduction.

Implementations with an __i__ perform the encryption in-place. This reduces the need for intermediate buffers. This reduces the pressure on the scarce RAM during en-/decryption and also provides a small increase in speed and a small reduction in size.

Implementations with a __p__ perform a precomputation of the S-Box using the key. Due to the aimed small plaintext size, the S-Box is very large compared to it and also the computation thereof. A precomputed S-Box reduces this overhead for every de-/encryption to just one call of `memcpy`. This provides a 3x speedup but comes at the expense of RAM. Moving this to EEPROM does not any good as one can see at the implementation with an __e__.

|      Type       | time _ms_ | space _bytes_ |               note                      |
|-----------------|----------:|--------------:|-----------------------------------------|
| AES             |     11040 |          1972 | occupies additional 562 bytes of RAM    |
| AES (opt)       |     11516 |          1936 |                                         |
| RC4             |      8856 |           394 |                                         |
| RC4 (opt)       |      4340 |           290 |                                         |
| RC4 k           |      1790 |           270 |                                         |
| RC4 k (opt)     |      1700 |           234 |                                         |
| RC4D            |     17619 |          1080 |                                         |
| RC4D (opt)      |      9054 |           674 |                                         |
| RC4D k          |      3719 |           612 |                                         |
| RC4D k (opt)    |      3719 |           612 |                                         |
| RC4D ki         |      3617 |           526 |                                         |
| RC4D ki (opt)   |      3482 |           492 |                                         |
| RC4D kip        |      1101 |           490 | occupies additional 256 bytes of RAM    |
| RC4D kip (opt)  |      1135 |           466 | occupies additional 256 bytes of RAM    |
| RC4D kipe       |      4309 |           724 | occupies additional 256 bytes of EEPROM |
| RC4D kipe (opt) |      4323 |           692 | occupies additional 256 bytes of EEPROM |

The row _time_ shows the measured time for 1024 iterations.

The following table summarizes the features of the different ciphers and implementations that have been compared

|        Feature     |  AES  |  RC4 | RC4 k | RC4D | RC4D k |
|--------------------|-------|------|-------|------|--------|
| Payload [bytes]    |   16  |  any |  any  |  any |   any  |
| Key length [bytes] |   16  |  any |   16  |  any |   16   |
| Diffusion          | block | none |  none | full |  full  |


### Remarks ###

All of the optimizations that have been applied to RC4D can also be applied to RC4. But I leave it as an exercise to the reader as I do not plan to use RC4 anyway. But none of those optimizations can be applied to AES because they are already there, like the fixed key length, or they cannot be applied due to the completely different algorithm.



## Conclusion ##

While derived from a stream cipher, the proposed algorithm RC4D is actually a block cipher with blocks of arbitrary length. Compared to AES, the algorithm is up to 10x faster, takes about only 1/4 of flash space and can be applied to plaintext of any length without padding. Compared to RC4 we traded the arbitrary key length and a 2x longer execution time and space consumption for a higher degree of security through diffusion. Yet RC4D still has potential for optimization in terms of security. For example the two stages during encryption and/or decryption may use different keys to further increase the security.

I am not an cryptographic expert so I can only describe my observations.

The original RC4 algorithm has not been modified. It is just being used in a different mode of operation. So in the worst case the algorithm is just as bad as the original RC4. But as the mode of operation strengthens block cipher one can assume that this is here the case as well.

Diffusion is a highly desirable property of a cipher. And here we have achieved a level of diffusion that affects the entire ciphertext. For large data this can even exceed the level of diffusion in block ciphers.

So in my humble opinion the presented method is much stronger than the original RC4. And due to its simplicity it is suited for use in embedded devices.


[github]: https://github.com/kwasmich/RC4D
[wiki_diffusion]: https://en.wikipedia.org/wiki/Confusion_and_diffusion
[wiki_AES]: https://en.wikipedia.org/wiki/Advanced_Encryption_Standard
[wiki_RC4]: https://en.wikipedia.org/wiki/RC4
[wiki_modes]: https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation
[wiki_modes_ECB]: https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation#Electronic_Codebook_(ECB)
[wiki_modes_CFB]: https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation#Cipher_Feedback_(CFB)
[wiki_IV]: https://en.wikipedia.org/wiki/Initialization_vector