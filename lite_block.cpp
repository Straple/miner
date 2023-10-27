#include "lite_block.hpp"

lite_block::lite_block(const fast_string &bytes_data) {
    ASSERT(bytes_data.size() == 80, "bad bytes data in init lite_block");
    SHA256_Init(&sha256_ctx);
    SHA256_Update(&sha256_ctx, bytes_data.c_str(), 76);
}

fast_string lite_block::calc_hash(uint32_t x) const {
    fast_string hash(SHA256_DIGEST_LENGTH);

    SHA256_CTX cur_sha256 = sha256_ctx;
    SHA256_Update(&cur_sha256, reinterpret_cast<void *>(&x), 4);
    SHA256_Final(reinterpret_cast<unsigned char *>(hash.c_str()), &cur_sha256);

    SHA256_CTX new_sha256;
    SHA256_Init(&new_sha256);
    SHA256_Update(&new_sha256, hash.c_str(), SHA256_DIGEST_LENGTH);
    SHA256_Final(reinterpret_cast<unsigned char *>(hash.c_str()), &new_sha256);

    return hash;
}