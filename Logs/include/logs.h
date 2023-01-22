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
 * @brief Constante qui remplace std::endl
 */
struct Mendl {
} const mendl;

/*!
 * @class Logger
 * @brief Classe qui permet de logger des informations dans un fichier et dans la console
 * @example Logger logger("sys");\n
 *          logger \<\< "Hello" \<\< mendl;
 */
class Logger {
private:
    std::string name;
    std::ofstream of;
    std::stringstream ss;

    void logStream();
public:
    explicit Logger(std::string  logger_name, const std::string& logger_path = "../logs.log");

    friend Logger& operator<<(Logger& logger, const Mendl& _) {
        logger.logStream();
        return logger;
    }

    template<typename T>
    friend Logger& operator<<(Logger& logger, const T &data) {
        logger.ss << data;
        return logger;
    };
};

#endif //LOGS_H
