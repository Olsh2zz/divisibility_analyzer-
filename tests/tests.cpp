// Тесты для divisibility, rules, console и history.
// Сторонних библиотек нет: программа завершается с кодом 0, если все проверки прошли.
#include <climits>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "console.h"
#include "divisibility.h"
#include "history.h"
#include "rules.h"

namespace {

int failures = 0;
int checks = 0;

#define CHECK(condition)                                                              \
    do {                                                                              \
        ++checks;                                                                     \
        if (!(condition)) {                                                           \
            ++failures;                                                               \
            std::cout << "ПРОВАЛ " << __FILE__ << ":" << __LINE__ << ": " #condition "\n"; \
        }                                                                             \
    } while (false)

#define CHECK_THROWS(expression, exception)      \
    do {                                         \
        ++checks;                                \
        bool thrown = false;                     \
        try {                                    \
            (void)(expression);                  \
        } catch (const exception&) {             \
            thrown = true;                       \
        }                                        \
        if (!thrown) {                           \
            ++failures;                          \
            std::cout << "ПРОВАЛ " << __FILE__ << ":" << __LINE__ << ": нет исключения в " #expression "\n"; \
        }                                        \
    } while (false)

using dv::i64;
using List = std::vector<i64>;

void testDivisibility() {
    CHECK(dv::isDivisible(42, 7));
    CHECK(!dv::isDivisible(43, 7));
    CHECK(dv::isDivisible(0, 5));
    CHECK(dv::isDivisible(-42, 7));
    CHECK(dv::isDivisible(42, -7));
    CHECK(dv::isDivisible(LLONG_MIN, -1));
    CHECK_THROWS(dv::isDivisible(42, 0), std::invalid_argument);
}

void testPrimes() {
    for (i64 p : {2, 3, 17, 97}) CHECK(dv::classify(p) == dv::Primality::Prime);
    for (i64 c : {4, 15, 100, 123456, 999999}) CHECK(dv::classify(c) == dv::Primality::Composite);
    for (i64 n : {0, 1, -1, -7}) CHECK(dv::classify(n) == dv::Primality::Neither);

    CHECK(dv::isPrime(999983));
    CHECK(dv::isPrime(1000000007));
    CHECK(dv::isPrime(9223372036854775783LL));    // наибольшее простое меньше 2^63
    CHECK(!dv::isPrime(LLONG_MAX));               // 7^2 · 73 · 127 · 337 · 92737 · 649657
    CHECK(!dv::isPrime(3215031751LL));            // сильное псевдопростое по основаниям 2, 3, 5, 7
    CHECK(!dv::isPrime(1000000007LL * 998244353LL));

    for (i64 n = 0; n < 5000; ++n) {
        bool naive = n >= 2;
        for (i64 d = 2; d * d <= n; ++d) {
            if (n % d == 0) naive = false;
        }
        CHECK(dv::isPrime(n) == naive);
    }
}

void testFactorize() {
    using F = std::vector<std::pair<i64, int>>;
    CHECK(dv::factorize(360) == (F{{2, 3}, {3, 2}, {5, 1}}));
    CHECK(dv::factorize(97) == (F{{97, 1}}));
    CHECK(dv::factorize(1).empty());
    CHECK(dv::factorize(-12) == (F{{2, 2}, {3, 1}}));
    CHECK(dv::factorize(123456) == (F{{2, 6}, {3, 1}, {643, 1}}));
    CHECK(dv::factorize(999999) == (F{{3, 3}, {7, 1}, {11, 1}, {13, 1}, {37, 1}}));
    CHECK(dv::factorize(LLONG_MAX) == (F{{7, 2}, {73, 1}, {127, 1}, {337, 1}, {92737, 1}, {649657, 1}}));
    CHECK(dv::factorize(1000000007LL * 998244353LL) == (F{{998244353, 1}, {1000000007, 1}}));
    CHECK_THROWS(dv::factorize(0), std::invalid_argument);

    CHECK(dv::factorizationToString(360) == "360 = 2^3 × 3^2 × 5");
    CHECK(dv::factorizationToString(-12) == "-12 = -1 × 2^2 × 3");
    CHECK(dv::factorizationToString(1) == "1 = 1");
    CHECK(dv::factorizationToString(97) == "97 = 97");

    for (i64 n = 1; n < 3000; ++n) {
        i64 product = 1;
        for (const auto& [p, e] : dv::factorize(n)) {
            CHECK(dv::isPrime(p));
            for (int i = 0; i < e; ++i) product *= p;
        }
        CHECK(product == n);
    }
}

void testDivisors() {
    CHECK(dv::divisors(1) == (List{1}));
    CHECK(dv::divisors(12) == (List{1, 2, 3, 4, 6, 12}));
    CHECK(dv::divisors(-12) == (List{1, 2, 3, 4, 6, 12}));
    CHECK(dv::divisors(97) == (List{1, 97}));
    CHECK(dv::divisors(100) == (List{1, 2, 4, 5, 10, 20, 25, 50, 100}));
    CHECK(dv::divisors(123456).size() == 28);
    CHECK(dv::divisors(999999).size() == 64);
    CHECK_THROWS(dv::divisors(0), std::invalid_argument);

    for (i64 n = 1; n < 500; ++n) {
        List naive;
        for (i64 d = 1; d <= n; ++d) {
            if (n % d == 0) naive.push_back(d);
        }
        CHECK(dv::divisors(n) == naive);
    }
}

void testGcdLcm() {
    CHECK(dv::gcd(12, 18) == 6);
    CHECK(dv::lcm(12, 18) == 36);
    CHECK(dv::gcd(-12, 18) == 6);
    CHECK(dv::lcm(-4, 6) == 12);
    CHECK(dv::gcd(0, 5) == 5);
    CHECK(dv::gcd(0, 0) == 0);
    CHECK(dv::lcm(0, 5) == 0);
    CHECK(dv::gcd(17, 97) == 1);
    CHECK(dv::gcd(List{12, 18, 30}) == 6);
    CHECK(dv::lcm(List{2, 3, 4, 5}) == 60);
    CHECK(dv::gcd(123456, 999999) == 3);
    CHECK_THROWS(dv::lcm(LLONG_MAX, LLONG_MAX - 1), std::overflow_error);
    CHECK_THROWS(dv::gcd(List{}), std::invalid_argument);
}

void testSequences() {
    CHECK(dv::divisibleByAll(1, 50, {3, 5}) == (List{15, 30, 45}));
    CHECK(dv::multiplesInRange(1, 20, 5) == (List{5, 10, 15, 20}));
    CHECK(dv::multiplesInRange(20, 1, 5) == (List{5, 10, 15, 20}));
    CHECK(dv::multiplesInRange(-10, 10, 5) == (List{-10, -5, 0, 5, 10}));
    CHECK(dv::multiplesInRange(-9, -1, 4) == (List{-8, -4}));
    CHECK(dv::multiplesInRange(1, 10, -3) == (List{3, 6, 9}));
    CHECK(dv::multiplesInRange(1, 4, 7).empty());
    CHECK(dv::multiplesInRange(LLONG_MAX - 5, LLONG_MAX, 4) == (List{LLONG_MAX - 3}));
    CHECK(dv::divisibleByAll(1, 100, {4, 6}) == (List{12, 24, 36, 48, 60, 72, 84, 96}));
    CHECK(dv::divisibleByAll(-5, 5, {LLONG_MAX, LLONG_MAX - 1}) == (List{0}));
    CHECK_THROWS(dv::multiplesInRange(1, 10, 0), std::invalid_argument);
    CHECK_THROWS(dv::divisibleByAll(1, 10, {3, 0}), std::invalid_argument);
    CHECK_THROWS(dv::multiplesInRange(1, 100000000, 1), std::length_error);

    CHECK(dv::primesInRange(1, 30) == (List{2, 3, 5, 7, 11, 13, 17, 19, 23, 29}));
    CHECK(dv::primesInRange(-10, 1).empty());
    CHECK(dv::primesInRange(90, 100) == (List{97}));
    CHECK(dv::primesInRange(1, 100).size() == 25);
    CHECK(dv::primesInRange(1, 1000000).size() == 78498);
    CHECK(dv::primesInRange(1000000000000LL, 1000000000100LL) ==
          (List{1000000000039LL, 1000000000061LL, 1000000000063LL, 1000000000091LL}));
    CHECK(dv::primesInRange(9223372036854775700LL, LLONG_MAX) == (List{9223372036854775783LL}));
    CHECK_THROWS(dv::primesInRange(1, 100000000), std::length_error);

    for (i64 x : dv::primesInRange(999000, 1001000)) CHECK(dv::isPrime(x));
}

void testRules() {
    for (int n = 0; n <= 30000; ++n) {
        std::string text = std::to_string(n);
        for (const dv::RuleResult& r : dv::checkAllRules(text)) {
            CHECK(r.divisible == (n % r.divisor == 0));
        }
    }

    const std::vector<std::string> big = {
        "123456789012345678901234567890",
        "999999999999999999999999999999999999",
        "100000000000000000000000000000000000000000000000000",
        "121212121212121212121212121212121212121212",
        "98765432109876543210987654321098765432109876543210",
        "39916800000000000000000000000000000000",
    };
    for (const std::string& text : big) {
        for (const dv::RuleResult& r : dv::checkAllRules(text)) {
            CHECK(r.divisible == (dv::remainderOf(text, r.divisor) == 0));
        }
    }

    CHECK(dv::checkRule("123456", 3).explanation == "сумма цифр 21 делится на 3");
    CHECK(dv::checkRule("121", 11).divisible);
    CHECK(dv::checkRule("-42", 6).divisible);
    CHECK(dv::checkRule("0", 11).divisible);
    CHECK(dv::normalizeDigits("-0042") == "42");
    CHECK(dv::normalizeDigits("000") == "0");
    CHECK(dv::isNumberString("+15"));
    CHECK(!dv::isNumberString(""));
    CHECK(!dv::isNumberString("-"));
    CHECK(!dv::isNumberString("12a"));
    CHECK(!dv::isNumberString("1.5"));
    CHECK_THROWS(dv::checkRule("abc", 2), std::invalid_argument);
    CHECK_THROWS(dv::checkRule("12", 7), std::invalid_argument);
}

void testConsoleHelpers() {
    long long value = 0;
    CHECK(ui::parseInt("42", value) && value == 42);
    CHECK(ui::parseInt("-7", value) && value == -7);
    CHECK(ui::parseInt("+0", value) && value == 0);
    CHECK(ui::parseInt("9223372036854775807", value) && value == LLONG_MAX);
    CHECK(!ui::parseInt("9223372036854775808", value));
    CHECK(!ui::parseInt("-9223372036854775808", value));
    CHECK(!ui::parseInt("", value));
    CHECK(!ui::parseInt("abc", value));
    CHECK(!ui::parseInt("12 3", value));
    CHECK(!ui::parseInt("1e5", value));

    CHECK(ui::formatList({15, 30, 45}) == "[15, 30, 45]");
    CHECK(ui::formatList({}) == "[]");
    CHECK(ui::formatList({1, 2, 3, 4, 5}, 2) == "[1, 2, … ещё 3]");
    CHECK(ui::padRight("да", 5) == "да   ");
    CHECK(ui::padRight("длинное", 3) == "длинное");
}

void testHistory() {
    History h;
    CHECK(h.empty());
    h.add("Проверка делимости: 42 на 7", "42 делится на 7 (42 ÷ 7 = 6)");
    h.add("Таблица", "строка 1\nстрока 2");
    CHECK(h.entries().size() == 2);
    std::string text = h.toText();
    CHECK(text.find("1. [") == 0);
    CHECK(text.find("   42 делится на 7 (42 ÷ 7 = 6)\n") != std::string::npos);
    CHECK(text.find("   строка 2\n") != std::string::npos);

    const std::string path = "history_test_output.txt";
    CHECK(h.exportTo(path));
    std::remove(path.c_str());
    CHECK(!h.exportTo("no_such_directory/file.txt"));

    h.clear();
    CHECK(h.empty());
}

}  // namespace

int main() {
    testDivisibility();
    testPrimes();
    testFactorize();
    testDivisors();
    testGcdLcm();
    testSequences();
    testRules();
    testConsoleHelpers();
    testHistory();

    std::cout << "Проверок: " << checks << ", провалов: " << failures << "\n";
    return failures == 0 ? 0 : 1;
}
