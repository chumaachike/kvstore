// #include <gtest/gtest.h>

// #include <stdexcept>
// #include <thread>
// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <unistd.h>

// #include "server.hpp"
// #include "store.hpp"
// #include "server_loop.hpp"

// namespace {
//     int connect_to_server(uint16_t port) {
//         int fd = socket(AF_INET, SOCK_STREAM, 0);
//         if (fd == -1) {
//             throw std::runtime_error("socket failed");
//         }

//         sockaddr_in addr{};
//         addr.sin_family = AF_INET;
//         addr.sin_port = htons(port);
//         addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

//         if (connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1) {
//             close(fd);
//             throw std::runtime_error("connect failed");
//         }

//         return fd;
//     }

//     void send_command(int fd, const std::string& command) {
//         std::string msg = command + "\n";
//         send(fd, msg.c_str(), msg.size(), 0);
//     }

//     std::string recv_response(int fd) {
//         std::string response;
//         char ch{};

//         while (true) {
//             ssize_t received = recv(fd, &ch, 1, 0);

//             if (received <= 0) {
//                 break;
//             }

//             response.push_back(ch);

//             if (ch == '\n') {
//                 break;
//             }
//         }

//         return response;
//     }
// }

// TEST(TCPIntegrationTest, SetAndGetWorksOverTCP) {
//     constexpr uint16_t port = 9090;

//     KVStore store;
//     TCPServer server;

//     server.start(port);

//     std::thread server_thread([&] {
//         int client_fd = server.accept_client();
//         handle_client(client_fd, store, server);
//     });

//     std::this_thread::sleep_for(std::chrono::milliseconds(100));

//     int client_fd = connect_to_server(port);

//     send_command(client_fd, "SET name Edward");
//     EXPECT_EQ(recv_response(client_fd), "OK\n");

//     send_command(client_fd, "GET name");
//     EXPECT_EQ(recv_response(client_fd), "Edward\n");

//     send_command(client_fd, "QUIT");
//     EXPECT_EQ(recv_response(client_fd), "Bye\n");

//     close(client_fd);

//     server_thread.join();
// }

// TEST(TCPIntegrationTest, UnknownCommandReturnsErrorOverTCP) {
//     constexpr uint16_t port = 9091;

//     KVStore store;
//     TCPServer server;

//     server.start(port);

//     std::thread server_thread([&] {
//         int accepted_fd = server.accept_client();
//         handle_client(accepted_fd, store, server);
//     });

//     std::this_thread::sleep_for(std::chrono::milliseconds(100));

//     int client_fd = connect_to_server(port);

//     send_command(client_fd, "BANANA");
//     EXPECT_EQ(
//         recv_response(client_fd),
//         "Error: unknown command 'BANANA'\n"
//     );

//     send_command(client_fd, "QUIT");
//     EXPECT_EQ(recv_response(client_fd), "Bye\n");

//     close(client_fd);
//     server_thread.join();
// }

// TEST(TCPIntegrationTest, QuitClosesClientConnection) {
//     constexpr uint16_t port = 9092;

//     KVStore store;
//     TCPServer server;

//     ASSERT_NO_THROW(server.start(port));

//     std::thread server_thread([&] {
//         int accepted_fd = server.accept_client();
//         handle_client(accepted_fd, store, server);
//     });

//     int client_fd = connect_to_server(port);

//     send_command(client_fd, "QUIT");

//     EXPECT_EQ(recv_response(client_fd), "Bye\n");

//     char buffer[16];
//     ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);

//     EXPECT_EQ(n, 0);

//     close(client_fd);

//     if (server_thread.joinable()) {
//         server_thread.join();
//     }
// }