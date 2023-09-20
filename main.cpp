#include "block.hpp"
#include <algorithm>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

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

#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace boost::asio;
using ip::tcp;

#include <random>

int main() {
    // solo.ckpool.org
    // bs.poolbinance.com
    // pool.veriblock.cc
    std::string ip_address = "bch.poolbinance.com";
    std::string port = "1800";//"3333";
    std::string worker =
            "StrapleMiner.001";//"16p9y6EstGYcnofGNvUJMEGKiAWhAr1uR8";

    boost::asio::io_context io_context;

    tcp::iostream sockstream([&]() {
        tcp::socket socket(io_context);

        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(ip_address, port);

        boost::system::error_code error_code;
        boost::asio::connect(socket, endpoints, error_code);

        ASSERT(!error_code, "failed connect to server, message: " + error_code.message());
        return socket;
    }());

    std::string extranonce1;
    int extranonce2_size;
    // subscribe
    {
        sockstream << "{\"id\": 1, \"method\": \"mining.subscribe\", \"params\": []}\n";

        std::string response;
        while (true) {
            sockstream >> response;
            std::cout << json::parse(response)["id"] << std::endl;
            if (json::parse(response)["id"] == 1) {
                break;
            }
        }
        std::cout << "SUBSCRIBE RESPONSE: >" << response << "<\n\n";

        json data = json::parse(response);

        extranonce1 = data["result"][1];
        extranonce2_size = data["result"][2];

        std::cout << "extranonce1: " << extranonce1 << "\n\n";
        std::cout << "extranonce2_size: " << extranonce2_size << "\n\n";
    }

    // authorize
    {
        sockstream << "{\"id\": 2, \"method\": \"mining.authorize\", \"params\": [\"" + worker + "\", \"123456\"]}\n";

        std::string response;
        while (true) {
            sockstream >> response;
            if (json::parse(response)["id"] == 2) {
                break;
            }
        }

        std::cout << "AUTHORIZE RESPONSE: >" << response << "<\n\n";

        ASSERT(json::parse(response)["result"] == true, "bad result");
    }

    std::string previous_block_hash, coinb1, coinb2, job_id;
    uint32_t timestamp, nbits, version;
    std::vector<std::string> merkle_branch;
    do {
        json data;
        while (true) {
            std::string response;
            sockstream >> response;
            data = json::parse(response);
            if (data["method"] == "mining.notify") {
                break;
            }
        }

        std::cout << "DATA: >" << data << "<\n\n";

        job_id = data["params"][0];
        previous_block_hash = data["params"][1];
        coinb1 = data["params"][2];
        coinb2 = data["params"][3];
        merkle_branch = data["params"][4];
        version = hex_to_integer(reverse_str(data["params"][5]));
        nbits = hex_to_integer(data["params"][6]);
        timestamp = hex_to_integer(data["params"][7]);

        std::cout << "job_id: " << job_id << "\n\n";
        std::cout << "previous_block_hash: \"" << previous_block_hash<< "\"\n\n";
        std::cout << "coinb1: \"" << coinb1 << "\"\n\n";
        std::cout << "coinb2: \"" << coinb2 << "\"\n\n";
        std::cout << "merkle_branch: " << data["params"][4] << "\n\n";
        std::cout << "version: " << version << "\n\n";
        std::cout << "nbits: " << nbits << "\n\n";
        std::cout << "timestamp: " << timestamp << "\n\n";
        std::cout << "clean_jobs: " << data["params"][8] << "\n\n";
    } while (0);

    auto submit = [&](uint32_t nonce, const std::string &extranonce2) {
        sockstream << "{\"params\": [\"" + worker + "\", \"" + job_id +
                              "\", \"" + extranonce2 + "\", \"" +
                              integer_to_hex(timestamp, 8) + "\", \"" +
                              integer_to_hex(nonce, 8) +
                              "\"], \"id\": 4, \"method\": \"mining.submit\"}\n";

        json response;
        while (true) {
            std::string str;
            sockstream >> str;
            try {
                response = json::parse(str);
                if (response["id"] == 4) {
                    break;
                }
            } catch (...) {
                std::cerr << "json parse error: >" << str << "<\n";
                break;
            }
        }

        std::cout << "SUBMIT RESPONSE: >" << response << "<\n\n";
    };

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

    b.build_data();

    {
        std::cout << "START MINING...\n\n";
        std::cout << "target: " << bytes_to_hex(b.calc_target()) << "\n\n";

        /*
        N = 256 * 256 * 256;
        progress: 0% 0H/s
        progress: 10% 4MH/s
        progress: 20% 4MH/s
        progress: 30% 4MH/s
        progress: 40% 4MH/s
        progress: 50% 4MH/s
        progress: 60% 4MH/s
        progress: 70% 4MH/s
        progress: 80% 4MH/s
        progress: 90% 4MH/s
        4MH/s
        time calculating: 3.47532s
        */

        const uint64_t N = 256 * 256;// 0x100000000;
        std::string target = b.calc_target();

        auto start = steady_clock::now();

        std::mt19937 rnd(42);

        // (hash, block)
        /*std::vector<std::pair<std::string, block>> queue;

        for (int i = 0; i < 10'000; i++) {
            block x = b;
            uint32_t extranonce2 = rnd();
            x.extranonce2 = integer_to_hex(extranonce2, extranonce2_size * 2);
            x.nonce = rnd();

            x.build_data();
            queue.emplace_back(x.calc_hash(), x);
        }

        std::sort(queue.begin(), queue.end());

        int steps = 0;

        std::string best_hash = b.calc_hash();
        std::cout << "BEST: " << bytes_to_hex(best_hash) << "\n\n";

        auto start = steady_clock::now();

        while (true) {
            auto [hash, block] = queue[0];
            if (hash < best_hash) {
                best_hash = hash;
                std::cout << "NEW BEST: " << bytes_to_hex(best_hash) << "\n\n";
            }

            if (steps % 0x000100000 == 0) {
                int64_t hashrate =
                    1.0 * steps /
                    (duration_cast<nanoseconds>(steady_clock::now() - start)
                         .count() /
                     1e9);
                std::cout << "progress: " << steps * 1.0 / N * 100 << "% "
                          << pretty_hashrate(hashrate) << std::endl;
            }

            if (hash < target) {
                std::cout << "COMPLETED!!!: " << block.nonce << ": "
                          << bytes_to_hex(b.calc_hash()) << std::endl;
                submit(block.nonce, block.extranonce2);
            }

            // 8 нулей в хеше нужно было перебрать 70% из 2^32 значений
            if (hash[0] == 0 && hash[1] == 0 && hash[2] == 0 && hash[3] == 0) {
                std::cout << "FIND NICE: " << block.nonce << ": "
                          << bytes_to_hex(b.calc_hash()) << std::endl;
                submit(block.nonce, block.extranonce2);
            }

            std::vector<std::pair<std::string, ::block>> new_que;
            for (auto [hash, block] : queue) {
                for (int step = 0; step < 100; step++) {
                    uint32_t x = rnd();
                    block.nonce ^= x;
                    std::string hash = block.calc_hash();

                    steps++;
                    new_que.emplace_back(hash, block);

                    block.nonce ^= x;
                }
            }
            std::sort(new_que.begin(), new_que.end());

            while (new_que.size() > 1000) {
                new_que.pop_back();
            }

            queue = std::move(new_que);
        }*/

        /*while (true) {
            auto [hash, block] = *queue.begin();
            queue.erase(queue.begin());
            // std::cout << bytes_to_hex(hash) << "\n\n";

            if (hash < best_hash) {
                best_hash = hash;
                std::cout << "NEW BEST: " << bytes_to_hex(best_hash) << "\n\n";
            }

            if (steps % 0x000100000 == 0) {
                int64_t hashrate =
                    1.0 * steps /
                    (duration_cast<nanoseconds>(steady_clock::now() - start)
                         .count() /
                     1e9);
                std::cout << "progress: " << steps * 1.0 / N * 100 << "% "
                          << pretty_hashrate(hashrate) << std::endl;
            }

            if (hash < target) {
                std::cout << "COMPLETED!!!: " << block.nonce << ": "
                          << bytes_to_hex(b.calc_hash()) << std::endl;
                submit(block.nonce, block.extranonce2);
            }

            // 8 нулей в хеше нужно было перебрать 70% из 2^32 значений
            if (hash[0] == 0 && hash[1] == 0 && hash[2] == 0 && hash[3] == 0) {
                std::cout << "FIND NICE: " << block.nonce << ": "
                          << bytes_to_hex(b.calc_hash()) << std::endl;
                submit(block.nonce, block.extranonce2);
            }

            for (int step = 0; step < 10; step++) {
                uint32_t x = rnd();
                block.nonce ^= x;
                std::string hash = block.calc_hash();

                steps++;
                queue.insert(std::make_pair(hash, block));

                block.nonce ^= x;
            }

            while (queue.size() > 10'000) {
                queue.erase(--queue.end());
            }
        }*/

        uint32_t extranonce2 = rnd();
        b.extranonce2 = integer_to_hex(extranonce2, extranonce2_size * 2);
        b.build_data();

        // 348KH/s -> 433KH/s

        for (uint64_t x = 0; x < N; x++) {
            if (x % 0x100 == 0) {
                int64_t hashrate = 1.0 * (x*256) / (duration_cast<nanoseconds>(steady_clock::now() - start).count() / 1e9);
                std::cout << "progress: " <<x * 1.0 / N * 100 << "% " << pretty_hashrate(hashrate) << std::endl;
            }
            auto [nonce, hash] = b.calc_hash(x);

            //std::cout << bytes_to_hex(hash) << " " << (nonce >> 24) << " " << (nonce & ((1 << 24) - 1))<< "\n";

            // validate find_best_hash
            /*{
                uint32_t best_byte = 0;
                for (uint32_t byte = 0; byte < 256; byte++) {
                    if (b.trivial_calc_hash(x | (byte << 24)) <
                        b.trivial_calc_hash(x | (best_byte << 24))) {
                        best_byte = byte;
                    }
                }
                //std::cout << nonce << ' ' << (x | (best_byte << 24)) << '\n';
                //std::cout << (nonce >> 24) << " " << best_byte << '\n';
                //std::cout << bytes_to_hex(hash) << "\n"
                //          << bytes_to_hex(
                //                 b.trivial_calc_hash(x | (best_byte << 24))
                //             )
                //          << "\n";
                ASSERT(
                    hash == b.trivial_calc_hash(x | (best_byte << 24)),
                    "failed find best"
                );
            }*/

            // std::cout << bytes_to_hex(b.calc_hash(nonce)) << '\n' <<
            // bytes_to_hex(b.trivial_calc_hash(nonce)) << '\n';
            // std::cout << bytes_to_hex(b.calc_hash(nonce)) << '\n';
            // std::cout << bytes_to_hex(target) << '\n';
            if (hash < target) {
                std::cout << "COMPLETED!!!: " << nonce << ": "
                          << bytes_to_hex(hash) << std::endl;
                submit(nonce, b.extranonce2);
            }

            // 8 нулей в хеше нужно было перебрать 70% из 2^32 значений
            if (hash[0] == 0 && hash[1] == 0 && hash[2] == 0 && hash[3] == 0) {
                std::cout << "FIND NICE: " << nonce << ": "
                          << bytes_to_hex(hash) << std::endl;
                submit(nonce, b.extranonce2);
            }
        }

        /*for (uint64_t nonce = 0; nonce < N; nonce++) {
            if (nonce % 0x001000000 == 0) {
                int64_t hashrate =
                    1.0 * nonce /
                    (duration_cast<nanoseconds>(steady_clock::now() - start)
                         .count() /
                     1e9);
                std::cout << "progress: " << nonce * 1.0 / N * 100 << "% "
                          << pretty_hashrate(hashrate) << std::endl;
            }
            b.nonce = nonce;
            std::string hash = b.calc_hash();
            // ASSERT(b.calc_hash(nonce) == b.trivial_calc_hash(nonce),
            //"failed");
            // std::cout << bytes_to_hex(b.calc_hash(nonce)) << '\n' <<
            // bytes_to_hex(b.trivial_calc_hash(nonce)) << '\n';
            // std::cout << bytes_to_hex(b.calc_hash(nonce)) << '\n';
            // std::cout << bytes_to_hex(target) << '\n';
            if (hash < target) {
                std::cout << "COMPLETED!!!: " << nonce << ": "
                          << bytes_to_hex(hash) << std::endl;
                submit(nonce, std::string(extranonce2_size * 2, '0'));
            }

            // 8 нулей в хеше нужно было перебрать 70% из 2^32 значений
            if (hash[0] == 0 && hash[1] == 0 && hash[2] == 0 && hash[3] == 0) {
                std::cout << "FIND NICE: " << nonce << ": "
                          << bytes_to_hex(hash) << std::endl;
                submit(nonce, std::string(extranonce2_size * 2, '0'));
            }
        }*/

        auto stop = steady_clock::now();
        auto duration = stop - start;

        // Nvidia 3050: 10.63 MH/s
        // me: 952H/s -> 57444H/s -> 62398H/s -> 4MH/s

        // 800s перебрал весь nonce

        int64_t hashrate =
                1.0 * N / (duration_cast<nanoseconds>(duration).count() / 1e9);
        std::cout << pretty_hashrate(hashrate) << "\n";
        std::cout << "time calculating: "
                  << duration_cast<nanoseconds>(duration).count() / 1e9
                  << "s\n";
    }
}
