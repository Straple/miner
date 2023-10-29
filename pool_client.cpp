#include "pool_client.hpp"

#include "assert.hpp"
#include "bits_manipulation.hpp"
#include "utils.hpp"
#include <iostream>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "constants.hpp"

void PoolClient::init() {
    // connect
    sockstream = std::move(tcp::iostream([&]() {
        logger.print("CONNECTION...");
        tcp::socket socket(io_context);

        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(IP_ADDRESS, PORT);

        boost::system::error_code error_code;
        boost::asio::connect(socket, endpoints, error_code);

        ASSERT(!error_code, "failed connect to server, message: " + error_code.message());
        logger.print("SUCCESS");
        return socket;
    }()));

    subscribe();

    authorize();
}

PoolClient::PoolClient() : logger("pool_client.txt") {
    //init();
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

    extranonce1 = std::string(data["result"][1]);
    extranonce2_size = data["result"][2];

    logger.print("extranonce1: ", extranonce1);
    logger.print("extranonce2_size: ", std::to_string(extranonce2_size));
}

void PoolClient::authorize() {
    logger.print("AUTHORIZATION...");

    sockstream << R"({"id": 2, "method": "mining.authorize", "params": [")" + WORKER_NAME + "\", \"123456\"]}\n";

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

    fast_string previous_block_hash;
    std::string coinb1, coinb2;
    uint32_t nbits, version, timestamp;
    std::vector<fast_string> merkle_branch;
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

        job_id = std::string(data["params"][0]);
        previous_block_hash = std::string(data["params"][1]);
        coinb1 = std::string(data["params"][2]);
        coinb2 = std::string(data["params"][3]);

        //merkle_branch = data["params"][4];
        for (auto value: data["params"][4]) {
            merkle_branch.emplace_back(std::string(value));
        }

        version = hex_to_integer(reverse_str(std::string(data["params"][5])));
        nbits = hex_to_integer(std::string(data["params"][6]));
        timestamp = hex_to_integer(std::string(data["params"][7]));

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

    b.build_extranonce2();

    if (new_miner_task) {
        logger.print("DETECTED NEW BLOCK");
    }

    return b;
}

void PoolClient::submit(const block &b) {
    logger.print("SUBMIT...");

    sockstream << R"({"params": [")" + WORKER_NAME + R"(", ")" + job_id.to_str() +
                          R"(", ")" + b.extranonce2.to_str() + R"(", ")" +
                          integer_to_hex(b.timestamp, 8).to_str() + R"(", ")" +
                          integer_to_hex(b.nonce, 8).to_str() +
                          R"("], "id": 4, "method": "mining.submit"})"
               << "\n";

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
            logger.print("connection failed");
            return;
        }
    }

    logger.print("SUBMIT RESPONSE: >" + nlohmann::to_string(response) + "<");
}

bool PoolClient::reading_is_available() {
    return sockstream.rdbuf()->available() != 0;
}

void PoolClient::update_connection() {
    if (sockstream.error()) {
        logger.print("connection failed: ", sockstream.error());
        logger.print("try to reconnect");
        init();
    }
}