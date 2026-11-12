// Реализация признаков делимости из rules.h.
#include "rules.h"

#include <cctype>
#include <stdexcept>

namespace dv {

namespace {

int digitAt(const std::string& digits, std::size_t i) {
    return digits[i] - '0';
}

int lastDigit(const std::string& digits) {
    return digitAt(digits, digits.size() - 1);
}

long long digitSum(const std::string& digits) {
    long long sum = 0;
    for (char c : digits) sum += c - '0';
    return sum;
}

// Цифры справа налево со знаками +, -, +, ...
long long alternatingSum(const std::string& digits) {
    long long sum = 0;
    int sign = 1;
    for (std::size_t i = digits.size(); i-- > 0;) {
        sum += sign * digitAt(digits, i);
        sign = -sign;
    }
    return sum;
}

int tail(const std::string& digits, std::size_t count) {
    std::size_t from = digits.size() > count ? digits.size() - count : 0;
    return std::stoi(digits.substr(from));
}

std::string verdict(bool divisible, int divisor) {
    return std::string(divisible ? "делится на " : "не делится на ") + std::to_string(divisor);
}

}  // namespace

const std::vector<int>& ruleDivisors() {
    static const std::vector<int> list{2, 3, 4, 5, 6, 8, 9, 10, 11};
    return list;
}

bool isNumberString(const std::string& text) {
    std::size_t start = (!text.empty() && (text[0] == '-' || text[0] == '+')) ? 1 : 0;
    if (start == text.size()) return false;
    for (std::size_t i = start; i < text.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(text[i]))) return false;
    }
    return true;
}

std::string normalizeDigits(const std::string& text) {
    if (!isNumberString(text)) throw std::invalid_argument("ожидалось целое число");
    std::size_t start = (text[0] == '-' || text[0] == '+') ? 1 : 0;
    while (start + 1 < text.size() && text[start] == '0') ++start;
    return text.substr(start);
}

RuleResult checkRule(const std::string& number, int divisor) {
    const std::string digits = normalizeDigits(number);
    const std::string last = std::to_string(lastDigit(digits));

    switch (divisor) {
        case 2: {
            bool ok = lastDigit(digits) % 2 == 0;
            return {2, ok, "последняя цифра " + last + (ok ? " — чётная" : " — нечётная")};
        }
        case 3: {
            long long sum = digitSum(digits);
            bool ok = sum % 3 == 0;
            return {3, ok, "сумма цифр " + std::to_string(sum) + " " + verdict(ok, 3)};
        }
        case 4: {
            int t = tail(digits, 2);
            bool ok = t % 4 == 0;
            return {4, ok, "две последние цифры дают " + std::to_string(t) + ", оно " + verdict(ok, 4)};
        }
        case 5: {
            bool ok = lastDigit(digits) == 0 || lastDigit(digits) == 5;
            return {5, ok, "последняя цифра " + last + (ok ? "" : " — не 0 и не 5")};
        }
        case 6: {
            bool by2 = checkRule(digits, 2).divisible;
            bool by3 = checkRule(digits, 3).divisible;
            std::string why;
            if (by2 && by3) why = "делится и на 2, и на 3";
            else if (!by2 && !by3) why = "не делится ни на 2, ни на 3";
            else why = by2 ? "на 2 делится, но не делится на 3" : "на 3 делится, но не делится на 2";
            return {6, by2 && by3, why};
        }
        case 8: {
            int t = tail(digits, 3);
            bool ok = t % 8 == 0;
            return {8, ok, "три последние цифры дают " + std::to_string(t) + ", оно " + verdict(ok, 8)};
        }
        case 9: {
            long long sum = digitSum(digits);
            bool ok = sum % 9 == 0;
            return {9, ok, "сумма цифр " + std::to_string(sum) + " " + verdict(ok, 9)};
        }
        case 10: {
            bool ok = lastDigit(digits) == 0;
            return {10, ok, "последняя цифра " + last + (ok ? "" : " — не 0")};
        }
        case 11: {
            long long sum = alternatingSum(digits);
            bool ok = sum % 11 == 0;
            return {11, ok, "знакопеременная сумма цифр " + std::to_string(sum) + " " + verdict(ok, 11)};
        }
        default:
            throw std::invalid_argument("для делителя " + std::to_string(divisor) + " признак не реализован");
    }
}

std::vector<RuleResult> checkAllRules(const std::string& number) {
    std::vector<RuleResult> results;
    for (int d : ruleDivisors()) results.push_back(checkRule(number, d));
    return results;
}

int remainderOf(const std::string& number, int divisor) {
    if (divisor <= 0) throw std::invalid_argument("делитель должен быть положительным");
    long long r = 0;
    for (char c : normalizeDigits(number)) r = (r * 10 + (c - '0')) % divisor;
    return static_cast<int>(r);
}

}  // namespace dv
