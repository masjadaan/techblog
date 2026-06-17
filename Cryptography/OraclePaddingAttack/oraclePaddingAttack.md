---
layout: single
title: "Oracle Padding Attack"
date: 2024-01-06
categories: [Cryptography]
tags: [cbc, padding-oracle, pkcs7, cryptography]
author_profile: true
toc: true
toc_sticky: true
read_time: true
show_date: true
---

## Introduction
The Oracle Padding Attack is a type of cryptographic attack targeting Block Cipher algorithms, especially when operating under Cipher Block Chaining (CBC) mode. In this context, the term 'oracle' refers to a system that discloses information about whether the padding of a cryptographic message is correct or not. While our goal is to provide a concise overview, we will refrain from delving too deeply into theory. Instead, we will present the essential concepts necessary for understanding how the attack works.

## Block Cipher
Within the domain of block cipher encryption, data undergoes encryption one block at a time, with the block length varying among different algorithms. When the length of the data intended for encryption is not a multiple of the block length, padding becomes necessary. However, when examining encrypted data in block ciphers, determining the precise length of the original data is not a straightforward task.

Several well-known algorithms employ block ciphers, including:
- Advanced Encryption Standard (AES)
- Data Encryption Standard (DES)
- Triple Data Encryption Standard (3DES)
- Blowfish
- Twofish

## Padding Scheme
As previously mentioned, block ciphers employ a fixed-size block, necessitating padding when the plaintext is not a multiple of the block size. Various techniques exist for padding, but in this attack, our focus lies on PKCS#7.

### PKCS#7
Public Key Cryptography Standards #7 (PKCS#7) stands out as a widely recognized cryptographic standard defining a padding scheme used to pad the last block of plaintext before encryption in block cipher modes requiring a fixed block size.

To understand the workings of this padding, consider an example with a block size of 8 bytes and the words "Exploit, Attack, Cyber, Hack" requiring padding. The image below illustrates how padding is applied to these words. If a word's length is less than the block size, padding is added until the word reaches the exact block size length, and the value of the padding is the number of bytes added. For instance, "Exploit" needs only one byte, and the value of the padded byte is 0x01. However, "Hack" needs 4 bytes of padding, with the value of the padding byte being 0x04.
![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/Cryptography/OraclePaddingAttack/images/padding1block.png)

In another scenario, when a word's length matches the block size exactly, a new block is added, and the value of the padded bytes is set to 0x08, as demonstrated when encrypting the word "Standard."
![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/Cryptography/OraclePaddingAttack/images/padding2blocks.png)

While the concept of padding may seem straightforward, let's delve into another essential concept.

## Block Ciphers Modes
Another important aspect of block ciphers is the mode of operation. Since block ciphers operate on fixed-size blocks of data, challenges arise when dealing with plaintext data that exceeds the block length. Various modes of operation address this issue, allowing for the encryption and decryption of messages of different lengths. Some commonly used modes include:

- Electronic Codebook (ECB) Mode
- Cipher Block Chaining (CBC) Mode
- Counter (CTR) Mode

Any algorithm can employ these modes; for instance, you might come across AES-CBC or DES-CBC. In the context of this attack, our focus is on Cipher Block Chaining (CBC) mode.

### Cipher Block Chaining (CBC) Mode
In general, CBC uses two main operations: the encrypting algorithm operation and the XOR operation, which is used to add randomness to each encryption operation to prevent the presence of duplicate blocks. Let's examine in a bit more detail how encryption/decryption occurs when using CBC mode.

#### Encryption
Take a look at the image below. What's happening is that the ciphertext of the previous block gets XORed with the plaintext of the next block just before encryption kicks in (First XOR, then encryption). So, essentially, the ciphertext block becomes the input for the next encryption algorithm:
```
(Ciphertext data block n) ^ (Plaintext data block (n+1))
```

However, this is not the case for the initial block because it doesn't have a prior block. For this reason, we use what is called an Initialization Vector (IV), which gets XORed with the first plaintext block:
```
(IV) ^ (Plaintext data block 1)
```
![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/Cryptography/OraclePaddingAttack/images/cbc_encryption.png)

#### Decryption
Now, regarding decryption, the ciphertext of the previous block is XORed with the output of the decrypting algorithm of the next block (First decryption, then XOR). In simpler terms:
```
(Ciphertext data block n) ^ (Decrypting Algorithm output block (n+1))
```

But again, there's a special case for the first block, and here the Initialization Vector (IV) comes into play. It is XORed with the output of the decrypting algorithm for the first ciphertext data block:
```
(IV) ^ (Decrypting Algorithm output block 1)
```
![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/Cryptography/OraclePaddingAttack/images/cbc-decryption.png)


Now that we've discussed the essential concepts, let's delve into the attack itself.

## Oracle Padding Attack
I will illustrate the attack through an example, aiming to make it easier to follow and comprehend. I've developed a server-side application (our Oracle) that performs encryption/decryption operations using AES-CBC mode. Additionally, when this server-side application receives data from the client for decryption, it responds with an error indicating either correct or incorrect padding.

In addition, I've created a client that communicates with the backend application. This client has encrypted data from the server, let's assume it's a cookie containing secret information used in each request to communicate with the server. Below is a snippet from the code.
```python
if __name__ == '__main__':
    encrypted_data = b'31323334353637383930313233343536f044039223b4b9aea7bc48cd1be80682'
    oracle_padding(ciphertext=encrypted_data)
```

As we lack the secret key, decrypting this cookie is currently beyond our reach. Given that the encryption algorithm is AES, signifying a block size of 16 bytes, the Initialization Vector (IV) is also 16 bytes. The client is equipped with a function designed for the Oracle Padding Attack. Our objective is to decrypt the encrypted data and unveil the plaintext.

**Note**: *In many implementations, the IV is transmitted with the message. However, in our scenario, the initial 16 bytes in "encrypted_data" represent the IV values, while the remainder constitutes the encrypted message (cookie).*

For simplicity, this attack focuses on a single block. Let's explore the decryption process for one block. 
![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/Cryptography/OraclePaddingAttack/images/1blockDecryption.png)

The ciphertext is initially decrypted by AES using the hidden secret key on the server, generating an intermediate value, referred to as the keystream. Subsequently, this keystream is XORed with the IV to produce the plaintext. For an attacker, the keystream is unknown, or is it?

Actually, this attack revolves around the concept of recovering the keystream value. Once obtained, it becomes straightforward to deduce the plaintext without knowledge of the secret key.

In essence, the attack comprises two phases. In the first phase, we recover the keystream bytes, and in the second phase, we retrieve the plaintext bytes.

### Phase 1: Recovering Keystream Bytes
To achieve this, we manipulate the original IV. Knowing that the encrypted data comprises the IV and the original message, we employ our own modified IV. Initially, we use an IV with all zero values for our first attempt. In the code snippet below, you can observe the extraction of cookies from the encrypted data, with the addition of our new modified IV containing all zeros.
```python
# Original Ciphertext
original_ct = bytearray(unhexlify(ciphertext))
original_iv = original_ct[:BLOCK_SIZE]
cookies = original_ct[BLOCK_SIZE:]

modified_iv = bytearray(b'0000000000000000')

# modified ciphertext
modified_ct = modified_iv + cookies
```

Upon sending the modified cookie to the server for decryption, an error is returned, signaling a padding error. This failure indicates that the server checks the last byte of the plaintext and identifies it as an invalid padding byte.
![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/Cryptography/OraclePaddingAttack/images/allzerosIV.png)

As our first attempt has failed, let's refine our approach. Focusing on one byte at a time, we begin with the last byte of the IV (the 16th byte of IV). Incrementing it by one, we resend the modified cookie for decryption. Once again, an error is received, indicating invalid padding. We repeat this process until the server no longer sends a padding error. When this occurs, it signifies correct padding, revealing that the last byte of the plaintext has the byte value 0x01, in line with the earlier-discussed padding scheme.

Now armed with the knowledge of the IV byte value that avoids a padding error and the corresponding plaintext value, computing the last byte of the keystream becomes a straightforward XOR operation.
```
Keystream[15] = IV[15] ^ 0x01
Keystream[15] = 0x35 ^ 0x01 = 0x34
```
![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/Cryptography/OraclePaddingAttack/images/lastbyteIV.png)

Next, we repeat the same process for the second to last byte of the IV (the 15th byte of IV), and the challenge is the same we need to guess IV values and send them to the server until we no longer encounter a padding error. However, the difference now is that we require the last two bytes of the plaintext to be 0x02 and 0x02. For the last byte, we can solve it with a simple XOR operation.
```
IV[15] ^ Keystream[15] = 0x02
IV[15] ^ 0x34 = 0x02 => IV[15] = 0x36
```

However, for the second to last byte, we need to try all possible values. Starting at 0x00 and incrementing it until no error is encountered, we find that the IV byte is 0x35. Consequently, we compute the keystream bytes.
```
Keystream[14] = IV[14] ^ 0x02
Keystream[14] = 0x35 ^ 0x02 = 0x37
```
![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/Cryptography/OraclePaddingAttack/images/last2bytesIV.png)

This process is repeated until all the keystream bytes are recovered. Once accomplished, Phase 1 concludes, and we proceed to Phase 2.

### Phase 2: Recovering Plaintext
Phase 2 is straightforward; now armed with the original IV and having successfully recovered all the keystream bytes, all that remains is to XOR them to unveil the plaintext values. Let's begin with the last byte:
```
Plaintext[15] = IV[15] ^ Keystream[15]
Plaintext[15] = 0x36 ^ 0x34 = 0x2 -> padding no ASCII
```

Moving on to the second to last byte:
```
Plaintext[14] = IV[14] ^ Keystream[14]
Plaintext[14] = 0x35 ^ 0x37 = 0x2 -> padding no ASCII
```

For the third to last byte:
```
Plaintext[13] = IV[13] ^ Keystream[13]
Plaintext[13] = 0x34 ^ 0x15 = 0x21 -> convert to ASCII !
```

And for the fourth to last byte:
```
Plaintext[12] = IV[12] ^ Keystream[12]
Plaintext[12] = 0x33 ^ 0x57 = 0x64 -> convert to ASCII d
```


This process continues until we recover the entire plaintext.
![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/Cryptography/OraclePaddingAttack/images/plaintext.png)

That's the essence of how the Oracle Padding Attack works. Cryptography is fascinating, isn't it?
![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/Cryptography/OraclePaddingAttack/images/cover.png)

## Appendix A
If you wish to observe the decryption of the ciphertext, both the server and client applications are available on my [GitHub repository](https://github.com/masjadaan/TechSecurityArticles/tree/main/Cryptography/OraclePaddingAttack).

The server application establishes a Flask web application with an endpoint "/process" designed to handle POST requests. The endpoint anticipates data in the request body and examines the value of the 'Is-Encrypted' header. If the header is set to 'true,' the received data is presumed to be ciphertext and is decrypted using the AesCbc class from cbc.py. Conversely, if the header is set to 'false,' the received data is assumed to be plaintext and is encrypted using the same class. The implementation for both encryption and decryption is detailed in the cbc.py file.

Feel free to clone the server application and run it, allowing it to await requests from the client.
```
git clone 
pip install Flask pycryptodome requests
./server.py
```

The client application interacts with the server for tasks such as encryption, decryption, and the execution of a padding oracle attack.
Upon cloning the client application, execute the client.py file, and you will see the decryption process.
```
git clone
pip install requests
./client.py
```
For experimenting with various messages beyond "Hello, Friend!" you can modify the code at the bottom of the client. Simply comment out the existing code, run the client, acquire the new encrypted message, and utilize it to perform the attack once again.


Happy Learning... <br>
Mahmoud Jadaan

#### Resources
- White Paper: [Security Flaws Induced by CBC Padding Applications to SSL, IPSEC, WTLS](https://www.iacr.org/cryptodb/archive/2002/EUROCRYPT/2850/2850.pdf) by Serge Vaudenay
