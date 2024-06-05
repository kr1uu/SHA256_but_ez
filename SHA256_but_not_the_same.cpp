#include <iostream>
#include <sstream>
#include <bitset>
#include <vector>
#include <iomanip>
#include <cstring>

using namespace std;

vector<unsigned long> convert_to_binary(const string s) {
    vector<unsigned long> binary;
    for (char c : s) {
        unsigned long byte = c;
        for (int i = 7; i >= 0; i--) {
            binary.push_back((byte >> i) & 1);
        }
    }
    return binary;
}

vector<unsigned long> pad_to_512bits(const vector<unsigned long> binary) {
    int message_length = binary.size();
    int padding_length = 448 - (message_length % 512);
    if (padding_length < 0) {
        padding_length += 512;
    }
    vector<unsigned long> padded_binary = binary;
    padded_binary.push_back(1);
    for (int i = 0; i < padding_length - 1; i++) {
        padded_binary.push_back(0);
    }
    unsigned long message_length_bits = message_length * 8;
    for (int i = 56; i >= 0; i -= 8) {
        padded_binary.push_back((message_length_bits >> i) & 0xFF);
    }
    return padded_binary;
}

vector<unsigned long> resize_block(vector<unsigned long> padded_binary) {
    vector<unsigned long> block;
    for (int i = 0; i < padded_binary.size(); i += 32) {
        unsigned long word = 0;
        for (int j = 0; j < 32; j++) {
            word |= padded_binary[i + j] << (31 - j);
        }
        block.push_back(word);
    }
    return block;
}

string compute_hash(const vector<unsigned long> block) {
    unsigned long h[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    for (int i = 0; i < block.size(); i += 16) {
        unsigned long w[64];
        for (int j = 0; j < 16; j++) {
            w[j] = block[i + j];
        }
        for (int j = 16; j < 64; j++) {
            unsigned long s0 = (w[j - 15] >> 7 | w[j - 15] << 25) ^
                (w[j - 15] >> 18 | w[j - 15] << 14) ^ (w[j - 15] >> 3);
            unsigned long s1 = (w[j - 2] >> 17 | w[j - 2] << 15) ^
                (w[j - 2] >> 19 | w[j - 2] << 13) ^ (w[j - 2] >> 10);
            w[j] = (w[j - 16] + s0 + w[j - 7] + s1) & 0xFFFFFFFF;
        }
        unsigned long a = h[0], b = h[1], c = h[2], d = h[3], e = h[4], f = h[5], g = h[6], hh = h[7];
        for (int j = 0; j < 64; j++) {
            unsigned long S1 = (e >> 6 | e << 26) ^ (e >> 11 | e << 21) ^ (e >> 25 | e << 7);
            unsigned long ch = (e & f) ^ ((~e) & 1 & g);
            unsigned long temp1 = (hh + S1 + ch + 0x428a2f98d728ae22 + w[j]) & 0xFFFFFFFF;
            unsigned long S0 = (a >> 2 | a << 30) ^ (a >> 13 | a << 19) ^ (a >> 22);
            unsigned long maj = (a & b) ^ (a & c) ^ (b & c);
            unsigned long temp2 = (S0 + maj) & 0xFFFFFFFF;
            hh = h[7];
            h[7] = h[6];
            h[6] = h[5];
            h[5] = h[4];
            h[4] = h[3];
            h[3] = h[2];
            h[2] = h[1];
            h[1] = h[0];
            h[0] = (temp1 + temp2) & 0xFFFFFFFF;
        }
        // Update the h array
        h[0] += a;
        h[1] += b;
        h[2] += c;
        h[3] += d;
        h[4] += e;
        h[5] += f;
        h[6] += g;
        h[7] += hh;
    }
    stringstream ss;
    for (int i = 0; i < 8; i++) {
        ss << hex << setw(8) << setfill('0') << h[i];
    }
    return ss.str();
}

int main() {
    cout << "Enter a message: ";// nhap thong tin de chuyen doi sang hash 
    string message;
    getline(cin, message);

    vector<unsigned long> binary = convert_to_binary(message);
    vector<unsigned long> padded_binary = pad_to_512bits(binary);
    vector<unsigned long> block = resize_block(padded_binary);
    string hash = compute_hash(block);
    cout << "SHA-256 hash of \"" << message << "\": " << hash << endl;
    return 0;
}
