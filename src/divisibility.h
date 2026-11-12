// Математическое ядро «Анализатора делимости».
// Здесь нет ввода-вывода: только вычисления, которые удобно проверять тестами.
// На некорректные аргументы функции бросают исключения из <stdexcept>.
#pragma once

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace dv {

using i64 = long long;

// Ограничения, чтобы случайный ввод не занял всю память.
constexpr std::size_t MAX_RESULTS = 200000;   // чисел в одной последовательности
constexpr i64 MAX_PRIME_SPAN = 2000000;       // длина диапазона при поиске простых

enum class Primality { Prime, Composite, Neither };

// Делится ли a на b без остатка. b == 0 -> std::invalid_argument.
bool isDivisible(i64 a, i64 b);

// Простое / составное / ни то ни другое (всё, что меньше 2).
Primality classify(i64 n);
bool isPrime(i64 n);

// Разложение |n| на простые множители: пары (простое, степень) по возрастанию.
// Для |n| == 1 результат пуст. n == 0 -> std::invalid_argument.
std::vector<std::pair<i64, int>> factorize(i64 n);

// Строка вида "360 = 2^3 × 3^2 × 5".
std::string factorizationToString(i64 n);

// Все положительные делители |n| по возрастанию. n == 0 -> std::invalid_argument.
std::vector<i64> divisors(i64 n);

// НОД и НОК. НОД(0, 0) = 0, НОК с нулём = 0.
// Если НОК не помещается в 64 бита -> std::overflow_error.
i64 gcd(i64 a, i64 b);
i64 lcm(i64 a, i64 b);
i64 gcd(const std::vector<i64>& numbers);
i64 lcm(const std::vector<i64>& numbers);

// Последовательности на отрезке [lo, hi] (границы можно передать в любом порядке).
// Слишком длинный результат -> std::length_error.
std::vector<i64> multiplesInRange(i64 lo, i64 hi, i64 k);
std::vector<i64> divisibleByAll(i64 lo, i64 hi, const std::vector<i64>& ks);
std::vector<i64> primesInRange(i64 lo, i64 hi);

}  // namespace dv
