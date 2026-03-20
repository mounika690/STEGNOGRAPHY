# PROJECT TITLE : STEGNOGRAPHY
# NAME : MOUNIKA D
# DATE : 17-03-2026


# Description

This project hides a secret file (like .txt or .c) inside a BMP image using LSB (Least Significant Bit) steganography. It can also decode the hidden secret from the image.

# Features

Encode secret file into a BMP image

Decode secret file from a BMP image

Supports text files

# Technologies

C Programming

File handling

Bitwise operations

Steganography concepts

# How to Run

Compile the project:

gcc *.c -o stego

Encode a secret file:

./stego -e input.bmp secret.txt output.bmp

Decode a secret file:

./stego -d output.bmp
# Concepts Used

Least Significant Bit (LSB) encoding

File I/O operations

Bitwise manipulation

Binary data handling

# Notes

Input image must be a .bmp file.

Secret file can be .txt or .c.

Ensure the image has enough size to store the secret file.
