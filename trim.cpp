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
/*uint32_t find_hash(block b) {
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
}*/

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