#include "Client.h"

Client::Client() {
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
    remove("shared_buffer.txt");
    close(sockfd);
}

void Client::inputString() {
    std::cout << "Type keyword 'exit' to terminate program." << std::endl;
    std::cout << "Enter a sequence of digits (max length = 64) : ";
    std::cin >> input;
}

void Client::checkString() {
    while (true) {
        inputString();

        auto checkDigits = std::find_if_not(input.begin(), input.end(), [](char element) {
            return isdigit(element);
        });

        if (checkDigits == input.end() && input.length() <= 64) {
            break;
        } else if (input == "exit") {
            std::cout << "Program has finished." << std::endl;
            exit(0);
        }

        std::cout << "Data is incorrect - try again!\n" << std::endl;
    }
}

void Client::sortByDesc() {
    std::sort(input.begin(), input.end(), std::greater<char>());
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
    buffer.open("shared_buffer.txt", std::ios::out);
    if (buffer.is_open()) {
        for (auto item : input) {
            buffer << item;
        }
        buffer.close();
    }
}

std::string Client::readFromBuffer() {
    std::string data;
    buffer.open("shared_buffer.txt", std::ios::in);
    if (buffer.is_open()) {
        while (!buffer.eof()) {
            std::getline(buffer, data);
        }
        buffer.close();
    }
    buffer.clear();

    std::cout << "Data from shared buffer: " << data << std::endl << std::endl;

    return data;
}

void Client::sumDigits() {
    std::string data = readFromBuffer();

    result = 0;
    for (auto element : data) {
        if (isdigit(element)) {
            result += int(element) - '0';
        }
    }
}

void Client::sendToServer() {
    memset(&sendline, 0, sizeof(sendline));

    strcpy(sendline, std::to_string(result).c_str());

    int n = send(sockfd, &sendline, sizeof(sendline), 0);
    if (n < 0) {
        std::cerr << "Error on send() message." << std::endl;
    }
}

void Client::thread1() {
    while (true) {
        checkString();

        sortByDesc();

        replaceEven();

        writeToBuffer();

        cond_v.notify_one();

        std::unique_lock<std::mutex> ul(mtx, std::defer_lock);
        cond_v.wait(ul);

    }
}

void *Client::thread1Wrapper(void *self) {
    static_cast<Client*>(self)->thread1();
    return nullptr;
}

void Client::thread2() {
    while (true) {
        std::unique_lock<std::mutex> ul(mtx, std::defer_lock);
        cond_v.wait(ul);

        sumDigits();

        sendToServer();

        cond_v.notify_one();
    }
}

void *Client::thread2Wrapper(void *self) {
    static_cast<Client*>(self)->thread2();
    return nullptr;
}

void Client::run() {
    std::thread th1(thread1Wrapper, this);
    std::thread th2(thread2Wrapper, this);

    th1.join();
    th2.join();
}