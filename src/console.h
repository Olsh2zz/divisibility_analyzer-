// Работа с консолью: безопасный ввод, цвета, форматирование списков и таблиц.
#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace ui {

// Бросается, когда ввод закончился (Ctrl+D / Ctrl+Z или конец файла при перенаправлении).
struct InputClosed {};

// Настраивает консоль: UTF-8 и цвета (на Windows это нужно включать явно).
void setupConsole();

// Цветной текст. Если вывод идёт не в терминал, строки возвращаются без изменений.
std::string green(const std::string& text);
std::string red(const std::string& text);
std::string yellow(const std::string& text);
std::string bold(const std::string& text);

// Читает строку без пробелов по краям. Конец ввода -> InputClosed.
std::string readLine(const std::string& prompt);

// Разбирает целое число. false, если это не число или оно не помещается в 64 бита.
bool parseInt(const std::string& text, long long& value);

// Спрашивают, пока не получат корректный ответ.
long long readInt(const std::string& prompt);
long long readIntInRange(const std::string& prompt, long long lo, long long hi);
std::vector<long long> readIntList(const std::string& prompt, std::size_t minCount);
std::string readNumberString(const std::string& prompt);  // целое любой длины

// "[15, 30, 45]". При limit > 0 показываются только первые limit чисел: "[1, 2, … ещё 98]".
std::string formatList(const std::vector<long long>& values, std::size_t limit = 0);

// Дополняет строку пробелами до ширины width; кириллица считается по символам, а не по байтам.
std::string padRight(const std::string& text, std::size_t width);

}  // namespace ui
