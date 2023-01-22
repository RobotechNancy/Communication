/*!
 * @file logs.cpp
 * @version 1.0
 * @date 2022-2023
 * @author Julien PISTRE
 * @brief Fichier source de la classe Logger
 * @details Version modifiée de la librairie de Théo RUSINOWITCH (v4.1a)
 */

#include "../include/logs.h"
using namespace std;


/*!
 * @brief <br>Constructeur de la classe Logger
 * @param logger_name Nom du logger
 * @param logger_path Chemin du fichier de log
 */
Logger::Logger(string  logger_name, const string& logger_path):
    name(std::move(logger_name)),
    of(logger_path, ios::app) {}


void Logger::logStream() {
    time_t now = time(0);
    tm *ltm = localtime(&now);

    stringstream log;
    log <<  "["
                << setw(2) << setfill('0') << ltm->tm_hour << ":"
                << setw(2) << setfill('0') << ltm->tm_min << ":"
                << setw(2) << setfill('0') << ltm->tm_sec << "-"<< name
        << "] " << ss.str() << endl;

    of << log.str();
    cout << log.str();
    ss.str("");
}
