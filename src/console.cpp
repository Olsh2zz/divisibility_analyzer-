// Реализация функций из console.h, включая настройку консоли Windows (UTF-8 и цвета).
#include "console.h"

#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "rules.h"

#ifdef _WIN32
#define NOMINMAX
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace ui {

namespace {

bool colorsEnabled = false;

std::string paint(const char* code, const std::string& text) {
    if (!colorsEnabled) return text;
    return std::string("\033[") + code + "m" + text + "\033[0m";
}

std::string trim(const std::string& text) {
    const char* spaces = " \t\r\n";
    std::size_t begin = text.find_first_not_of(spaces);
    if (begin == std::string::npos) return "";
    std::size_t end = text.find_last_not_of(spaces);
    return text.substr(begin, end - begin + 1);
}

std::size_t utf8Length(const std::string& text) {
    std::size_t length = 0;
    for (unsigned char c : text) {
        if ((c & 0xC0) != 0x80) ++length;  // байты-продолжения не считаются
    }
    return length;
}

void complain(const std::string& message) {
    std::cout << red("Ошибка: " + message) << "\n";
}

}  // namespace

void setupConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    bool terminal = _isatty(_fileno(stdout)) != 0;
    if (terminal) {
        HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        // Без этого режима консоль Windows печатает ANSI-коды как текст.
        terminal = GetConsoleMode(out, &mode) &&
                   SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#else
    bool terminal = isatty(STDOUT_FILENO) != 0;
#endif
    colorsEnabled = terminal && std::getenv("NO_COLOR") == nullptr;
}

std::string green(const std::string& text) { return paint("32", text); }
std::string red(const std::string& text) { return paint("31", text); }
std::string yellow(const std::string& text) { return paint("33", text); }
std::string bold(const std::string& text) { return paint("1", text); }

std::string readLine(const std::string& prompt) {
    std::cout << prompt << std::flush;
    std::string line;
    if (!std::getline(std::cin, line)) throw InputClosed{};
    return trim(line);
}

bool parseInt(const std::string& text, long long& value) {
    if (!dv::isNumberString(text)) return false;
    errno = 0;
    char* end = nullptr;
    long long parsed = std::strtoll(text.c_str(), &end, 10);
    // LLONG_MIN не принимаем: его модуль не помещается в long long.
    if (errno == ERANGE || *end != '\0' || parsed == LLONG_MIN) return false;
    value = parsed;
    return true;
}

long long readInt(const std::string& prompt) {
    while (true) {
        std::string text = readLine(prompt);
        long long value = 0;
        if (parseInt(text, value)) return value;
        if (text.empty()) complain("пустой ввод, нужно целое число");
        else if (dv::isNumberString(text)) complain("число слишком большое, допустимо до ±9223372036854775807");
        else complain("«" + text + "» — не целое число");
    }
}

long long readIntInRange(const std::string& prompt, long long lo, long long hi) {
    while (true) {
        long long value = readInt(prompt);
        if (value >= lo && value <= hi) return value;
        complain("нужно число от " + std::to_string(lo) + " до " + std::to_string(hi));
    }
}

std::vector<long long> readIntList(const std::string& prompt, std::size_t minCount) {
    while (true) {
        std::string text = readLine(prompt);
        for (char& c : text) {
            if (c == ',' || c == ';') c = ' ';
        }

        std::vector<long long> values;
        std::string badToken;
        std::size_t pos = 0;
        while (pos < text.size() && badToken.empty()) {
            std::size_t begin = text.find_first_not_of(" \t", pos);
            if (begin == std::string::npos) break;
            std::size_t end = text.find_first_of(" \t", begin);
            if (end == std::string::npos) end = text.size();
            std::string token = text.substr(begin, end - begin);
            long long value = 0;
            if (parseInt(token, value)) values.push_back(value);
            else badToken = token;
            pos = end;
        }

        if (!badToken.empty()) complain("«" + badToken + "» — не целое число (или оно слишком большое)");
        else if (values.size() < minCount) complain("нужно ввести хотя бы " + std::to_string(minCount) + " числа(о) через пробел");
        else return values;
    }
}

std::string readNumberString(const std::string& prompt) {
    while (true) {
        std::string text = readLine(prompt);
        if (dv::isNumberString(text)) return text;
        if (text.empty()) complain("пустой ввод, нужно целое число");
        else complain("«" + text + "» — не целое число");
    }
}

std::string formatList(const std::vector<long long>& values, std::size_t limit) {
    std::size_t shown = (limit > 0 && values.size() > limit) ? limit : values.size();
    std::string text = "[";
    for (std::size_t i = 0; i < shown; ++i) {
        if (i > 0) text += ", ";
        text += std::to_string(values[i]);
    }
    if (shown < values.size()) text += ", … ещё " + std::to_string(values.size() - shown);
    return text + "]";
}

std::string padRight(const std::string& text, std::size_t width) {
    std::size_t length = utf8Length(text);
    return length >= width ? text : text + std::string(width - length, ' ');
}

}  // namespace ui
