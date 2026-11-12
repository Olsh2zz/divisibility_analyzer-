// Признаки делимости на 2, 3, 4, 5, 6, 8, 9, 10, 11.
// Число передаётся строкой цифр, поэтому его длина не ограничена.
#pragma once

#include <string>
#include <vector>

namespace dv {

struct RuleResult {
    int divisor;
    bool divisible;
    std::string explanation;
};

const std::vector<int>& ruleDivisors();

bool isNumberString(const std::string& text);

// "-0042" -> "42". Не число -> std::invalid_argument.
std::string normalizeDigits(const std::string& text);

// Неизвестный делитель или не число -> std::invalid_argument.
RuleResult checkRule(const std::string& number, int divisor);
std::vector<RuleResult> checkAllRules(const std::string& number);

// Остаток от деления числа-строки на divisor (схема Горнера).
int remainderOf(const std::string& number, int divisor);

}  // namespace dv
