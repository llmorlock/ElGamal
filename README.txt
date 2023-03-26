CIS 4634 Assignment 03 (ElGamal).cpp

Program intakes message in hexadecimal, then intakes key size (valid sizes are 2 - 16).
Key size is limited due to my usage of the int datatype for the key.

Primes are generated and verified as prime, then checked as a safe prime.
Is the prime is safe, a generator can be found for it.
Generation method taken from http://groups.csail.mit.edu/mac/classes/6.001/ST98/psets/ps2web/ps2-elgamal/node1.html (section: Finding generators: safe primes).

Program then maps hexadecimal to binary, which is then mapped to decimal numbers within the finite field.
Encryption and decryption occur, then the map is reversed to reveal the original message.