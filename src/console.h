#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace ui {

// Конец ввода (EOF).
struct InputClosed {};

void setupConsole();

std::string green(const std::string& text);
std::string red(const std::string& text);
std::string yellow(const std::string& text);
std::string bold(const std::string& text);

std::string readLine(const std::string& prompt);

// false, если это не целое или оно не помещается в 64 бита.
bool parseInt(const std::string& text, long long& value);

long long readInt(const std::string& prompt);
long long readIntInRange(const std::string& prompt, long long lo, long long hi);
std::vector<long long> readIntList(const std::string& prompt, std::size_t minCount);
std::string readNumberString(const std::string& prompt);

// "[15, 30, 45]"; при limit > 0 — "[1, 2, … ещё 98]".
std::string formatList(const std::vector<long long>& values, std::size_t limit = 0);

std::string padRight(const std::string& text, std::size_t width);

}  // namespace ui
