#pragma once

#include <string>
#include <vector>

struct HistoryEntry {
    std::string time;
    std::string operation;
    std::string result;
};

class History {
public:
    void add(const std::string& operation, const std::string& result);
    void clear();
    bool empty() const { return entries_.empty(); }
    const std::vector<HistoryEntry>& entries() const { return entries_; }

    std::string toText() const;

    // false, если файл не удалось записать.
    bool exportTo(const std::string& path) const;

private:
    std::vector<HistoryEntry> entries_;
};
