#ifndef TASK_SERVER_H
#define TASK_SERVER_H

#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

class Server {

private:

    /* Файловый дескриптор серверного сокета */
    int listenfd;

    /* Файловый дескриптор клиентского сокета */
    int                 connfd;

    /* Структура адреса серверного сокета */
    struct sockaddr_in  servaddr;

    /*
     * Массив элементов для получения данных от программы №1.
     * Размер массива определяется из условия, что
     * все введенные пользователем числа будут четными.
     * */
    char recvline[128];

    /*Ожидание получения строки от программы №1*/
    int onReceiveMessage();

    /*
     * Анализ полученной от программы №1 строки.
     * Если она больше 2-ух символов и кратная 32 - выводится сообщение о полученных данных,
     * иначе - выводится сообщение об ошибке.
     * */
    void checkMessage(const std::string &msg);

public:

    Server();

    ~Server();

    void run();
};

#endif //TASK_SERVER_H
