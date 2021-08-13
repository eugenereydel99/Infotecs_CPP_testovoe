#include "Client.h"
#include <iostream>
#include <algorithm>
#include <cstring>
#include <unistd.h>

Client::Client() {

    pthread_cond_init(&cond, nullptr);
    pthread_mutex_init(&mutex, nullptr);
    sem_init(&semaphore, 0, 0);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error on create() socket." << std::endl;
        exit(-1);
    }

    addr.sin_family         = AF_INET;
    addr.sin_addr.s_addr    = inet_addr("127.0.0.1");
    addr.sin_port           = htons(8888);

    int res = connect(sockfd, (sockaddr*)&addr, sizeof(addr));
    if (res < 0) {
        std::cerr << "Error on connect()." << std::endl;
        exit(-1);
    }

}

Client::~Client() {

    remove("buffer.txt");
    close(sockfd);
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    sem_destroy(&semaphore);

}

void Client::inputString() {

    std::cout << "Type keyword 'exit' to terminate program." << std::endl;

    while (true) {
        std::cout << "Enter a sequence of digits (max length = 64) : ";
        std::cin >> input;

        auto checkDigits = std::find_if_not(input.begin(), input.end(), [](char element){
            return isdigit(element);
        });

        if ((checkDigits == input.end() && input.length() <= 64) || input == "exit") {
            break;
        } else {
            std::cout << "Data is incorrect - try again!\n" << std::endl;
        }
    }

}

void Client::replaceEven() {
    std::string elements;
    for (auto item : input) {
        if ((int(item) - '0') % 2 == 0) {
            elements += "KB";
        } else {
            elements += item;
        }
    }
    input = elements;
}

void Client::writeToBuffer() {
    buffer.open("buffer.txt", std::ios::out);
    if (buffer.is_open()) {
        for (auto item : input) {
            buffer << item;
        }
        buffer.close();
    }
}

void Client::thread1() {

    while (true)
    {
        /*
         * После первой итерации работы текущего потока необходимо
         * ожидать синхронизирующего сигнала от потока №1.
         * Изменить название resultFromThread2
         * */
        if (resultFromThread2 != -1) {
            input.clear();
            sem_wait(&semaphore);
        }

        inputString();

        if (input == "exit"){
            pthread_cond_signal(&cond);
            break;
        } else {
            std::sort(input.begin(),  input.end(), std::greater<char>());
            replaceEven();
            writeToBuffer();
        }

        //  Очищаем результат из потока №2 и передаём управление потоку №2.
        resultFromThread2 = 0;
        pthread_cond_signal(&cond);
    }
}

void *Client::thread1Wrapper(void *self) {
    static_cast<Client*>(self)->thread1();
    return nullptr;
}

std::string Client::readFromBuffer() {
    std::string data;
    buffer.open("buffer.txt", std::ios::in);
    if (buffer.is_open()) {
        while (!buffer.eof()) {
            getline(buffer, data);
        }
        buffer.close();
    }
    buffer.clear();

    std::cout << "Data from shared buffer: " << data << std::endl << std::endl;

    return data;
}

void Client::sumDigits(const std::string &data) {
    for (auto element : data) {
        if (isdigit(element)) {
            resultFromThread2 += int(element) - '0';
        }
    }
}

void Client::sendToServer() {

    memset(&recvline, 0, 128);
    memset(&sendline, 0, 128);

    strcpy(sendline, std::to_string(resultFromThread2).c_str());

    int res = send(sockfd, &sendline, sizeof(sendline), 0);
    if (res < 0) {
        std::cerr << "Error on send() message." << std::endl;
    }
}

void Client::thread2() {

    while (true)
    {
        /*
         * Блокируем мьютекс только на первой итерации.
         * */
        if (resultFromThread2 == -1) pthread_mutex_lock(&mutex);

        /*
         * Если строка, вводимая пользователем из потока №1 пуста,
         * значит необходимо передать управление потоку №1.
         * Здесь мы блокируем текущий поток, пока не получим
         * сигнал от потока №1 о завершении записи в общий буфер.
         * */
        if (input.empty()) {
            pthread_cond_wait(&cond, &mutex); // после освобождения текущего потока мьютекс снова блокируется
        }

        if (input == "exit") {
            std::cout << "Program has finished." << std::endl;
            break;
        }

        std::string result = readFromBuffer();

        sumDigits(result);

        sendToServer();

        // Передаём управление потоку №1
        sem_post(&semaphore);
    }
}

void *Client::thread2Wrapper(void *self) {
    static_cast<Client*>(self)->thread2();
    return nullptr;
}

void Client::run() {
    pthread_create(&thread[0], nullptr, Client::thread1Wrapper, this);
    pthread_create(&thread[1], nullptr, Client::thread2Wrapper, this);

    pthread_join(thread[0], nullptr);
    pthread_join(thread[1], nullptr);
}








