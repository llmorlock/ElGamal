// CIS 4634 Assignment 03 (ElGamal).cpp

#include <iostream>
#include <string>
#include <math.h>
#include "cryptopp870/integer.h"
#include "cryptopp870/osrng.h"
#include "cryptopp870/nbtheory.h"

using namespace std;
using namespace CryptoPP;

class Keys {
private:
    Integer public_key[3];
    Integer private_key;

public:
    Keys() = default;
    ~Keys() = default;

    /*
    * @param p the prime part of the key
    * @param alpha the generator for the prime
    * @param x = alpha^a (mod p)
    */
    void set_pub_key(Integer p, Integer alpha, Integer x) {
        public_key[0] = p;
        public_key[1] = alpha;
        public_key[2] = x;
    }

    /*
    * @param a single part of the private key
    */
    void set_priv_key(Integer a) {
        private_key = a;
    }

    Integer get_prime() {
        return public_key[0];
    }

    Integer get_alpha() {
        return public_key[1];
    }

    Integer get_x() {
        return public_key[2];
    }

    Integer get_a() {
        return private_key;
    }


};

bool fermat_primality(Integer, int);
bool miller_rabin_primality(Integer, int);
Integer square_and_mult(Integer, Integer, Integer);
Integer prime_facto_MR(Integer, Integer&);
void dec_to_binary(Integer, vector< Integer> &, Integer, bool);
Integer binary_size(Integer);
void hex_to_binary(string, vector< Integer>&, int);
string binary_to_hex(vector< Integer>&, int);
Integer binary_to_dec(vector< Integer>&, int);
Integer generate_prime(int, Integer&);
Integer safe_prime(int, Integer);
Integer field_generator(Integer, Integer);
void key_generation(Keys&, int);
void encrypt(Integer&, vector< Integer>&, Keys, vector< Integer>&, int);
void decrypt(Integer, vector< Integer>&, Keys, vector< Integer>&, int);
void map_message(string, vector< Integer>&, int, int, int);
string unmap_message(vector< Integer>&, int, int, int);

/*
* @param n the number whose primality will be tested -- must be odd, >= 3 
* @param t the number of times m will be tested
* @return 1 if number is prime, 0 if composite
*/
bool fermat_primality(Integer n, int t) {
    if (n < 2 || n % 2 == 0) {
        return 0;
    }
    if (n == 3) {
        return 1;
    }

    for (int i = 0; i < t; i++) {
        // returns a in range 2 to (n - 2)
        Integer ub = n - 2;
        Integer lb = 2;
        Integer a;// = (rand() % (ub - lb + 1)) + lb;
        AutoSeededRandomPool prng;
        a.Randomize(prng, lb, ub);

        Integer r = square_and_mult(a, n-1, n);

        if (r != 1) {
            return 0;
        }
    }

    return 1;
}

/*
* @param n the number whose primality will be tested -- must be odd, >= 3 
* @param t the number of times m will be tested
* @return 1 if number is prime, 0 if composite
*/
bool miller_rabin_primality(Integer n, int t) {
    if (n < 2 || n % 2 == 0) {
        return 0;
    }
    if (n == 3) {
        return 1;
    }

    Integer s = 0;
    Integer r = prime_facto_MR(n - 1, s);
    
    for (int i = 0; i < t; i++) {
        // returns a in range 2 to (n - 2)
        Integer ub = n - 2;
        int lb = 2;
        Integer a;// = (rand() % (ub - lb + 1)) + lb;
        AutoSeededRandomPool prng;
        a.Randomize(prng, lb, ub);

        Integer y = square_and_mult(a, r, n);

        if (y != 1 && y != (n-1) ) {
            int j = 1;
            while (j <= (s-1) && y != (n-1) ) {
                /*y = y ^ 2;
                y %= n;*/
                y = a_exp_b_mod_c(y, 2, n);

                if (y == 1) {
                    return 0;
                }
                j++;
            }

            if (y != (n-1) ) {
                return 0;
            }
        }
    }

    return 1;
}

/*
* @param num the number that you want to factorize
* @param s will be "returned" as the power of 2 in the factorization
* @return r in 2^s * r
*/
Integer prime_facto_MR(Integer num, Integer& s) {
    s = 0;

    if (num % 2 == 0) {
        num /= 2;
        s++;
    }

    return num;
}

/*
* @param a in a^k (mod n)
* @param k in a^k (mod n)
* @param n in a^k (mod n)
* @return the result of a^k (mod n)
*/
Integer square_and_mult(Integer a, Integer k, Integer n) {
    Integer len = binary_size(k);
    //int* k_bin = new int[len];
    vector<Integer> k_bin(len.ConvertToLong());
    dec_to_binary(k, k_bin, len, true);

    Integer b = 1;

    for (int i = 0; i < len; i++) {
        if (k_bin[i] != 0) {
            break;
        }
        else if (i == (len - 1) ) {
            return b;
        }
    }

    Integer A = a;
    if (k_bin[0] == 1) {
        b = a;
    }

    for (int i = 1; i < len; i++) {
        /*A = A ^ 2;
        A %= n;*/
        A = a_exp_b_mod_c(A, 2, n);

        if (k_bin[i] == 1) {
            b = A * b;
            //b %= n;
            b %= n;
        }
    }

    //delete[] k_bin;
    return b;
}

/* 
* @param dec the decimal you want to convert
* @param binary[] a holder of sufficient length for binary conversion
* @param len the length of binary[]
* @param rev if you want the binary to come out reversed or not
*/
void dec_to_binary(Integer dec, /*int binary[]*/vector<Integer> &binary, Integer len, bool rev) {
    Integer r;
    Integer q;

    //int* binary_rev = new int[len];
    vector< Integer> binary_rev(len.ConvertToLong());

    for (int i = 0; i < len; i++) {
        r = dec % 2;
        q = dec / 2;    // int division

        binary_rev[i] = r;
        dec = q;
    }

    if (rev == false) {
        Integer end = len - 1;
        for (int i = 0; i < len; i++) {
            binary[i] = binary_rev[end.ConvertToLong()];
            end--;
        }
    }
    else {
        for (int i = 0; i < len; i++) {
            binary[i] = binary_rev[i];
        }
    }

    //delete[] binary_rev;
}

/* 
* @param dec_num the number whose binary size you will receive
* @return the number of bits that dec_num will fit into if converted to binary
*/
Integer binary_size(Integer dec_num) {
    size_t bin_size = 1;

    while (true) {
        Integer compare = compare.Power2(bin_size);
        if (dec_num >= compare) {
            bin_size++;
        }
        else {
            Integer bin_size_int = bin_size;
            return bin_size_int;
        }
    }
}

/*
* @param hex the hexadecimal to be converted
* @param binary[] "returns" the converted binary
* @param len the length the binary will be
*/
void hex_to_binary(string hex, vector< Integer>& binary, int len) {
    string num;
    int* binary_rev = new int[len];
    int num_split;
    int r;
    int q;

    for (int i = 0; i < (len / 4); i++) {
        num = hex[i];

        // convert letters to ints via ascii
        if (num == "A" || num == "B" || num == "C" || num == "D" || num == "E" || num == "F") {
            num_split = int(hex[i] - 65 + 10);
        }
        else {
            num_split = hex[i] - '0';
        }

        for (int j = 0; j < 4; j++) {
            r = num_split % 2;
            q = num_split / 2;

            binary_rev[(4 * i) + j] = r;
            num_split = q;
        }

        int end = (4 * i) + 3;
        for (int k = (4 * i); k < (4 * i) + 4; k++) {
            binary[k] = binary_rev[end];
            end--;
        }

    }
}

/*
* @param binary[] the binary to be converted
* @param len the length of the binary to be converted
* @return converted hexadecimal
*/
string binary_to_hex(/*int binary[]*/vector< Integer> &binary, int bin_len) {
    string hex = "";
    //int bin_group[16][4] = { 0 };
    int len = bin_len / 4;
    /*int** bin_group = new int* [len];
    for (int i = 0; i < len; i++) {
        bin_group[i] = new int[4];
    }*/
    vector<vector< Integer>> bin_group(len, vector< Integer>(4));

    for (int i = 0; i < len; i++) {
        for (int j = 0; j < 4; j++) {
            bin_group[i][j] = binary[(i * 4) + j];
        }
    }

    for (int i = 0; i < len; i++) {
        Integer num = binary_to_dec(bin_group[i], 4);

        if (num > 9) {
            num = num - 10 + 65;
            char num_hex = char(num.ConvertToLong());
            hex += num_hex;
        }
        else {
            hex += to_string(num.ConvertToLong());
        }
    }

    return hex;
}

/*
* @param binary[] the binary to be converted
* @param bin_len the length of the binary to be converted
* @return the converted decimal number
*/
Integer binary_to_dec(/*int binary[]*/vector< Integer> &binary, int bin_len) {
    Integer dec = 0;

    for (int i = 0; i < bin_len; i++) {
        Integer power;
        //dec += binary[i] * int(pow(2, (bin_len - 1 - i)));
        dec += binary[i] * power.Power2(bin_len - 1 - i);
    }

    return dec;
}

/*
* @param bit_size the size of the prime to be generated in bits
* @param generator will be "returned" as the generator of the finite field
* @return the generated prime
*/
Integer generate_prime(int bit_size, Integer& generator) {
    AutoSeededRandomPool prng;
    //cout << "finished rng" << endl;
    while (true) {
        bool is_prime = false;
        // can generate a prime of bit size n between by generating
        // between 2^(n-1) + 1 and 2^n - 1
        //Integer ub = pow(2, bit_size) - 1;
        Integer ub = ub.Power2(bit_size) - 1;
        //Integer lb = pow(2, bit_size - 1) + 1;
        Integer lb = lb.Power2( bit_size - 1) + 1;
        //Integer p = (rand() % (ub - lb + 1)) + lb;
        Integer p;
        p.Randomize(prng, lb, ub, Integer::PRIME);

        /*if (bit_size < 8) {
            is_prime = fermat_primality(p, 15);
        }
        else if (bit_size >= 8) {
            is_prime = miller_rabin_primality(p, 10);
        }*/
        is_prime = IsPrime(p);

        if (is_prime == false) {
            continue;
        }

        //cout << "**** IS PRIME ****" << endl;

        Integer q = safe_prime(bit_size, p);

        if (q != 0) {
            //cout << "** IS SAFE **" << endl;
            generator = field_generator(p, q);
            return p;
        }
    }
}

/*
* @param bit_size the size of the prime to be tested
* @param p the prime whose safety will be tested
* @return 0 if prime is not safe, q of q = (p - 1) / 2 if prime is safe
*/
Integer safe_prime(int bit_size, Integer p) {
    bool is_prime = false;
    Integer q = (p - 1) / 2;

    /*if (bit_size < 8) {
        is_prime = fermat_primality(q, 15);
    }
    else if (bit_size >= 8) {
        is_prime = miller_rabin_primality(q, 10);
    }*/

    is_prime = IsPrime(q);

    if (is_prime == false) {
        return 0;
    }
    else {
        return q;
    }
}

/*
* @param p the prime to generate a field for
* @param q of q = (p - 1) / 2
* @return generator of a finite field for p
*/
Integer field_generator(Integer p, Integer q) {
    AutoSeededRandomPool prng;
    Integer g;
    Integer ub = p - 2;
    Integer lb = 2;

    // if p is a safe prime, then for a random int g, 2 <= g <= p-2, either
    // g^2 == 1 mod p,
    // g^q == 1 mod p,
    // or g is a generator for p
    while (true) {
        //g = (rand() % (ub - lb + 1)) + lb;
        g.Randomize(prng, lb, ub);

        if (square_and_mult(g, 2, p) == 1) {
            continue;
        }
        else if (square_and_mult(g, q, p) == 1) {
            continue;
        }
        else {
            return g;
        }
    }
}

/*
* @param keys an object holding the public and private keys
* @param alpha the generator for the prime
* @param p the prime
*/
void key_generation(Keys& keys, int prime_len) {
    AutoSeededRandomPool prng;
    cout << "Generating " << prime_len << " bit prime, testing safety..." << endl << endl;
    Integer alpha;
    Integer p = generate_prime(prime_len, alpha);

    Integer ub = p - 2;
    Integer lb = 1;

    Integer a;// = (rand() % (ub - lb + 1)) + lb;

    a.Randomize(prng, lb, ub);
    cout << "a is " << a << endl;

    Integer x = square_and_mult(alpha, a, p);

    keys.set_pub_key(p, alpha, x);
    keys.set_priv_key(a);
    cout << "Public key (prime, alpha, x) is " << keys.get_prime() << ", " << keys.get_alpha() << ", " << keys.get_x() << endl;
    cout << "Private key (a) is " << keys.get_a() << endl << endl;
}

/*
* @param gamma "returns" alpha^k (mod p)
* @param delta[] "returns" m * x^k (mod p)
* @param keys object holding public and private keys
* @param msg[] the mapped message
* @param num_blocks the number of blocks needed for the message
*/
void encrypt(Integer& gamma, /*int delta[]*/vector< Integer> &delta, Keys keys, vector< Integer>& msg, int num_blocks) {
    Integer ub = keys.get_prime() - 2;
    Integer lb = 1;
    Integer k;// = (rand() % (ub - lb + 1)) + lb;
    AutoSeededRandomPool prng;
    k.Randomize(prng, lb, ub);
    cout << "random num k is " << k << endl;

    gamma = square_and_mult(keys.get_alpha(), k, keys.get_prime());
    Integer d = square_and_mult(keys.get_x(), k, keys.get_prime());
    for (int i = 0; i < num_blocks; i++) {
        delta[i] = (d * msg[i]) % keys.get_prime();
    }
}

/*
* @param gamma holds alpha^k (mod p)
* @param delta[] holds m * x^k (mod p)
* @param keys object holding public and private keys
* @param decode_msg[] "returns" the decoded mapped message
* @param num_blocks how many blocks the message was mapped to
*/
void decrypt(Integer gamma, /*int delta[]*/vector< Integer> &delta, Keys keys, /*int decode_msg[]*/vector< Integer> &decode_msg, int num_blocks) {
    gamma = square_and_mult(gamma, keys.get_prime() - 1 - keys.get_a(), keys.get_prime());
    for (int i = 0; i < num_blocks; i++) {
        decode_msg[i] = gamma * delta[i] % keys.get_prime();
    }
}

/*
* @param msg the message in hexadecimal
* @param map_msg[] "returns" the message mapped to the finite field
* @param block_size size of the message blocks
* @param num_blocks number of message blocks
* @param bin_msg_len the length of the original message in binary
*/
void map_message(string msg, vector< Integer>& map_msg, int block_size, int num_blocks, int bin_msg_len) {
    // bin_msg will contain msg without blocks
    //int* bin_msg = new int[bin_msg_len];
    vector< Integer> bin_msg(bin_msg_len);
    // bin_map_msg will contain msg with blocks
    /*int** bin_map_msg = new int* [num_blocks];
    for (int i = 0; i < num_blocks; i++) {
        bin_map_msg[i] = new int[block_size];
    }*/
    vector< vector< Integer>> bin_map_msg(num_blocks, vector< Integer>(block_size));

    // fill bin_msg
    hex_to_binary(msg, bin_msg, bin_msg_len);

    // convert bin_msg to bin_map_msg
    int bin_remaining = bin_msg_len;
    bool first = true;
    for (int i = 0; i < num_blocks; i++) {
        // if only one block total
        if (bin_remaining <= block_size && first) {
            for (int j = 0; j < bin_msg_len; j++) {
                bin_map_msg[i][j] = bin_msg[j];
            }
            // fill with trailing 0's
            for (int j = bin_remaining; j < block_size; j++) {
                bin_map_msg[i][j] = 0;
            }
            first = false;
        }
        // if only one block left
        else if (bin_remaining <= block_size) {
            for (int j = 0; j < bin_remaining; j++) {
                bin_map_msg[i][j] = bin_msg[(i * block_size) + j];
            }
            // fill with trailing 0's
            for (int j = bin_remaining; j < block_size; j++) {
                bin_map_msg[i][j] = 0;
            }
        }
        // if multiple blocks left
        else {
            for (int j = 0; j < block_size; j++) {
                bin_map_msg[i][j] = bin_msg[(i * block_size) + j];
            }
            bin_remaining -= block_size;
            first = false;
        }
    }

    // map binary msg to the finite field
    for (int i = 0; i < num_blocks; i++) {
        map_msg[i] = binary_to_dec(bin_map_msg[i], block_size);
    }

    // return to freestore
    /*delete[] bin_msg;
    for (int i = 0; i < num_blocks; i++) {
        delete[] bin_map_msg[i];
    }
    delete[] bin_map_msg;*/
}

/*
* @param decoded_mapped_msg[] the decoded message in decimal
* @param block_size the size of the message blocks
* @param num_blocks the number of message blocks
* @param bin_msg_len the length of the message in binary
* @return the decoded message in hexadecimal
*/
string unmap_message(/*int decoded_mapped_msg[]*/vector< Integer> &decoded_mapped_msg, int block_size, int num_blocks, int bin_msg_len) {
    // will hold the decoded binary message in blocks
    /*int** decode_bin = new int* [num_blocks];
    for (int i = 0; i < num_blocks; i++) {
        decode_bin[i] = new int[block_size];
    }*/
    vector<vector< Integer>> decode_bin(num_blocks, vector< Integer>(block_size));
    // will hold the decoded binary message without blocks
    //int* decode_bin_flat = new int[num_blocks * block_size];
    vector< Integer> decode_bin_flat(num_blocks * block_size);

    // fill decode_bin
    for (int i = 0; i < num_blocks; i++) {
        dec_to_binary(decoded_mapped_msg[i], decode_bin[i], block_size, false);
    }

    // fill decode_bin_flat
    for (int i = 0; i < num_blocks; i++) {
        for (int j = 0; j < block_size; j++) {
            decode_bin_flat[i * block_size + j] = decode_bin[i][j];
        }
    }

    string decoded = binary_to_hex(decode_bin_flat, bin_msg_len);

    // return to freestore
    /*for (int i = 0; i < num_blocks; i++) {
            delete[] decode_bin[i];
        }
    delete[] decode_bin;*/
    //delete[] decode_bin_flat;

    return decoded;
}

int main() {
    //Integer test;
    ////RandomNumberGenerator rng;
    //AutoSeededRandomPool prng;
    //test.Randomize(prng, 4096);
    //cout << "test is " << test << endl;

    Keys keys = Keys();

    // get hex message
    string msg;
    cout << "Please enter your message in hexadecimal" << endl;
    cin >> msg;
    cout << endl;
    
    // make hex uppercase
    for (int i = 0; i < msg.length(); i++) {
        msg[i] = toupper(msg[i]);
    }

    // get key length
    int prime_len;
    cout << "Please enter the desired length of the prime" << endl;
    //cout << "Valid lengths range from 2 to 16" << endl;
    cin >> prime_len;
    cout << endl;

    // generate keys
    key_generation(keys, prime_len);

    // make block size 1 less than key size
    //int block_size = binary_size(keys.get_prime()) - 1;
    int block_size = prime_len - 1;
    cout << "Block size is " << block_size << endl;

    // calculate number of blocks needed
    int bin_msg_len = 4 * msg.length();
    int num_blocks = ceil((double)bin_msg_len / block_size);
    cout << "Number of blocks is " << num_blocks << endl << endl;

    // will hold mapped message
    //int* map_msg = new int[num_blocks];
    vector< Integer> map_msg(num_blocks);

    // map message
    map_message(msg, map_msg, block_size, num_blocks, bin_msg_len);
    cout << "Mapped message is..." << endl;
    for (int i = 0; i < num_blocks; i++) {
        cout << "..." << map_msg[i] << endl;
    }
    cout << endl;;

    // encrypt message
    Integer gamma;
    //int* delta = new int[num_blocks];
    vector< Integer> delta(num_blocks);
    encrypt(gamma, delta, keys, map_msg, num_blocks);

    cout << "Ciphertext (gamma, delta) is..." << endl;
    for (int i = 0; i < num_blocks; i++) {
        cout << "...(" << gamma << ", " << delta[i] << ")" << endl;;
    }
    cout << endl;

    // decrypt message
    //int* decode_map_msg = new int[num_blocks];
    vector< Integer> decode_map_msg(num_blocks);
    decrypt(gamma, delta, keys, decode_map_msg, num_blocks);
    
    // unmap message
    string decoded_string = unmap_message(decode_map_msg, block_size, num_blocks, bin_msg_len);
    cout << "Decoded message is " << decoded_string;

    //delete[] map_msg;
    //delete[] delta;
    //delete[] decode_map_msg;

	return 0;
}
