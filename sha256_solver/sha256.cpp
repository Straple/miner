#include "sha256.hpp"
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <sstream>

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

template<uint32_t bits>
solver<bits> choose(solver<bits> e, solver<bits> f, solver<bits> g) {
    return (e & f) ^ (~e & g);
}

template<uint32_t bits>
solver<bits> rotr(solver<bits> x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

template<uint32_t bits>
solver<bits> majority(solver<bits> a, solver<bits> b, solver<bits> c) {
    return (a & (b | c)) | (b & c);
}

template<uint32_t bits>
solver<bits> sig0(solver<bits> x) {
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

template<uint32_t bits>
solver<bits> sig1(solver<bits> x) {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

void SHA256::transform() {
    uint_t maj, xorA, ch, xorE, sum, newA, newE, m[64];
    uint_t state[8];

    for (uint8_t i = 0, j = 0; i < 16; i++, j += 4) {// Split data in 32 bit blocks for the 16 first words
        m[i] = (uint_t(m_data[j]) << 24) | (uint_t(m_data[j + 1]) << 16) | (uint_t(m_data[j + 2]) << 8) | uint_t(m_data[j + 3]);
    }

    for (uint8_t k = 16; k < 64; k++) { // Remaining 48 blocks
        m[k] = sig1(m[k - 2]) + m[k - 7] + sig0(m[k - 15]) + m[k - 16];
    }

    for (uint8_t i = 0; i < 8; i++) {
        state[i] = m_state[i];
    }

    for (uint8_t i = 0; i < 64; i++) {
        maj = majority(state[0], state[1], state[2]);
        xorA = rotr(state[0], 2) ^ rotr(state[0], 13) ^ rotr(state[0], 22);

        ch = choose(state[4], state[5], state[6]);

        xorE = rotr(state[4], 6) ^ rotr(state[4], 11) ^ rotr(state[4], 25);

        sum = m[i] + K[i] + state[7] + ch + xorE;
        newA = xorA + maj + sum;
        newE = state[3] + sum;

        state[7] = state[6];
        state[6] = state[5];
        state[5] = state[4];
        state[4] = newE;
        state[3] = state[2];
        state[2] = state[1];
        state[1] = state[0];
        state[0] = newA;
    }

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
    // SHA uses big endian byte ordering
    // Revert all bytes
    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            hash[i + (j * 4)] = byte_t(m_state[j] >> (24 - i * 8));
        }
    }
}
