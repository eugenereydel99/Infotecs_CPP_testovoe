#ifndef TASK_CLIENT_H
#define TASK_CLIENT_H

#include <vector>
#include <semaphore.h>
#include <arpa/inet.h>
#include <string>
#include <fstream>

class Client {
private:

    std::fstream buffer;

    std::string input;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    sem_t semaphore;

    pthread_t thread[2];

    int resultFromThread2 = -1; // сумма чисел

    int                 sockfd, n;
    struct sockaddr_in  addr;
    char                recvline[128];
    char                sendline[128];

    void thread1();
    static void *thread1Wrapper(void *);

    void thread2();
    static void *thread2Wrapper(void *);

    void inputString();

    void replaceEven();

    void writeToBuffer();

    std::string readFromBuffer();

    void sumDigits(const std::string &data);

    void sendToServer();

public:
    Client();

    ~Client();

    void run();

};

#endif //TASK_CLIENT_H
