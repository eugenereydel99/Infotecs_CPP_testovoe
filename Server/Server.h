#ifndef TASK_SERVER_H
#define TASK_SERVER_H

#include <netinet/in.h>
#include <string>
#include <set>

class Server {

private:
    int                 listenfd, connfd, n;
    struct sockaddr_in  servaddr;
    char                recvline[128];
    char                sendline[128];
    std::set<int> clients;

    void checkMessage(const std::string &msg);

public:

    Server();

    ~Server();

    int onReceiveMessage();

    void run();
};

#endif //TASK_SERVER_H
