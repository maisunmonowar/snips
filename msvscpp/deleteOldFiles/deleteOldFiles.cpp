#include <iostream>
#include <filesystem>
#include <chrono>
#include <regex>

namespace fs = std::filesystem;

bool isLogFileOlderThan(const fs::directory_entry& entry, int days) {
    std::regex datePattern(R"(\d{4}-\d{2}-\d{2})");
    std::smatch match;
    std::string filename = entry.path().filename().string();

    if (std::regex_search(filename, match, datePattern)) {
        std::tm tm = {};
        std::istringstream ss(match.str());
        ss >> std::get_time(&tm, "%Y-%m-%d");
        auto fileTime = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        auto currentTime = std::chrono::system_clock::now();
        auto age = std::chrono::duration_cast<std::chrono::hours>(currentTime - fileTime).count();
        return age > days * 24;
    }
    return false; // If the date is not found in the filename, consider it not old
}

void deleteOldLogFiles(const std::string& folderPath, int days) {
    std::regex logFilePattern(R"(foo_bar_id_\d{4}-\d{2}-\d{2}\.log)");

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        std::cout << "Checking file: " << entry.path().filename().string() << std::endl;
        if (entry.is_regular_file() && std::regex_match(entry.path().filename().string(), logFilePattern)) {
            if (isLogFileOlderThan(entry, days)) {
                std::cout << "Deleting: " << entry.path() << std::endl;
                fs::remove(entry);
            }
        }
    }
}

int main() {
    std::string folderPath = "path\\to\\file";
    int days = 7; // You can change this value or read it from a config file or other sources
    deleteOldLogFiles(folderPath, days);
    return 0;
}