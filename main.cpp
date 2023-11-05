// #pragma comment(linker, "/STACK:66777216")

#include <chrono>
#include <iostream>
#include <vector>

using namespace std::chrono;

#include "assert.hpp"
#include "bits_manipulation.hpp"
//#include "constants.hpp"
//#include "miner.hpp"
//#include "pool_client.hpp"

#include <algorithm>

#include "sha256_solver/sha256.hpp"

std::vector<byte_t> calc_sha256(const std::vector<byte_t> &data) {
    SHA256 sha;
    sha.update(data);
    auto digest = sha.digest();
    return digest;
}

std::vector<byte_t> build_digest(const fast_string &bytes) {
    std::vector<byte_t> data(bytes.size());
    for (int i = 0; i < bytes.size(); i++) {
        data[i] = static_cast<unsigned char>(bytes[i]);
    }
    for (uint32_t index = 0; index < SOLVER_LEN; index++) {
        data.back()[index] = index;
        //data[data.size() - 2][index] = index >> 8;
        //data[data.size() - 3][index] = index >> 16;
    }
    return calc_sha256(calc_sha256(data));
}

// принимает строку из байтов
// последние 2 байта являются X
// мы ищем лучший X, чтобы получить как можно меньший хеш
// возвращаем этот X и hash
std::pair<uint32_t, fast_string> find_best_hash(const fast_string &block_bytes_data) {
    auto digest = build_digest(block_bytes_data);
    std::reverse(digest.begin(), digest.end());

    auto less_compare = [&](uint32_t lhs, uint32_t rhs) {
        uint32_t index = 0;
        for (; index < digest.size() && digest[index][lhs] == digest[index][rhs]; index++) {
        }
        return index < digest.size() && digest[index][lhs] < digest[index][rhs];
    };

    uint32_t best_x = 0;
    for (uint32_t x = 0; x < SOLVER_LEN; x++) {
        if (less_compare(x, best_x)) {
            best_x = x;
        }
    }

    fast_string hash;
    for (uint32_t i = 0; i < digest.size(); i++) {
        hash += digest[i][best_x];
    }
    return {best_x, hash};
}

#include "lite_block.hpp"
#include "utils.hpp"

int main(int argc, char **argv) {
    std::string block_bytes_data = hex_to_bytes(std::string("02000000f0bf96a69b0253ef9defec6344de196426fbad670001edb20000000000000000cda56413abdfa7af235fc46d19a7d121d3672f4551a211c9620ba65f0f7d4a3c97203e65a99c041731b331c1"));
    fast_string str = block_bytes_data;
    std::cout << bytes_to_hex(reverse_str(lite_block(str).calc_hash(3241259825))) << std::endl;

    for (int i = 0; i < 4; i++) {
        block_bytes_data[76 + i] = 0;
    }

    /*lite_block block(block_bytes_data);
    for (uint32_t x = 0; x < (1 << 16); x++) {
        uint32_t nonce = ((uint32_t(3241259825) << 16) >> 16) | (x << 16);
        if (nonce == 3241259825) {
            std::cout << "YES " << x << std::endl;
        }
        if (block.calc_hash(nonce).builtin_ctz() > 20) {
            std::cout << bytes_to_hex(reverse_str(lite_block(str).calc_hash(nonce))) << std::endl;
        }
    }*/
    const int N = 100000;

    auto time_start = steady_clock::now();
    for (uint32_t x = 0; x < N /*(1 << 24)*/; x++) {
        for (int i = 0; i < 1; i++) {
            block_bytes_data[76 + i] = (x >> (8 * i)) & 0xff;
        }
        //std::cout << bytes_to_hex(block_bytes_data) << std::endl;

        // 7MH/s
        /*lite_block block(block_bytes_data);
        uint32_t best_X = 0;
        fast_string best_hash = block.calc_hash(x | (best_X << 16));
        for(uint32_t z = 0; z < (1 << 16); z++){
            uint32_t nonce = x | (z << 16);
            fast_string hash = block.calc_hash(nonce);
            if(hash < best_hash){
                best_hash = hash;
                best_X = z;
            }
        }*/

        // 2 bytes
        // 270KH/s -> 330KH/s -> 450KH/s -> 510KH/s

        // 1 bytes
        // 670KH/s -> 888KH/s
        auto [best_X, best_hash] = find_best_hash(block_bytes_data);

        //std::cout << bytes_to_hex(lite_block(block_bytes_data).calc_hash(x | (best_X << 16))) << std::endl;
        //std::cout << bytes_to_hex(reverse_str(best_hash)) << std::endl;
        // for find_best_hash
        //ASSERT(lite_block(block_bytes_data).calc_hash(x | (best_X << 8)) == reverse_str(best_hash), "failed");

        // for block calc_hash
        //ASSERT(lite_block(block_bytes_data).calc_hash(x | (best_X << 16)) == best_hash, "failed");


        // auto time_stop = steady_clock::now();
        // auto duration = time_stop - time_start;
        // double time = duration_cast<nanoseconds>(duration).count() / 1e9;
        // uint64_t hashrate = ((x + 1) * SOLVER_LEN) / time;
        //
        // std::cout << x << ' ' << time << ' ' << pretty_hashrate(hashrate) << ' ' << bytes_to_hex(best_hash) << '\n';
    }

    auto time_stop = steady_clock::now();
    auto duration = time_stop - time_start;
    double time = duration_cast<nanoseconds>(duration).count() / 1e9;
    uint64_t hashrate = (N * SOLVER_LEN) / time;

    std::cout << N << ' ' << time << ' ' << pretty_hashrate(hashrate);// << ' ' << bytes_to_hex(best_hash) << '\n';


    /*uint32_t nonce = ((uint32_t(3241259825) << 16) >> 16);
    for (int i = 0; i < 2; i++) {
        block_bytes_data[76 + i] = (nonce >> (8 * i)) & 0xff;
    }
    std::cout << bytes_to_hex(block_bytes_data) << std::endl;

    auto time_start = steady_clock::now();
    auto [best_X, hash] = find_best_hash(block_bytes_data);
    auto time_stop = steady_clock::now();
    auto duration = time_stop - time_start;
    double time = duration_cast<nanoseconds>(duration).count() / 1e9;

    std::cout << "time: " << time << "s\n";
    std::cout << (SOLVER_LEN / time) << '\n';

    std::cout << best_X << '\n'
              << bytes_to_hex(hash) << '\n';*/
}

//0000000000a939d0b921d320901ac82f0723dd767eb4b6e213fe3716b2279fe8
//0 0.327679 200.0KH/s 000083179839c22862f927752c2238581615390fef36590fa938e62db3b612a8
//1 0.580019 225.9KH/s 0000ffe45d591bcabd45333479173ba3fbe8ee4cf2f572ddbf9c123c07b08d11
//2 0.797838 246.4KH/s 000004496d8179db7cb165f754a66b1580cc5b559acc675d4964f608fe58807d
//3 1.0167 257.8KH/s 00002db9a294e8a19d0e3ebe0cb831571238128994be454b444eea72133e5ba4
//4 1.23543 265.2KH/s 000128120990e8c14c7784a0ff3837edcfb6ae2d1a0025beb80b63276e6bc7ec
//5 1.45565 270.1KH/s 0000025abaad7202deb125f0e48a81d342611bbc814196816021d52750fd15a4
//6 1.71552 267.4KH/s 0000591a5de1b7bebe6c0f1ea6942a23aa00ab0f109430f87352a590c1c07b12
//7 1.92629 272.1KH/s 00000a942b7c616ba0f18746888bbfbb07d52178279cf026dc640101aeae2356
//8 2.14256 275.2KH/s 00026f70d6a91943fbb27af3a84a67d962e8b2ec8b28cbd3d1274f97c64c3630
//9 2.37991 275.3KH/s 0000c40c40a5746f99c9042948b9437058955b84e3fad5c773c2d6de9aba0ae9
//10 2.59002 278.3KH/s 0000ac688ce1330ea2db6273bc1918bfc22ac574e730d0673b537319622779b3
//11 2.82281 278.5KH/s 00013823330c0eea988638e6b44b1d52785b56bb2a02accdbeb23b158c8c37c5
//12 3.06227 278.2KH/s 00033cfdc28d11d759ce5390be5a29f3e590390562f00d06d1831b05679b86e2
//13 3.30039 277.9KH/s 0000305f77f1d1c47bb2f385e7a7ef740829ffce398a0209f778891cf4ded560
//14 3.52545 278.8KH/s 0001b5c15a44a90c4ed3a6a0d8027c8abc802e0deb27ca26f650071bf01e74b3
//15 3.7508 279.5KH/s 000039b0df03eb24094120bdf65ba4c33e8239dd200d8e2402cc92eedddd5895
//16 3.97331 280.3KH/s 000066aa95d89410027b99920ee03bb29badcdea5e920f086e5f5dc4224b8a5f
//17 4.2048 280.5KH/s 00004f3f8c5f30633b4416aebeec399b938e980e7f1fd1958365801f7ad8bd60
//18 4.42635 281.3KH/s 00002ab40e59b075a1a484df932b5ecb5a33ca87cd7880f63c9dee00c12e2928
//19 4.66477 280.9KH/s 000018d16e1a7871145bb3c53db3d1e766bdea6d6d0d92dcc0effbc98117ddf8
//20 4.88391 281.7KH/s 0000c0a4e1f836dd0b49e4360da0bf3aef586b4c77c08786abfb92ae22fda91c
//21 5.12825 281.1KH/s 000326aa94cb0cf5b593cde44b6e20e301d0079b4639f01ed3be2aaab4e34837
//22 5.33494 282.5KH/s 000351bf636a959072c94326b48d20887d779ab4d82b26d5249be687692467a1
//23 5.56894 282.4KH/s 0000ff270721c2f40ad33cbd2fd895c798f4774ab9fb16fd617176829d491fd5
//24 5.77839 283.5KH/s 00022ed8bfc898d8fde4b23b5b32dd631ca666aab05c327fa11ecfb8f6745a0f
//25 6.00914 283.5KH/s 0000291b4a9ff9a174f387e00345bf0ff540851a4fb71c41de5440c70129f464
//26 6.21683 284.6KH/s 000039a151b35cdb01a7507aeb4548211eab08f2a60436f1fd46b1f8b50687ba
//27 6.44759 284.6KH/s 00017dd74d51b01bce1b30920bb53352c3d4507531dd40d437b99b985ea155ac
//28 6.69098 284.0KH/s 000195189ad430f9068f181af5e3ef7b8453fb5a6166d03d5a0351d63ff47ba6
//29 6.91756 284.2KH/s 000037ad2ca00968433c1ab6ff2b2c3c094566c259a902e4756b1f127015ef2f
//30 7.18547 282.7KH/s 000364ed2b7eb7861a9b20e7c984db0ca5c88476bd677f887f1cd5a24fc0215c
//31 7.40947 283.0KH/s 0001e5502feebb62a84693fa0c1ccf779aee55180171181303c97b5f38bc89e8
//32 7.71135 280.4KH/s 0000ab115c18fbcb6de66f5c9be33f9e1e3b528442acc0d1f6afee9dbc5df731
//33 7.94282 280.5KH/s 0000c686e234b49d31f1f6199c9265db94be806f0e9412573ce79ff2ef3173fe
//34 8.17258 280.6KH/s 000128955e2dfec1124af3d2769589bb81670a600813c965fa2e7bbc7bb0ff0e
//35 8.40051 280.8KH/s 00004251516c5449854c2f376e7117f93570e8ef238d0f9deb630a3f9a937f70
//36 8.62114 281.2KH/s 0000c6197b8c87447564ca6ff7ea38b9b76b357f93786c90175e98d15896d82b
//37 8.87027 280.7KH/s 00004ac7761bbdaf438edc71915e63037cf4017e8e51cad37849bb256a5b0f01
//38 9.12961 279.9KH/s 00001f813b1fe1f018fd32882b922973667fcf60aeeb468949a3958a1404a2e5
//39 9.34715 280.4KH/s 000073988754d5104e0ada9061b7f7feb478ad5701d7e3385f6ad048b502fd68
//40 9.57247 280.6KH/s 0000014cb8b9ee797f77460ee2479a85f133089bd38a02b902539fbb3a1e8a1e
//41 9.82467 280.1KH/s 000249b072f21e7e912a39f58753e2378b2327a47e761c826328b476d9962c34
//42 10.0387 280.7KH/s 0000b3d76e5765afb756e01764f18497192f1167c9f27ad13e91f09322637d22
//43 10.2771 280.5KH/s 000120bf7a67d0dcb4520cffe43a492cd8ba19ecbc9850c815742f50968933c1
//44 10.5003 280.8KH/s 00010c656f04d74aad8831bc14598ae20617aeee726c24d22603f8ce18e2daa3
//45 10.8098 278.8KH/s 0000c79cd6516b4a747bee7fec30307a47197aeff96daa20b1cc32cdd6afe7c8
//46 11.0743 278.1KH/s 0004dc6c4cba8fc315c2a6eabab238472759c8e35c0d198dcb8b23932805fade
//47 11.2968 278.4KH/s 00006f2cf3c527ebd7c6163afc45e166b5c4b3bc015d620e696a02f0964785c8
//48 11.5871 277.1KH/s 0000484d97b0563df033020cfd340110208285a2c1b72dadedbd204c061390ff
//49 11.8669 276.1KH/s 000010b06e0737bc3a125deb0bf3bc7df3401342d4dece2c7111dae0a227580a
//50 12.1892 274.2KH/s 00006cac00b35d99e4af6555e1222ea94ee46502f6542cb09a2dca53dc45b5d1
//51 12.42 274.3KH/s 000039d3ada304b64043779c86857b1747818685d3bece786ffff663e2709af1
//52 12.6465 274.6KH/s 00002f6547380867d47c84ef1ad0e3da4ca1ce17f4f64ebd6e4ec71ce767d9e2
//53 12.9557 273.1KH/s 00006025f38b93729c3fcc8fca25f17ad388289f2390a49bf2f88003d00fcf20
//54 13.2021 273.0KH/s 0001dbd184188f9078aedc2587deef5edfdea1e1fe3df3589cf5c14633e0dc25
//55 13.5443 270.9KH/s 0001cee4d01e462862af0f4218be6aa466d1c2168f58c97a57424e80b3574150
//56 13.8051 270.5KH/s 0000b0df968533fa7d5c09869c542602c9ad1718d57e41d003e2474dda1e7867
//57 14.0279 270.9KH/s 000269b889c6132b27a59c07b1263087610f6f81e9b65567294213b43e76091b
//58 14.3691 269.0KH/s 0000c88b37349ee14e6874aeec2b6be751bd35d3b3e548e3f50aca6939d738e9
//59 14.5985 269.3KH/s 00007a752de123fb48ceea16cbad3c169243eb0edc8b27beb959e01f98c2a553
//60 14.8322 269.5KH/s 000222ff8d5cda3d389e7061d40027ac1eea52161943a60c18bac9559c401481
//61 15.0724 269.5KH/s 00011c6086041f3daf21951f072c9d02a54b65541212f53700669cd5b60d3289
//62 15.2984 269.8KH/s 00001834cfee819291da59d54d69c98b6d75255d7edd394a5224bc2405ee811e
//63 15.583 269.1KH/s 0001cf200a3f5abe92beb9acf923a7a6276fb4b0d5849328fa601b1ad592cb09
//64 15.8558 268.6KH/s 0000bf7bbf805f675101b9ad274ccdb109e2987197acf651541e27d36fc3397c
//65 16.0972 268.7KH/s 00010e9fcc78ccfedc6f5993ea3f1caa721008e5a28c498d91efdeef425facdf
//66 16.3805 268.0KH/s 00013a216743f0f00a3a8401b242d2d431dbceb03bff5fb91f1f9599938f490e
//67 16.636 267.8KH/s 00007c98d3422c6373232ab75259dec9450ef8777546e395c63395e19a53bc32
//68 16.8688 268.0KH/s 0000770bc58a1c88fca0bf1542a56e34b6f5470df758163d13c0f1b4f80f54ee