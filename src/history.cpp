#include "history.h"

#include <ctime>
#include <fstream>
#include <sstream>

namespace {

std::string now(const char* format) {
    std::time_t t = std::time(nullptr);
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), format, std::localtime(&t));
    return buffer;
}

}  // namespace

void History::add(const std::string& operation, const std::string& result) {
    entries_.push_back({now("%H:%M:%S"), operation, result});
}

void History::clear() {
    entries_.clear();
}

std::string History::toText() const {
    std::ostringstream out;
    for (std::size_t i = 0; i < entries_.size(); ++i) {
        const HistoryEntry& e = entries_[i];
        out << (i + 1) << ". [" << e.time << "] " << e.operation << "\n";
        // Результат печатается с отступом, построчно.
        std::istringstream lines(e.result);
        std::string line;
        while (std::getline(lines, line)) out << "   " << line << "\n";
    }
    return out.str();
}

bool History::exportTo(const std::string& path) const {
    std::ofstream file(path);
    if (!file) return false;
    file << "Анализатор делимости — история операций\n";
    file << "Сохранено: " << now("%Y-%m-%d %H:%M:%S") << "\n\n";
    file << toText();
    file.flush();
    return static_cast<bool>(file);
}
