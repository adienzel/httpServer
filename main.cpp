#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <random>

#include "env.h"

using tcp = boost::asio::ip::tcp;       // from <boost/asio.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>


std::random_device device{};
std::mt19937 generator(device());
std::uniform_int_distribution<long> distribution(1, (long) 1e12);
static long transactionCounter = distribution(generator);

// Function to generate random text
std::string generate_random_text() {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string result;
    auto length = distribution(generator) % 1024;
    if (length < 10) {
        return "";
    }
    result.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        result += charset[distribution(generator) % (sizeof(charset) - 1)];
    }
    return result;
}

// Function to handle HTTP requests
void handle_request(http::request<http::string_body>& req, http::response<http::string_body>& res, long ns) {
    //auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    
    res.version(req.version());
    res.result(http::status::ok);
    res.set(http::field::server, "OUR.TEST.SERVER/28");
    
    auto it = req.find("X-Arrived-time");
    if (it != req.end()) {
        res.set("X-Arrived-time", it->value());
    }
    it = req.find("X-Start-Time");
    if (it != req.end()) {
        res.set("X-Start-Time", it->value());
    }
    res.set("X-App-time", std::to_string(ns));
    auto body = generate_random_text(); // Generate random text of length 100
    if (!body.empty()) {
        res.set(http::field::content_type, "application/text");
        res.body() = body;
        res.prepare_payload();
    }
    auto ns1 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    
    
}

// Function to handle connections
void do_session(tcp::socket& socket) {
    try {
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    
        boost::beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(socket, buffer, req);
        
        http::response<http::string_body> res;
        handle_request(req, res, ns);
        
        http::write(socket, res);
        auto ns1 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        std::cout << "message sent after : " << (ns1 - ns)/1000000 << std::endl;
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

// Main function to set up the server
int main() {
    auto port = (uint16_t)EnvUtils::getEnvInt("HTTP_SERVER_PORT_PORT", 8080);
    
    try {
        auto const address = boost::asio::ip::make_address("0.0.0.0");
        //auto port = static_cast<unsigned short>(std::atoi("8080"));
        
        boost::asio::io_context ioc{1};
        tcp::acceptor acceptor{ioc, {address, port}};
//        tcp::socket socket{ioc};
        
        
        std::cout << "Server is running on port " << port << std::endl;
    
    
        // Create a pool of threads to run the io_context
        std::vector<std::thread> threads;
        //future add parameter for num of thereads or thake all allocated 
        unsigned int num_threads = 4; //std::thread::hardware_concurrency();
        for (unsigned int i = 0; i < num_threads; ++i) {
            threads.emplace_back([&ioc] { ioc.run(); });
        }
    
        while (true) {
            tcp::socket socket{ioc};
            acceptor.accept(socket);
            std::thread{std::bind(&do_session, std::move(socket))}.detach();
    
//            acceptor.accept(socket);
//            do_session(socket);
//            socket.shutdown(tcp::socket::shutdown_send);
        }
    
        // Join the threads
        for (auto& t : threads) {
            t.join();
        }
    
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
