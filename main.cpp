#include <algorithm>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include "block.hpp"

using namespace std::chrono;

#include "bits_manipulation.hpp"

std::string pretty_hashrate(uint64_t hashrate) {
    auto unit = [&]() {
        if (hashrate < 1e3) {
            return "";
        } else if (hashrate < 1e6) {
            hashrate /= 1e3;
            return "K";
        } else if (hashrate < 1e9) {
            hashrate /= 1e6;
            return "M";
        } else if (hashrate < 1e12) {
            hashrate /= 1e9;
            return "G";
        } else if (hashrate < 1e15) {
            hashrate /= 1e12;
            return "T";
        }
        return "%@#?";
    };
    std::string s = unit();
    return std::to_string(hashrate) + s + "H/s";
}

/*void test() {
    block b = {
        2,
        "000000000000000117c80378b8da0e33559b5997f2ad55e2f7d18ec1975b9717",
        "871714dcbae6c8193a2bb9b2a69fe1c0440399f38d94b3a0f1b447275a29978a",
        0x53058b35,
        0x19015f53,
        856192328};

    //uint64_t nonce = 0x01f3a5c78;
    //std::cout << uint32_t(-1) << '\n';
    //std::cout << nonce << '\n';
    //std::cout << integer_to_hex(nonce, 8) << '\n';
    //std::cout << byte_reverse_in_hex(integer_to_hex(nonce, 8)) << '\n';
    //std::cout << reverse_bytes(nonce) << '\n';
    //std::cout << reverse_str(integer_to_bytes(reverse_bytes(nonce), 8)) <<
'\n';
    //std::cout << hex_to_bytes(byte_reverse_in_hex(integer_to_hex(nonce, 8)))
    //          << '\n';
    //return 0;

    b.build_data();

    std::cout << "answer: " << b.calc_hash(856192328) << '\n';

    auto start = steady_clock::now();

    const uint64_t N = 0x100000000;

    uint64_t nonce = 856192328;
    std::string target = b.calc_target();
    std::cout << "target: " << target << '\n';
    while (nonce < N) {
        if (nonce % 0x001000000 == 0) {
            std::cout << "progress: " << nonce * 1.0 / N * 100 << "%"
                      << std::endl;
        }
        if (b.calc_hash(nonce) < target) {
            std::cout << nonce << ": " << bytes_to_hex(b.calc_hash(nonce))
                      << std::endl;
            // break;
        }
        nonce++;
    }

    auto stop = steady_clock::now();
    auto duration = stop - start;

    // Nvidia 3050: 10.63 MH/s
    // me: 952H/s -> 57444H/s -> 62398H/s -> 4MH/s

    // 800s перебрал весь nonce

    int64_t hashrate =
        1.0 * N / (duration_cast<nanoseconds>(duration).count() / 1e9);
    std::cout << pretty_hashrate(hashrate) << "\n";
    std::cout << "time calculating: "
              << duration_cast<nanoseconds>(duration).count() / 1e9 << "s\n";
}*/

#include <random>

// return nonce
uint32_t find_hash(block b) {
    b.build_data();

    std::mt19937 rnd(42);

    std::cout << "TARG: " << bytes_to_hex(b.calc_target()) << '\n';

    std::string best = b.calc_hash(0);

    auto find_best_nonce_in_segment = [&](uint32_t l, uint32_t r) {
        uint32_t best = l;
        for (uint32_t nonce = l; nonce <= r; nonce++) {
            std::string best_hash = b.calc_hash(best);
            std::string new_hash = b.calc_hash(nonce);
            if (new_hash < best_hash) {
                best = nonce;
            }
        }
        return best;
    };

    while (true) {
        uint32_t nonce = rnd();

        bool run = true;

        // std::cout << "OPTIMIZED ROUND:\n";
        // std::cout << bytes_to_hex(b.calc_hash(nonce)) << std::endl;
        while (run) {
            run = false;
            for (int bit = 0; bit < 32; bit++) {
                std::string old_hash = b.calc_hash(nonce);
                uint32_t super_nonce = find_best_nonce_in_segment(
                    nonce ^ (1U << bit), nonce ^ (1U << bit) + 10'000
                );
                std::string new_hash = b.calc_hash(super_nonce);
                if (new_hash < old_hash) {
                    nonce = super_nonce;
                    // std::cout << bytes_to_hex(b.calc_hash(nonce)) <<
                    // std::endl;
                    run = true;
                }
            }

            for (int step = 0; step < 32; step++) {
                uint32_t x = rnd();
                std::string old_hash = b.calc_hash(nonce);
                uint32_t super_nonce =
                    find_best_nonce_in_segment(nonce ^ x, nonce ^ x + 10'000);
                std::string new_hash = b.calc_hash(super_nonce);
                if (new_hash < old_hash) {
                    nonce = super_nonce;
                    // std::cout << bytes_to_hex(b.calc_hash(nonce)) <<
                    // std::endl;
                    run = true;
                }
            }

            /*for (int x = 0; x < 32; x++) {
                for (int y = x + 1; y < 32; y++) {
                    std::string old_hash = b.calc_hash(nonce);
                    std::string new_hash =
                        b.calc_hash(nonce ^ (1U << x) ^ (1U << y));
                    if (new_hash < old_hash) {
                        nonce ^= (1U << x) ^ (1U << y);
                        // std::cout << bytes_to_hex(b.calc_hash(nonce)) <<
                        // std::endl;
                        run = true;
                    }
                }
            }

            for (int x = 0; x < 32; x++) {
                for (int y = x + 1; y < 32; y++) {
                    for (int z = y + 1; z < 32; z++) {
                        std::string old_hash = b.calc_hash(nonce);
                        std::string new_hash = b.calc_hash(
                            nonce ^ (1U << x) ^ (1U << y) ^ (1U << z)
                        );
                        if (new_hash < old_hash) {
                            nonce ^= (1U << x) ^ (1U << y) ^ (1U << z);
                            // std::cout << bytes_to_hex(b.calc_hash(nonce)) <<
                            // std::endl;
                            run = true;
                        }
                    }
                }
            }*/
        }
        if (b.calc_hash(nonce) < best) {
            best = b.calc_hash(nonce);
            std::cout << "BEST: " << bytes_to_hex(best) << std::endl;
        }

        // std::cout << "BEST: " << bytes_to_hex(best) << '\n';
        // std::cout << std::endl;

        if (b.calc_hash(nonce) < b.calc_target()) {
            return nonce;
        }
    }
}

/*int main() {
    block b = {
        2,
        "000000000000000117c80378b8da0e33559b5997f2ad55e2f7d18ec1975b9717",
        "871714dcbae6c8193a2bb9b2a69fe1c0440399f38d94b3a0f1b447275a29978a",
        0x53058b35,
        0x19015f53,
        856192328};
    auto start = steady_clock::now();
    std::cout << find_hash(b) << '\n';
    auto stop = steady_clock::now();
    auto duration = stop - start;
    std::cout << "time calculating: "
              << duration_cast<nanoseconds>(duration).count() / 1e9 << "s\n";
}*/

#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace boost::asio;
using ip::tcp;

int main() {
    std::string ip_address = "solo.ckpool.org";
    std::string port = "3333";

    boost::asio::io_context io_context;

    tcp::iostream sockstream([&]() {
        tcp::socket socket(io_context);

        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(ip_address, port);

        boost::system::error_code error_code;
        boost::asio::connect(socket, endpoints, error_code);

        ASSERT(
            !error_code,
            "failed connect to server, message: " + error_code.message()
        );
        return socket;
    }());

    // authorize
    {
        sockstream << "{\"id\": 1, \"method\": \"mining.subscribe\", "
                      "\"params\": []}\n";

        sockstream << "{\"params\": [\"" +
                          std::string("16p9y6EstGYcnofGNvUJMEGKiAWhAr1uR8") +
                          "\", \"password\"], \"id\": 2, \"method\": "
                          "\"mining.authorize\"}\n";
    }

    std::string extranonce1;
    int extranonce2_size;
    // get extranonce
    {
        std::string str;
        sockstream >> str;
        // std::cout << "RESPONSE: >" << str << "<\n";

        json data = json::parse(str);

        extranonce1 = data["result"][1];
        extranonce2_size = data["result"][2];

        std::cout << "extranonce1: " << extranonce1 << "\n\n";
        std::cout << "extranonce2_size: " << extranonce2_size << "\n\n";
    }

    std::string previous_block_hash, coinb1, coinb2;
    uint32_t timestamp, nbits, version;
    std::vector<std::string> merkle_branch;
    do {
        std::string str;
        sockstream >> str;  // skip
        sockstream >> str;

        // std::cout << "DATA: >" << str << "<\n";

        json data_json = json::parse(str);

        previous_block_hash = data_json["params"][1];
        coinb1 = data_json["params"][2];
        coinb2 = data_json["params"][3];
        auto merkle_branch = data_json["params"][4];
        version = hex_to_integer(reverse_str(data_json["params"][5]));
        nbits = hex_to_integer(data_json["params"][6]);
        timestamp = hex_to_integer(data_json["params"][7]);


        std::cout << "job_id: " << data_json["params"][0] << "\n\n";
        std::cout << "previous_block_hash: \"" << previous_block_hash << "\"\n\n";
        std::cout << "coinb1: \"" << coinb1 << "\"\n\n";
        std::cout << "coinb2: \"" << coinb2 << "\"\n\n";
        std::cout << "merkle_branch: " << merkle_branch << "\n\n";
        std::cout << "version: " << version << "\n\n";
        std::cout << "nbits: " << nbits << "\n\n";
        std::cout << "timestamp: " << timestamp << "\n\n";
        std::cout << "clean_jobs: " << data_json["params"][8] << "\n\n";

        for(auto hash : merkle_branch){
            merkle_branch.push_back(hash);
        }
    } while (0);

    block b;
    b.version = version;
    b.previous_block_hash = previous_block_hash;
    b.timestamp = timestamp;
    b.nbits = nbits;
    b.coinb1 = coinb1;
    b.coinb2 = coinb2;
    b.extranonce1 = extranonce1;
    b.extranonce2_size = extranonce2_size;
    b.merkle_branch = merkle_branch;

    std::cout << "target: " << bytes_to_hex(b.calc_target()) << "\n\n";

    std::cout << b.calc_hash(0) << '\n';
}
