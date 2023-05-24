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


/*!
 * @brief Classe qui permet de logger des informations dans un fichier et dans la console
 */
class Logger {
private:
    std::string name;
    std::ofstream of;
    std::stringstream ss;
    std::string level;
public:
    explicit Logger(std::string loggerName, const std::string& logFilePath = "logs.log"):
            name(std::move(loggerName)), of(logFilePath, std::ios::app) {};

    Logger& operator()(const std::string& value) {
        this->level = value;
        return *this;
    }

    template<typename T>
    Logger& operator<<(const T& t) {
        ss << t;
        return *this;
    }

    Logger& operator<<(std::ostream& (*f)(std::ostream&)) {
        f(ss);

        time_t now = time(nullptr);
        tm *ltm = localtime(&now);

        std::stringstream log;
        log <<  "["
            << std::setw(2) << std::setfill('0') << ltm->tm_hour << ":"
            << std::setw(2) << std::setfill('0') << ltm->tm_min << ":"
            << std::setw(2) << std::setfill('0') << ltm->tm_sec << "-"<< name
            << "] " << ss.str();

        of << log.str();
        std::cout << log.str();
        ss.str("");

        return *this;
    }
};

#endif //LOGS_H
