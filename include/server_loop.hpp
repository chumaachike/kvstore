#pragma once

#include "server.hpp"
#include "store.hpp"

void handle_client(
    int client_fd,
    KVStore& store,
    TCPServer& server
);