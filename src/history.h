// История операций за сеанс и её экспорт в текстовый файл.
#pragma once

#include <string>
#include <vector>

struct HistoryEntry {
    std::string time;       // чч:мм:сс
    std::string operation;  // что спрашивали
    std::string result;     // что получилось (может быть многострочным)
};

class History {
public:
    void add(const std::string& operation, const std::string& result);
    void clear();
    bool empty() const { return entries_.empty(); }
    const std::vector<HistoryEntry>& entries() const { return entries_; }

    // Вся история одним текстом (без цветовых кодов).
    std::string toText() const;

    // Записывает историю в файл. Возвращает false, если файл не удалось открыть или записать.
    bool exportTo(const std::string& path) const;

private:
    std::vector<HistoryEntry> entries_;
};
