#include "../include/logs.h"
using namespace std;

Logger::Logger(string  logger_name, const string& logger_path):
    name(std::move(logger_name)),
    of(logger_path, ios::app) {}

void Logger::append() {
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
