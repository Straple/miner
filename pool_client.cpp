#include "pool_client.hpp"

#include "assert.hpp"
#include "bits_manipulation.hpp"
#include "utils.hpp"
#include <iostream>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

// solo.ckpool.org
// bs.poolbinance.com
// pool.veriblock.cc
const static std::string ip_address = "bch.poolbinance.com";
const static std::string port = "1800";              //"3333";
const static std::string worker = "StrapleMiner.001";//"16p9y6EstGYcnofGNvUJMEGKiAWhAr1uR8";

PoolClient::PoolClient()
    : sockstream([&]() {
          std::cout << "CONNECTION..." << std::endl;
          tcp::socket socket(io_context);

          tcp::resolver resolver(io_context);
          auto endpoints = resolver.resolve(ip_address, port);

          boost::system::error_code error_code;
          boost::asio::connect(socket, endpoints, error_code);

          ASSERT(!error_code, "failed connect to server, message: " + error_code.message());
          std::cout << "OK\n\n";
          std::cout.flush();
          return socket;
      }()),
      logger("pool_client.txt") {

    subscribe();

    authorize();
}

void PoolClient::subscribe() {
    logger.print("SUBSCRIBE...");

    sockstream << "{\"id\": 1, \"method\": \"mining.subscribe\", \"params\": []}\n";

    std::string response;
    while (true) {
        sockstream >> response;
        if (json::parse(response)["id"] == 1) {
            break;
        }
    }
    logger.print("SUBSCRIBE RESPONSE: >" + response + "<");

    json data = json::parse(response);

    extranonce1 = data["result"][1];
    extranonce2_size = data["result"][2];

    logger.print("extranonce1: " + extranonce1);
    logger.print("extranonce2_size: " + std::to_string(extranonce2_size));
}

void PoolClient::authorize() {
    logger.print("AUTHORIZATION...");

    sockstream << "{\"id\": 2, \"method\": \"mining.authorize\", \"params\": [\"" + worker + "\", \"123456\"]}\n";

    std::string response;
    while (true) {
        sockstream >> response;
        if (json::parse(response)["id"] == 2) {
            break;
        }
    }

    logger.print("AUTHORIZE RESPONSE: >" + response + "<");
    ASSERT(json::parse(response)["result"] == true, "bad result");
}

block PoolClient::get_new_block(bool &new_miner_task) {
    logger.print("GET_NEW_BLOCK...");

    std::string previous_block_hash, coinb1, coinb2, job_id;
    uint32_t nbits, version, timestamp;
    std::vector<std::string> merkle_branch;
    {
        json data;
        while (true) {
            std::string response;
            sockstream >> response;
            data = json::parse(response);
            if (data["method"] == "mining.notify") {
                break;
            }
        }

        //std::cout << "DATA: >" << data << "<\n\n";

        job_id = data["params"][0];
        previous_block_hash = data["params"][1];
        coinb1 = data["params"][2];
        coinb2 = data["params"][3];
        merkle_branch = data["params"][4];
        version = hex_to_integer(reverse_str(data["params"][5]));
        nbits = hex_to_integer(data["params"][6]);
        timestamp = hex_to_integer(data["params"][7]);

        /*std::cout << "job_id: " << job_id << "\n\n";
            std::cout << "previous_block_hash: \"" << previous_block_hash << "\"\n\n";
            std::cout << "coinb1: \"" << coinb1 << "\"\n\n";
            std::cout << "coinb2: \"" << coinb2 << "\"\n\n";
            std::cout << "merkle_branch: " << data["params"][4] << "\n\n";
            std::cout << "version: " << version << "\n\n";
            std::cout << "nbits: " << nbits << "\n\n";
            std::cout << "timestamp: " << timestamp << "\n\n";
            std::cout << "clean_jobs: " << data["params"][8] << "\n\n";*/

        new_miner_task = data["params"][8];
    }

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
    b.job_id = job_id;

    b.build_extranonce2();

    if(new_miner_task){
        logger.print("DETECTED NEW BLOCK");
    }

    return b;
}

void PoolClient::submit(const block &b) {
    logger.print("SUBMIT...");

    sockstream << "{\"params\": [\"" + worker + "\", \"" + b.job_id +
                          "\", \"" + b.extranonce2 + "\", \"" +
                          integer_to_hex(b.timestamp, 8) + "\", \"" +
                          integer_to_hex(b.nonce, 8) +
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

    logger.print("SUBMIT RESPONSE: >" + nlohmann::to_string(response) + "<");
}

bool PoolClient::reading_is_available() {
    return sockstream.rdbuf()->available() != 0;
}