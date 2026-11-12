// Математика анализатора: делимость, простота, делители, НОД и НОК,
// разложение на простые множители, последовательности чисел в диапазоне.
// Функции ничего не печатают; при некорректных аргументах бросают исключения.
#pragma once

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace dv {

using i64 = long long;

constexpr std::size_t MAX_RESULTS = 200000;
constexpr i64 MAX_PRIME_SPAN = 2000000;

enum class Primality { Prime, Composite, Neither };

// b == 0 -> std::invalid_argument.
bool isDivisible(i64 a, i64 b);

Primality classify(i64 n);
bool isPrime(i64 n);

// Пары (простое, степень) для |n| по возрастанию.
// Для |n| == 1 пусто. n == 0 -> std::invalid_argument.
std::vector<std::pair<i64, int>> factorize(i64 n);

std::string factorizationToString(i64 n);

// Положительные делители |n| по возрастанию. n == 0 -> std::invalid_argument.
std::vector<i64> divisors(i64 n);

// НОД(0, 0) = 0, НОК с нулём = 0.
// Переполнение НОК -> std::overflow_error.
i64 gcd(i64 a, i64 b);
i64 lcm(i64 a, i64 b);
i64 gcd(const std::vector<i64>& numbers);
i64 lcm(const std::vector<i64>& numbers);

// Отрезок [lo, hi], порядок границ любой.
// Превышение лимитов выше -> std::length_error.
std::vector<i64> multiplesInRange(i64 lo, i64 hi, i64 k);
std::vector<i64> divisibleByAll(i64 lo, i64 hi, const std::vector<i64>& ks);
std::vector<i64> primesInRange(i64 lo, i64 hi);

}  // namespace dv
