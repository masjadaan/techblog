---
layout: single
title: "CBC Bit-Flipping Attack"
date: 2024-01-06
categories: [Cryptography]
tags: [cbc, block-cipher, bit-flipping, cryptography]
author_profile: true
toc: true
toc_sticky: true
read_time: true
show_date: true
---

## Introduction
Hey everyone, in the previous article, we discussed the Oracle Padding Attack against Block Cipher algorithms, particularly when operating under Cipher Block Chaining (CBC) mode. In this article, we will delve into the CBC Bit-Flipping Attack. It's important to understand that exploiting vulnerabilities in cryptographic implementations could lead to outcomes like content decryption, privilege escalation, or even key recovery. Let's start with a brief introduction to Block Ciphers.

## Block Cipher
In block ciphers encryption, data is encrypted one block at a time, with the block length varying across different algorithms. If the length of the data being encrypted is not a multiple of the block length, then padding becomes necessary. When examining encrypted data in block ciphers, pinpointing the exact length of the original data, without padding, proves to be challenging. However, we can calculate the minimum and maximum lengths, taking into account the padding.

Consider an example with a block size of 8 bytes and a ciphertext data length of 64 bytes. The question arises: What is the length range of the plaintext data (without padding)?

- First, let's calculate the number of blocks:
```
Nr. Blocks = (64 bytes ciphertext) / (8 bytes per block) = 8 blocks
```
We have 8 blocks, which implies two scenarios. The first scenario occurs when the plaintext data is a multiple of 8 and occupies the entire 8 blocks, representing the maximum length of the plaintext.
 ```
Plaintext length = (8 blocks) * (8 bytes per block) = 64 bytes
 ```

 In the second scenario, the plaintext data is not a multiple of 8, filling the entire 7 blocks with only one byte of the 8th block, while the rest consists of padding data. This scenario represents the minimum length.
```
Plaintext length = (7 blocks * 8 bytes per block) + 1 byte = 57 bytes plaintext data, with the rest being padding.
```

Several well-known algorithms use block ciphers, including:
- Advanced Encryption Standard (AES)
- Data Encryption Standard (DES)
- Triple Data Encryption Standard (3DES)
- Blowfish
- Twofish

## Block Ciphers Modes
Another important aspect of block ciphers is the mode of operation. Since block ciphers operate on fixed-size blocks of data, challenges arise when dealing with plaintext data that exceeds the block length. Various modes of operation address this issue, allowing for the encryption and decryption of messages of different lengths. Some commonly used modes include:
- Electronic Codebook (ECB) Mode
- Cipher Block Chaining (CBC) Mode
- Counter (CTR) Mode

Any algorithm can employ these modes; for instance, you might come across AES-CBC or DES-CBC. In the context of this attack, our focus is on Cipher Block Chaining (CBC) mode.

### Cipher Block Chaining (CBC) Mode
In general, CBC uses two main operations, the encrypting algoritm operation and the XOR operation with each block to add randomness to each encryption operation, this prevent the presence of duplicate blocks. Let's examine in a bit more detail how encryption/decryption occurs when using CBC mode. 

#### Encryption
Alright, take a look at the image below. What's happening is that the ciphertext of the previous block gets XORed with the plaintext of the next block just before encryption kicks in. So, essentially, the ciphertext block becomes the input for the next encryption algorithm:
```
(Ciphertext data block n) ^ (Plaintext data block (n+1))
```
However, there is a little twist in the process for the first block. For the first block, we use an Initialization Vector (IV) which gets XORed with the first plaintext block:
```
(IV) ^ (Plaintext data block 1)
```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Cryptography/CBCBitFlipAttack/images/encryptoin.png)

#### Decryption
Let's talk about decryption now. The ciphertext of the previous block is XORed with the output of the decrypting algorithm of the next block. In simpler terms:
```
(Ciphertext data block n) ^ (Decrypting Algorithm output block (n+1))
```
But again, there's a special case for the first block. The Initialization Vector (IV) comes into play, and it's XORed with the output of the decrypting algorithm for the first ciphertext data block:
```
(IV) ^ (Decrypting Algorithm output block 1)
```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Cryptography/CBCBitFlipAttack/images/decryption.png)


## CBC Bit-Flipping Attack
Now that we've covered the basics of CBC, let's explore the attack CBC Bit-Flipping. As we mentioned: the ciphertext of the previous block has a direct impact on the resulting plaintext of the next block. So, what if we manipulate the ciphertext at the bit level? Can we exploit this situation?

The answer is a cautious "maybe." You see, the tricky part for attackers is figuring out how these changes affect the decrypted text. Once they figure that out, it becomes a game of trial and error to manipulate the ciphertext and produce the desired arbitrary value.

To illustrate this attack, I've put together a simple web application using Python and the Flask framework. This app employs the AES algorithm with CBC mode to encrypt text. For the sake of our demonstration, I've used a fixed IV and Key (you can find all the code on my GitHub).

As you launch the app, you'll see the original values: Original Text, Original IV, and Original Encrypted Text. Beneath that, there are modified values: Modified IV and Decrypted Text. In this initial run, the "Original Text" and "Decrypted Text" are identical.
```
./app.py
```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Cryptography/CBCBitFlipAttack/images/userid9.png)

Now, here's where it gets interesting. The "Original Text" contains a userid encrypted by the app using AES-CBC. Imagine this userid plays a crucial role in the application, perhaps for user ID checks before granting access or escalating privileges. The attack strategy is to modify the encrypted text so that the userid is changed to 0, assuming it's an admin userid.

I've also written a function to manipulate the IV by simply XORing a bit at a specific position with a value of our choosing.
```python
def bit_flipping(ciphertext, position, xor_value):
    """
    Modifies the encrypted text by XORing a specified value at a specified position.

    :param ciphertext: (bytes) The input ciphertext.
    :param position: (int) The position in the ciphertext where XOR should be applied.
    :param xor_value: (int) The value to XOR at the specified position.
    :return: (bytes) The modified ciphertext.
    :raises ValueError: If the position or xor_value is not of type 'int' or if the position is invalid.
    :raises Exception: If an error occurs during bit flipping.
    """
    try:
        byte_list = bytearray(ciphertext)
        if not isinstance(position, int) or not isinstance(xor_value, int):
            raise ValueError("Position and xor_value must be integers")

        if position < 0 or position >= len(byte_list[16:]):
            raise ValueError("Invalid position")
        byte_list[position] ^= xor_value
        return bytes(byte_list)
    except Exception as e:
        print(f"Bit flipping error: {e}")
        raise
```

Since I don't know which byte in the Initialization Vector needs changing, I randomly selected position 7 and XORed it with another random value, 7.
```python
data = bit_flipping(ciphertext=data, position=7, xor_value=7)
```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Cryptography/CBCBitFlipAttack/images/usbrid.png)

After refreshing the web page, we observe the result: the word "userid" transforms into "usbrid" (you'll also notice the modification in the IV).

Clearly, I've picked the wrong position, so let's keep increasing it until we overwrite the "9" in "userid." Voila! It happens at position 13, and the userid "9" is decrypted to ">."

```python
data = bit_flipping(ciphertext=data, position=13, xor_value=7)
```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Cryptography/CBCBitFlipAttack/images/userid%3E.png)

We've determined the position changes, and now we need to fine-tune the xor_value parameter in the bit_flipping() function until we get zero. Actually, this occurs when the xor_value is equal to 9, as depicted in the image below. Once the web application decrypts the ciphertext, the userid value magically transforms into 0.
```python
data = bit_flipping(ciphertext=data, position=13, xor_value=9)
```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Cryptography/CBCBitFlipAttack/images/userid0.png)

## Let's do some Math
Now, let's break down the magic. Remember how we mentioned earlier that during decryption, the output of the Decrypt Algorithm is XORed with the IV? Well, the first time we messed with the user id, we manipulated the IV at position 13 with a value of 7. At position 13, we find the value 4, and its ASCII value is 0x34. We XORed it with 0x7 and got 0x33, which, in ASCII, is the number 3. That's why we got 3 in the modified IV:
```
0x34 ^ 0x07 = 0x33 -> ASCII 3
```

When we used 0x33 with the output of the Encrypted Algorithm, we got the plaintext ">", which in ASCII is 0x3E. Now, here comes the fun part, one of the XOR property says that if
```
a ^ b = c, then c ^ b = a and c ^ a = b
```

So, we can use this property to find the value of the Encrypted Algorithm output:
```
0x33 ^ 0x?? = 0x3E =>
0x33 ^ 0x3E = 0xD -> this is the output of Encrypted 
```

Fantastic! Our goal is to get zero in plaintext. In ASCII, zero is 0x30. What's the IV value that, when XORed with 0xD, will give us zero? Let's use the XOR property again:
```
# now we need the plaintext to become 0
0xD ^ 0x?? = 0x30 =>
0xD ^ 0x30 = 0x3D -> in ASSCI this is =
```
One last step—we know we need 0x3D in the IV. The question now is, what's the xor_value?
```
0x34 ^ 0x?? = 0x3D
0x34 ^ 0x3D = 0x9
```

And there you have it—mathematically achieving our goal of making the plaintext zero. Cryptography meets a bit of algebraic magic!


## Appendix
### Usage instructions
To begin, clone the GitHub repository to obtain the code. Next, navigate to the "application" folder. To launch the web application, execute ./app.py. This will initiate the Flask HTTP server, accessible at http://127.0.0.1:5000. Open your browser and visit this URL to access the main page of the web application.

To follow the steps outlined in this article, open app.py in any text editor, preferably PyCharm. Locate the bit_flipping function. Each time you modify the values of position or xor_value, simply refresh the web application page to observe the changes.

```sh
git clone https://github.com/masjadaan/TechSecurityArticles.git
cd TechSecurityArticles/Cryptography/CBCBitFlipAttack/application
./app.py 

# in a browser navigate to
http://127.0.0.1:5000/
```


Happy Learning <br>
Mahmoud Jadaan
