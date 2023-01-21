#ifndef LOGS_H
#define LOGS_H

#include <fstream>
#include <iomanip>
#include <sstream>
#include <utility>
#include <iostream>

struct Mendl{
} const mendl;

class Logger {
private:
    std::string name;
    std::ofstream of;
    std::stringstream ss;
public:
    explicit Logger(std::string  logger_name, const std::string& logger_path = "../logs.log");

    friend Logger& operator<<(Logger& logger, const Mendl& data) {
        logger.append();
        return logger;
    }

    template<typename T>
    friend Logger& operator<<(Logger& logger, const T &data) {
        logger.ss << data;
        return logger;
    };

    void append();
};

#endif //LOGS_H
