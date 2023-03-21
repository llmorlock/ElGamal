// CIS 4634 Assignment 03 (ElGamal).cpp

#include <iostream>
#include <string>
#include <math.h>
//#include <boost/multiprecision/cpp_int.hpp>
//#include <boost/random.hpp>
//using namespace boost::multiprecision;
//using namespace boost::random;
using namespace std;

//typedef independent_bits_engine<mt19937, 32, uint32_t> generator32_type;
//generator32_type gen32;
//
//typedef independent_bits_engine<mt19937, 64, uint64_t> generator64_type;
//generator64_type gen64;
//
//typedef independent_bits_engine<mt19937, 128, uint128_t> generator128_type;
//generator128_type gen128;
//
//typedef independent_bits_engine<mt19937, 256, uint256_t> generator256_type;
//generator256_type gen256;

class Keys {
public:
    Keys() = default;
    ~Keys() = default;

    /*
    * @param p the prime part of the key
    * @param alpha the generator for the prime
    * @param x = alpha^a (mod p)
    */
    void set_pub_key(int p, int alpha, int x) {
        public_key[0] = p;
        public_key[1] = alpha;
        public_key[2] = x;
    }

    /*
    * @param a single part of the private key
    */
    void set_priv_key(int a) {
        private_key = a;
    }

    int get_prime() {
        public_key[0] = 11;         //FIXME
        return public_key[0];
    }

    int get_alpha() {
        public_key[1] = 2;         //FIXME
        return public_key[1];
    }

    int get_x() {
        public_key[2] = 9;   //FIXME
        return public_key[2];
    }

    int get_a() {
        private_key = 6;        //FIXME
        return private_key;
    }

private:
    int public_key[3];
    int private_key;
};

bool fermat_primality(int, int);
bool miller_rabin_primality(int, int);
int square_and_mult(int, int, int);
int prime_facto_MR(int, int&);
void dec_to_binary(int, int[], int, bool);
int binary_size(int);
void hex_to_binary(string, int[], int);
string binary_to_hex(int[], int);
int binary_to_dec(int[], int);
//template <typename T> T generate_prime(int);
int generate_prime(int, int&);
int safe_prime(int, int);
int field_generator(int, int);
void key_generation(Keys, int, int);

/*
* @param n the number whose primality will be tested -- must be odd, >= 3 
* @param t the number of times m will be tested
* @return 1 if number is prime, 0 if composite
*/
bool fermat_primality(int n, int t) {
    cout << "DEBUG " << "entered fermat" << endl;
    if (n < 2 || n % 2 == 0) {
        return 0;
    }
    if (n == 3) {
        return 1;
    }

    for (int i = 0; i < t; i++) {
        // returns a in range 2 to (n - 2)
        int ub = n - 2;
        int lb = 2;
        int a = (rand() % (ub - lb + 1)) + lb;

        int r = square_and_mult(a, n-1, n);

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
bool miller_rabin_primality(int n, int t) {
    cout << "DEBUG " << "entered miller-rabin" << endl;
    if (n < 2 || n % 2 == 0) {
        return 0;
    }
    if (n == 3) {
        return 1;
    }

    int s = 0;
    int r = prime_facto_MR(n - 1, s);
    
    for (int i = 0; i < t; i++) {
        // returns a in range 2 to (n - 2)
        int ub = n - 2;
        int lb = 2;
        int a = (rand() % (ub - lb + 1)) + lb;

        int y = square_and_mult(a, r, n);

        if (y != 1 && y != (n-1) ) {
            int j = 1;
            while (j <= (s-1) && y != (n-1) ) {
                y = pow(y, 2);
                y %= n;

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
int prime_facto_MR(int num, int& s) {
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
int square_and_mult(int a, int k, int n) {
    int len = binary_size(k);
    int* k_bin = new int[len];
    dec_to_binary(k, k_bin, len, true);

    //cout << "DEBUG " << "len is " << len << endl;
    //cout << "DEBUG " << "k_bin is ";
    //for (int i = 0; i < len; i++) {
    //    cout << k_bin[i];
    //}
    //cout << endl;

    int b = 1;

    for (int i = 0; i < len; i++) {
        if (k_bin[i] != 0) {
            break;
        }
        else if (i == (len - 1) ) {
            return b;
        }
    }

    int A = a;
    if (k_bin[0] == 1) {
        b = a;
    }

    for (int i = 1; i < len; i++) {
        A = pow(A, 2);
        A %= n;

        if (k_bin[i] == 1) {
            b = A * b;
            b %= n;
        }
    }

    delete[] k_bin;
    return b;
}

/* 
* @param dec the decimal you want to convert
* @param binary[] a holder of sufficient length for binary conversion
* @param len the length of binary[]
* @param rev if you want the binary to come out reversed or not
*/
void dec_to_binary(int dec, int binary[], int len, bool rev) {
    int r;
    int q;

    int* binary_rev = new int[len];

    for (int i = 0; i < len; i++) {
        r = dec % 2;
        q = dec / 2;    // int division

        binary_rev[i] = r;
        dec = q;
    }

    if (rev == false) {
        int end = len - 1;
        for (int i = 0; i < len; i++) {
            binary[i] = binary_rev[end];
            end--;
        }
    }
    else {
        for (int i = 0; i < len; i++) {
            binary[i] = binary_rev[i];
        }
    }

    delete[] binary_rev;
}

/* 
* @param dec_num the number whose binary size you will receive
* @return the number of bits that dec_num will fit into if converted to binary
*/
int binary_size(int dec_num) {
    int bin_size = 1;

    while (true) {
        if (dec_num >= pow(2, bin_size)) {
            bin_size++;
        }
        else {
            return bin_size;
        }
    }
}

/*
* @param hex the hexadecimal to be converted
* @param binary[] "returns" the converted binary
* @param len the length the binary will be
*/
void hex_to_binary(string hex, int binary[], int len) {
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
string binary_to_hex(int binary[], int bin_len) {
    string hex = "";
    //int bin_group[16][4] = { 0 };
    int len = bin_len / 4;
    int** bin_group = new int* [len];
    for (int i = 0; i < len; i++) {
        bin_group[i] = new int[4];
    }

    for (int i = 0; i < len; i++) {
        for (int j = 0; j < 4; j++) {
            bin_group[i][j] = binary[(i * 4) + j];
        }
    }

    for (int i = 0; i < len; i++) {
        int num = binary_to_dec(bin_group[i], 4);

        if (num > 9) {
            num = num - 10 + 65;
            char num_hex = char(num);
            hex += num_hex;
        }
        else {
            hex += to_string(num);
        }
    }

    return hex;
}

/*
* @param binary[] the binary to be converted
* @param bin_len the length of the binary to be converted
* @return the converted decimal number
*/
int binary_to_dec(int binary[], int bin_len) {
    int dec = 0;

    for (int i = 0; i < bin_len; i++) {
        dec += binary[i] * int(pow(2, (bin_len - 1 - i)));
    }

    return dec;
}

//template <typename T>
//T generate_prime(int bit_size) {
//    T p;
//
//    
//}

/*
* @param bit_size the size of the prime to be generated in bits
* @param generator will be "returned" as the generator of the finite field
* @return the generated prime
*/
int generate_prime(int bit_size, int& generator) {
    cout << "DEBUG " << "entered gen_prime" << endl;
    while (true) {
        bool is_prime = false;
        // can generate a prime of bit size n between by generating
        // between 2^(n-1) + 1 and 2^n - 1
        int ub = pow(2, bit_size) - 1;
        int lb = pow(2, bit_size - 1) + 1;
        int p = (rand() % (ub - lb + 1)) + lb;

        cout << "DEBUG " << "prime is " << p << ", moving to testing" << endl;

        if (bit_size == 4) {
            is_prime = fermat_primality(p, 15);
        }
        else if (bit_size == 8) {
            is_prime = miller_rabin_primality(p, 10);
        }

        if (is_prime == false) {
            continue;
        }

        int q = safe_prime(bit_size, p);
        cout << "DEBUG " << "safe prime is " << q << endl;

        if (q != 0) {
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
int safe_prime(int bit_size, int p) {
    cout << "DEBUG " << "entered safe_prime" << endl;
    bool is_prime = false;
    int q = (p - 1) / 2;

    if (bit_size == 4) {
        is_prime = fermat_primality(q, 15);
    }
    else if (bit_size == 8) {
        is_prime = miller_rabin_primality(q, 10);
    }

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
int field_generator(int p, int q) {
    cout << "DEBUG " << "entered field_gen" << endl;
    int g;
    int ub = p - 2;
    int lb = 2;

    // if p is a safe prime, then for a random int g, 2 <= g <= p-2, either
    // g^2 == 1 mod p,
    // g^q == 1 mod p,
    // or g is a generator for p
    while (true) {
        g = (rand() % (ub - lb + 1)) + lb;

        cout << "DEBUG " << "g is " << g << endl;
        cout << "DEBUG " << "g^2 is " << square_and_mult(g, 2, p) << ", g^q is " << square_and_mult(g, q, p) << endl;

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
void key_generation(Keys keys, int alpha, int p) {
    int ub = p - 2;
    int lb = 1;

    int a = (rand() % (ub - lb + 1)) + lb;
    int x = square_and_mult(alpha, a, p);

    keys.set_pub_key(p, alpha, x);
    keys.set_priv_key(a);
}

/*
* @param gamma "returns" alpha^k (mod p)
* @param delta[] "returns" m * x^k (mod p)
* @param keys object holding public and private keys
* @param msg[] the mapped message
* @param num_blocks the number of blocks needed for the message
*/
void encrypt(int& gamma, int delta[], Keys keys, int msg[], int num_blocks) {
    int ub = keys.get_prime() - 2;
    int lb = 1;
    int k = (rand() % (ub - lb + 1)) + lb;
    cout << "DEBUG " << "k is " << k << endl;

    gamma = square_and_mult(keys.get_alpha(), k, keys.get_prime());
    int d = square_and_mult(keys.get_x(), k, keys.get_prime());
    cout << "DEBUG " << "d is " << d << endl;
    for (int i = 0; i < num_blocks; i++) {
        delta[i] = (d * msg[i]) % keys.get_prime();
    }
}

void decrypt(int gamma, int delta[], Keys keys, int decode_msg[], int num_blocks) {
    gamma = square_and_mult(gamma, keys.get_prime() - 1 - keys.get_a(), keys.get_prime());
    for (int i = 0; i < num_blocks; i++) {
        decode_msg[i] = gamma * delta[i] % keys.get_prime();
    }
}

int main() {
    //cout << "dynamic test" << endl;
    //int len = binary_size(129);
    //cout << "len is " << len << endl;
    //int* binary = new int[len];

    //dec_to_binary(129, binary, len, false);

    //delete[] binary;

    //cout << "square-and-mult test" << endl;
    //for (int i = 0; i < 4; i++) {
    //    int n = 13;
    //    int ub = n - 2;
    //    int lb = 2;
    //    int a = (rand() % (ub - lb + 1)) + lb;

    //    cout << "a is " << a << endl;

    //    int r = square_and_mult(a, n - 1, n);
    //    cout << "result is " << r << endl;
    //}

    //cout << "Square-and-mult test" << endl;
    //square_and_mult(2, 2, 11);

    //cout << "prime test" << endl;
    //cout << "generating 8 bit prime, testing safety" << endl;
    //int alpha;
    //int prime = generate_prime(8, alpha);
    //cout << "safe prime is " << prime << ", generator is " << alpha << endl;

    Keys keys;
    string msg = "AB";

    //int bin_msg_len = 4 * msg.length();
    //int* bin_msg = new int[bin_msg_len];

    //hex_to_binary(msg, bin_msg, bin_msg_len);
    //cout << "binary message: ";
    //for (int i = 0; i < bin_msg_len; i++) {
    //    cout << bin_msg[i];
    //}
    //cout << endl;

    //int* map_msg = new int[bin_msg_len];

    //if (bin_msg_len <= keys.get_prime() - 1) {
    //    for (int i = 0; i < bin_msg_len; i++) {

    //    }
    //}
    //else {  // need to make blocks
    //    //FIXME
    //}

    //cout << binary_to_hex(bin_msg, bin_msg_len);

    //delete[] bin_msg;
    //delete[] map_msg;

    // make block size 1 less than key size
    int block_size = binary_size(keys.get_prime()) - 1;
    cout << "DEBUG " << "block size is " << block_size << endl;

    int bin_msg_len = 4 * msg.length();
    int num_mapped_els = ceil((double)bin_msg_len / block_size);
    cout << "DEBUG " << "bin msg len is " << bin_msg_len << endl;
    cout << "DEBUG " << "num mapped els is " << num_mapped_els << endl;

    // bin_msg contains msg without blocks
    int* bin_msg = new int[bin_msg_len];

    // bin_map_msg contains ceil(length/blocksize) number of blocksize arrays
    int** bin_map_msg = new int*[num_mapped_els];       
    for (int i = 0; i < num_mapped_els; i++) {
        bin_map_msg[i] = new int[block_size];
    }

    // fill bin_msg
    hex_to_binary(msg, bin_msg, bin_msg_len);
    cout << "DEBUG " << "binary msg is ";
    for (int i = 0; i < bin_msg_len; i++) {
        cout << bin_msg[i];
    }
    cout << endl;

    // convert bin_msg to bin_map_msg
    int bin_msg_len_track = bin_msg_len;
    bool first = true;
    for (int i = 0; i < num_mapped_els; i++) {
        // if only one block total
        if (bin_msg_len_track <= block_size && first) {
            for (int j = 0; j < bin_msg_len; j++) {
                bin_map_msg[i][j] = bin_msg[i];
            }
            // fill with trailing 0's
            for (int j = bin_msg_len_track; j < block_size; j++) {
                bin_map_msg[i][j] = 0;
            }
            first = false;
        }
        // if only one block left
        else if (bin_msg_len_track <= block_size) {
            for (int j = 0; j < bin_msg_len_track; j++) {
                bin_map_msg[i][j] = bin_msg[(i * block_size) + j];
            }
            // fill with trailing 0's
            for (int j = bin_msg_len_track; j < block_size; j++) {
                bin_map_msg[i][j] = 0;
            }
        }
        // if multiple blocks left
        else {
            for (int j = 0; j < block_size; j++) {
                bin_map_msg[i][j] = bin_msg[(i * block_size) + j];
            }
            bin_msg_len_track -= block_size;
            first = false;
        }
    }

    cout << "DEBUG " << "bin map msg is..." << endl;;
    for (int i = 0; i < num_mapped_els; i++) {
        cout << "...";
        for (int j = 0; j < block_size; j++) {
            cout << bin_map_msg[i][j];
        }
        cout << endl;
    }
    cout << endl;

    // map binary msg to the finite field
    int* dec_map_msg = new int[num_mapped_els];
    for (int i = 0; i < num_mapped_els; i++) {
        dec_map_msg[i] = binary_to_dec(bin_map_msg[i], block_size);
    }

    cout << "DEBUG " << "dec mapped msg is ";
    for (int i = 0; i < num_mapped_els; i++) {
        cout << dec_map_msg[i] << ", ";
    }
    cout << endl;

    int gamma;
    int* delta = new int[num_mapped_els];
    encrypt(gamma, delta, keys, dec_map_msg, num_mapped_els);

    cout << "DEBUG " << "delta is ";
    for (int i = 0; i < num_mapped_els; i++) {
        cout << delta[i] << ", ";
    }
    cout << endl;

    int* decode_msg = new int[num_mapped_els];
    decrypt(gamma, delta, keys, decode_msg, num_mapped_els);
    cout << "DEBUG " << "decoded is ";
    for (int i = 0; i < num_mapped_els; i++) {
        cout << decode_msg[i] << ", ";
    }
    cout << endl;

    int** decode_bin = new int*[num_mapped_els];
    //int decode_bin_len = binary_size(decode_msg[0]);
    for (int i = 0; i < num_mapped_els; i++) {
        decode_bin[i] = new int[block_size];
    }

    for (int i = 0; i < num_mapped_els; i++) {
        dec_to_binary(decode_msg[i], decode_bin[i], block_size, false);
    }

    cout << "DEBUG " << "decoded bin is..." << endl;;
    for (int i = 0; i < num_mapped_els; i++) {
        cout << "...";
        for (int j = 0; j < block_size; j++) {
            cout << decode_bin[i][j];
        }
        cout << endl;
    }
    cout << endl;

    int* decode_bin_flat = new int[num_mapped_els * block_size];
    for (int i = 0; i < num_mapped_els; i++) {
        for (int j = 0; j < block_size; j++) {
            decode_bin_flat[i * block_size + j] = decode_bin[i][j];
        }
    }

    cout << "DEBUG " << "decode bin flat is ";
    for (int i = 0; i < num_mapped_els * block_size; i++) {
        cout << decode_bin_flat[i];
    }
    cout << endl;

    int chop = (num_mapped_els * block_size) % 4;
    int keep = (num_mapped_els * block_size) - chop;

    string decode_string = binary_to_hex(decode_bin_flat, keep);
    cout << decode_string;

    delete[] bin_msg;
    for (int i = 0; i < num_mapped_els; i++) {
        delete[] bin_map_msg[i];
    }
    delete[] bin_map_msg;
    delete[] dec_map_msg;
    delete[] delta;
    delete[] decode_msg;
    for (int i = 0; i < num_mapped_els; i++) {
        delete[] decode_bin[i];
    }
    delete[] decode_bin;
    delete[] decode_bin_flat;

	return 0;
}
