#include <iostream>
#include <filesystem>
#include <chrono>
#include <regex>

namespace fs = std::filesystem;

bool isLogFileOlderThan(const fs::directory_entry& entry, int days) {
    auto lastWriteTime = fs::last_write_time(entry);
    auto currentTime = std::chrono::system_clock::now();
    auto fileTime = std::chrono::file_clock::to_sys(lastWriteTime);
    auto age = std::chrono::duration_cast<std::chrono::hours>(currentTime - fileTime).count();
    return age > days * 24;
}

void deleteOldLogFiles(const std::string& folderPath, int days) {
    std::regex logFilePattern(R"(foo_bar_id_\d{4}-\d{2}-\d{2}\.log)");

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file() && std::regex_match(entry.path().filename().string(), logFilePattern)) {
            if (isLogFileOlderThan(entry, days)) {
                std::cout << "Deleting: " << entry.path() << std::endl;
                fs::remove(entry);
            }
        }
    }
}

int main() {
    std::string folderPath = "path/to/your/folder";
    int days = 7; // You can change this value or read it from a config file or other sources
    deleteOldLogFiles(folderPath, days);
    return 0;
}