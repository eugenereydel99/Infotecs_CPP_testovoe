#include "Server.h"
#include <iostream>
#include <algorithm>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

Server::Server() {

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        std::cerr << "Error on create() socket" << std::endl;
        exit(-1);
    }

    servaddr.sin_family         = AF_INET;
    servaddr.sin_addr.s_addr    = inet_addr("127.0.0.1");
    servaddr.sin_port           = htons(8888);

    int res = bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));
    if (res < 0){
        std::cerr << "Error on bind() socket" << std::endl;
        exit(-1);
    }

    res = listen(listenfd, 1);
    if (res < 0){
        std::cerr << "Error on listen()." << std::endl;
        exit(-1);
    }

}

Server::~Server() {

    close(connfd);
    close(listenfd);

}

void Server::checkMessage(const std::string &msg) {

    std::cout << "Data from program №1: " << msg << std::endl;

    if (msg.length() > 2 && std::stoi(msg) % 32 == 0) {
        std::cout << "Status: data is correct.\n\n";
    } else {
        std::cerr << "Error: data is incorrect.\n\n";
    }

}

int Server::onReceiveMessage() {
    memset(recvline, 0, 128);
    memset(sendline, 0, 128);

    std::cout << "Status: waiting for data..." << std::endl;

    int res = recv(connfd, recvline, 128, 0);
    if (res > 0) {
        checkMessage(recvline);
    }

    return res;
}

void Server::run() {

    connfd = accept(listenfd, nullptr, nullptr);
    if (connfd < 0)
    {
        std::cerr << "Error on accept()." << std::endl;
        exit(-1);
    }

    while(true) {

        int checkResult = onReceiveMessage();

        if (checkResult <= 0 || (strcmp(recvline, "exit") == 0)) {
            std::cout << "Program №1 has finished." << std::endl;
            break;
        }

    }

}


