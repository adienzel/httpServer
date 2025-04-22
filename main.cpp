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
    auto length = distribution(generator) % 4096;
    result.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        result += charset[distribution(generator) % (sizeof(charset) - 1)];
    }
    return result;
}

// Function to handle HTTP requests
void handle_request(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    auto now = std::chrono::system_clock::now();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count() % 1000000000;
    
    res.version(req.version());
    res.result(http::status::ok);
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set("Time-Arrival-Seconds", std::to_string(seconds));
    res.set("Time-Arrival-Nanoseconds", std::to_string(nanoseconds));
    res.body() = generate_random_text(); // Generate random text of length 100
    res.prepare_payload();
}

// Function to handle connections
void do_session(tcp::socket& socket) {
    try {
        boost::beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(socket, buffer, req);
        
        http::response<http::string_body> res;
        handle_request(req, res);
        
        http::write(socket, res);
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

// Main function to set up the server
int main() {
    try {
        auto const address = boost::asio::ip::make_address("0.0.0.0");
        auto port = static_cast<unsigned short>(std::atoi("8080"));
        
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
