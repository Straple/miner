#include "sha256.hpp"
#include <algorithm>
#include <chrono>
#include <cstring>
#include <iostream>

using namespace std::chrono;

uint_t rotr(uint_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

uint_t choose(uint_t e, uint_t f, uint_t g) {
    return (e & f) ^ (~e & g);
}

uint_t majority(uint_t a, uint_t b, uint_t c) {
    return (a & (b | c)) | (b & c);
}

uint_t sig0(uint_t x) {
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

uint_t sig1(uint_t x) {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

uint32_t rotr_int(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

uint32_t choose_int(uint32_t e, uint32_t f, uint32_t g) {
    return (e & f) ^ (~e & g);
}

uint32_t majority_int(uint32_t a, uint32_t b, uint32_t c) {
    return (a & (b | c)) | (b & c);
}

uint32_t sig0_int(uint32_t x) {
    return rotr_int(x, 7) ^ rotr_int(x, 18) ^ (x >> 3);
}

uint32_t sig1_int(uint32_t x) {
    return rotr_int(x, 17) ^ rotr_int(x, 19) ^ (x >> 10);
}

SHA256::SHA256() : m_blocklen(0), m_bitlen(0) {
    m_state[0] = 0x6a09e667;
    m_state[1] = 0xbb67ae85;
    m_state[2] = 0x3c6ef372;
    m_state[3] = 0xa54ff53a;
    m_state[4] = 0x510e527f;
    m_state[5] = 0x9b05688c;
    m_state[6] = 0x1f83d9ab;
    m_state[7] = 0x5be0cd19;
}

void SHA256::update(const std::vector<byte_t> &data) {
    for (size_t i = 0; i < data.size(); i++) {
        m_data[m_blocklen++] = data[i];
        if (m_blocklen == 64) {
            transform();

            // End of the block
            m_bitlen += 512;
            m_blocklen = 0;
        }
    }
}

std::vector<byte_t> SHA256::digest() {
    std::vector<byte_t> hash(32);
    pad();
    revert(hash);
    return hash;
}

void SHA256::transform() {


    uint_t maj, xorA, ch, xorE, sum, newA, newE, m[64];
    uint_t state[8];

    for (uint8_t i = 0, j = 0; i < 16; i++, j += 4) {// Split data in 32 bit blocks for the 16 first words
        m[i] = (uint_t(m_data[j]) << 24) | (uint_t(m_data[j + 1]) << 16) | (uint_t(m_data[j + 2]) << 8) | uint_t(m_data[j + 3]);
    }

    for (uint8_t k = 16; k < 64; k++) {// Remaining 48 blocks
        m[k] = sig1(m[k - 2]) + sig0(m[k - 15]) + m[k - 16] + m[k - 7];
    }

    for (uint8_t i = 0; i < 8; i++) {
        state[i] = m_state[i];
    }

    //auto time_start = steady_clock::now();
    for (uint8_t i = 0; i < 64; i++) {
        for (uint32_t index = 0; index < SOLVER_LEN; index++) {
            maj[index] = majority_int(state[0][index], state[1][index], state[2][index]);
        }

        for (uint32_t index = 0; index < SOLVER_LEN; index++) {
            xorA[index] = rotr_int(state[0][index], 2) ^ rotr_int(state[0][index], 13) ^ rotr_int(state[0][index], 22);
        }

        for (uint32_t index = 0; index < SOLVER_LEN; index++) {
            ch[index] = choose_int(state[4][index], state[5][index], state[6][index]);
        }

        for (uint32_t index = 0; index < SOLVER_LEN; index++) {
            xorE[index] = rotr_int(state[4][index], 6) ^ rotr_int(state[4][index], 11) ^ rotr_int(state[4][index], 25);
        }

        for (uint32_t index = 0; index < SOLVER_LEN; index++) {
            sum[index] = m[i][index] + K[i] + state[7][index] + ch[index] + xorE[index];
        }

        for (uint32_t index = 0; index < SOLVER_LEN; index++) {
            newA[index] = xorA[index] + maj[index] + sum[index];
        }
        for (uint32_t index = 0; index < SOLVER_LEN; index++) {
            newE[index] = state[3][index] + sum[index];
        }

        for (uint32_t index = 0; index < SOLVER_LEN; index++) {
            state[7][index] = state[6][index];
        }
        for (uint32_t index = 0; index < SOLVER_LEN; index++) {
            state[6][index] = state[5][index];
        }
        for (uint32_t index = 0; index < SOLVER_LEN; index++) {
            state[5][index] = state[4][index];
        }
        for (uint32_t index = 0; index < SOLVER_LEN; index++) {
            state[4][index] = newE[index];
        }
        for (uint32_t index = 0; index < SOLVER_LEN; index++) {
            state[3][index] = state[2][index];
        }
        for (uint32_t index = 0; index < SOLVER_LEN; index++) {
            state[2][index] = state[1][index];
        }
        for (uint32_t index = 0; index < SOLVER_LEN; index++) {
            state[1][index] = state[0][index];
        }
        for (uint32_t index = 0; index < SOLVER_LEN; index++) {
            state[0][index] = newA[index];
        }
    }

    //auto time_stop = steady_clock::now();
    //auto duration = time_stop - time_start;
    //double time = duration_cast<nanoseconds>(duration).count() / 1e9;
    //std::cout << "transform time: " << time << "s\n"; // very fast

    for (uint8_t i = 0; i < 8; i++) {
        m_state[i] = m_state[i] + state[i];
    }
}

void SHA256::pad() {
    uint64_t i = m_blocklen;
    uint8_t end = m_blocklen < 56 ? 56 : 64;

    m_data[i++] = 0x80;// Append a bit 1
    while (i < end) {
        m_data[i++] = 0x00;// Pad with zeros
    }

    if (m_blocklen >= 56) {
        transform();
        memset(m_data, 0, 56);
    }

    // Append to the padding the total message's length in bits and transform.
    m_bitlen += m_blocklen * 8;
    m_data[63] = m_bitlen;
    m_data[62] = m_bitlen >> 8;
    m_data[61] = m_bitlen >> 16;
    m_data[60] = m_bitlen >> 24;
    m_data[59] = m_bitlen >> 32;
    m_data[58] = m_bitlen >> 40;
    m_data[57] = m_bitlen >> 48;
    m_data[56] = m_bitlen >> 56;
    transform();
}

void SHA256::revert(std::vector<byte_t> &hash) {
    //auto time_start = steady_clock::now();
    // SHA uses big endian byte ordering
    // Revert all bytes
    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            hash[i + (j * 4)] = byte_t(m_state[j] >> (24 - i * 8));
        }
    }
    //auto time_stop = steady_clock::now();
    //auto duration = time_stop - time_start;
    //double time = duration_cast<nanoseconds>(duration).count() / 1e9;
    //std::cout << "revert time: " << time << "s\n"; // very fast
}