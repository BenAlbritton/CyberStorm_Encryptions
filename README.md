# CyberStorm Encryptions
This folder includes programs worked on during Network Security, or Cyen 301. 

Timelock.py is an example of a password generator. It was used for getting access to a server. The password for the server was changed once a minute during CyberStorm. The password generated from the double md5 hash of seconds from epoch. Two characters from the left are selected and the first two numbers from the right are selected for the final outcome. 

Vigenere.cc is an example of a Caesar Cipher built with a key. A Caesar Cipher is a cipher that shifts the characters of the alphabet a fixed amount. A Vigenere Cipher is built off of the Caesar Cipher - becuase it shifts characters; however, where a Caesar Cipher's shift amount is fixed, viginere's shift is variable based on a key. 

XOR.cc is an Xor cipher. This encryption is formed by xor-ing on a bitwise level. So to use this program, you would take two files of exact character length and xor them. (A xor key = b, b xor key = a)
