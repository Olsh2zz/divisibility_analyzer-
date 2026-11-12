// Признаки делимости на 2, 3, 4, 5, 6, 8, 9, 10, 11.
// Проверка идёт по десятичной записи (строке цифр), как в школьных правилах,
// поэтому число может быть любой длины — хоть в сто знаков.
#pragma once

#include <string>
#include <vector>

namespace dv {

struct RuleResult {
    int divisor;              // на что проверяем
    bool divisible;           // делится ли
    std::string explanation;  // почему: «сумма цифр 21 делится на 3»
};

// Делители, для которых реализованы признаки.
const std::vector<int>& ruleDivisors();

// Целое число в десятичной записи: необязательный знак и хотя бы одна цифра.
bool isNumberString(const std::string& text);

// Убирает знак и ведущие нули: "-0042" -> "42". Некорректная строка -> std::invalid_argument.
std::string normalizeDigits(const std::string& text);

// Проверка одного признака / всех сразу. Неизвестный делитель или
// некорректная строка -> std::invalid_argument.
RuleResult checkRule(const std::string& number, int divisor);
std::vector<RuleResult> checkAllRules(const std::string& number);

// Остаток от деления числа-строки на небольшой делитель «в лоб» (схема Горнера).
// Нужен, чтобы сверять признаки с обычным делением.
int remainderOf(const std::string& number, int divisor);

}  // namespace dv
