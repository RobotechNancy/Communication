/*!
 * @file logs.h
 * @version 1.0
 * @date 2022-2023
 * @author Julien PISTRE
 * @brief Fichier d'en-tête de la classe Logger
 * @details Version modifiée de la librairie de Théo RUSINOWITCH (v4.1a)
 */

#ifndef LOGS_H
#define LOGS_H

#include <fstream>
#include <iomanip>
#include <sstream>
#include <utility>
#include <iostream>


enum Log {
    NONE = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4
};


/*!
 * @brief Classe qui permet de logger des informations dans un fichier et dans la console
 */
class Logger {
private:
    std::string name;
    std::ofstream of;
    std::stringstream ss;
    Log level{Log::INFO};
public:
    explicit Logger(std::string loggerName, const std::string& logFilePath = "logs.log"):
            name(std::move(loggerName)), of(logFilePath, std::ios::app) {};

    Logger& operator()(Log value) {
        level = value;
        return *this;
    }

    template<typename T>
    Logger& operator<<(const T& t) {
        ss << t;
        return *this;
    }

    Logger& operator<<(std::ostream& (*f)(std::ostream&)) {
        f(ss);
        std::stringstream log;

        time_t now = time(nullptr);
        tm *ltm = localtime(&now);

        switch (level) {
            case Log::NONE:
                break;
            case Log::INFO:
                log << "[INFO]";
                break;
            case Log::WARNING:
                log << "[WARN]";
                break;
            case Log::ERROR:
                log << "[ERR]";
                break;
            case Log::CRITICAL:
                log << "[CRIT]";
                break;
        }

        log << "[" << std::setfill('0') << std::setw(2) << ltm->tm_hour << ":"
            << std::setfill('0') << std::setw(2) << ltm->tm_min << ":"
            << std::setfill('0') << std::setw(2) << ltm->tm_sec << "]"
            << "[" << name << "] ";

        log << ss.str();
        ss.str("");

        of << log.str();
        std::cout << log.str();

        return *this;
    }
};

#endif //LOGS_H
