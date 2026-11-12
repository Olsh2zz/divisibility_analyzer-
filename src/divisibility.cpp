#include "divisibility.h"

#include <algorithm>
#include <climits>
#include <cmath>
#include <stdexcept>

namespace dv {

namespace {

using u64 = unsigned long long;

// Модуль числа без переполнения на LLONG_MIN.
u64 uabs(i64 x) {
    return x < 0 ? 0ULL - static_cast<u64>(x) : static_cast<u64>(x);
}

// (a * b) mod m без переполнения. Требуется m < 2^63.
u64 mulmod(u64 a, u64 b, u64 m) {
#ifdef __SIZEOF_INT128__
    return static_cast<u64>(static_cast<unsigned __int128>(a) * b % m);
#else
    // Запасной вариант для компиляторов без 128-битных чисел: умножение сложениями.
    u64 result = 0;
    a %= m;
    b %= m;
    while (b > 0) {
        if (b & 1) {
            result += a;
            if (result >= m) result -= m;
        }
        a += a;
        if (a >= m) a -= m;
        b >>= 1;
    }
    return result;
#endif
}

u64 powmod(u64 base, u64 exp, u64 m) {
    u64 result = 1;
    base %= m;
    while (exp > 0) {
        if (exp & 1) result = mulmod(result, base, m);
        base = mulmod(base, base, m);
        exp >>= 1;
    }
    return result;
}

// Тест Миллера — Рабина. С этим набором оснований он точен для всех 64-битных чисел,
// поэтому большие числа проверяются мгновенно, без перебора до корня.
bool isPrimeU(u64 n) {
    if (n < 2) return false;
    static const u64 bases[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
    for (u64 p : bases) {
        if (n % p == 0) return n == p;
    }
    u64 d = n - 1;
    int s = 0;
    while (d % 2 == 0) {
        d /= 2;
        ++s;
    }
    for (u64 a : bases) {
        u64 x = powmod(a, d, n);
        if (x == 1 || x == n - 1) continue;
        bool witness = true;
        for (int i = 1; i < s; ++i) {
            x = mulmod(x, x, n);
            if (x == n - 1) {
                witness = false;
                break;
            }
        }
        if (witness) return false;
    }
    return true;
}

u64 gcdU(u64 a, u64 b) {
    while (b != 0) {
        u64 t = a % b;
        a = b;
        b = t;
    }
    return a;
}

i64 toSigned(u64 x, const char* what) {
    if (x > static_cast<u64>(LLONG_MAX)) throw std::overflow_error(what);
    return static_cast<i64>(x);
}

i64 isqrt(i64 n) {
    i64 r = static_cast<i64>(std::sqrt(static_cast<long double>(n)));
    while (r * r > n) --r;
    while ((r + 1) * (r + 1) <= n) ++r;
    return r;
}

}  // namespace

bool isDivisible(i64 a, i64 b) {
    if (b == 0) throw std::invalid_argument("на ноль делить нельзя");
    if (b == -1) return true;  // иначе LLONG_MIN % -1 — неопределённое поведение
    return a % b == 0;
}

bool isPrime(i64 n) {
    return n >= 2 && isPrimeU(static_cast<u64>(n));
}

Primality classify(i64 n) {
    if (n < 2) return Primality::Neither;
    return isPrime(n) ? Primality::Prime : Primality::Composite;
}

std::vector<std::pair<i64, int>> factorize(i64 n) {
    if (n == 0) throw std::invalid_argument("ноль нельзя разложить на простые множители");
    if (n == LLONG_MIN) throw std::overflow_error("число слишком велико по модулю");

    u64 m = uabs(n);
    std::vector<std::pair<i64, int>> factors;

    // Делит m на p, пока делится; возвращает true, если множитель нашёлся.
    auto strip = [&](u64 p) {
        int power = 0;
        while (m % p == 0) {
            m /= p;
            ++power;
        }
        if (power > 0) factors.emplace_back(static_cast<i64>(p), power);
        return power > 0;
    };

    strip(2);
    strip(3);
    // Все простые больше 3 имеют вид 6k ± 1. Как только остаток сам оказался простым,
    // перебор можно прекращать — это спасает на больших числах.
    if (!(m > 1 && isPrimeU(m))) {
        for (u64 p = 5; m > 1 && p <= m / p; p += 6) {
            bool found = strip(p);
            if (strip(p + 2)) found = true;
            if (found && isPrimeU(m)) break;
        }
    }
    if (m > 1) factors.emplace_back(static_cast<i64>(m), 1);
    return factors;
}

std::string factorizationToString(i64 n) {
    std::string text = std::to_string(n) + " = ";
    if (n == 1 || n == -1) return text + std::to_string(n);

    bool first = true;
    if (n < 0) {
        text += "-1";
        first = false;
    }
    for (const auto& [prime, power] : factorize(n)) {
        if (!first) text += " × ";
        first = false;
        text += std::to_string(prime);
        if (power > 1) text += "^" + std::to_string(power);
    }
    return text;
}

std::vector<i64> divisors(i64 n) {
    if (n == 0) throw std::invalid_argument("у нуля бесконечно много делителей: он делится на любое число");

    // Делители строятся из разложения: каждый — произведение простых в степенях от 0 до max.
    std::vector<i64> result{1};
    for (const auto& [prime, power] : factorize(n)) {
        std::size_t count = result.size();
        i64 multiplier = 1;
        for (int k = 1; k <= power; ++k) {
            multiplier *= prime;
            for (std::size_t i = 0; i < count; ++i) result.push_back(result[i] * multiplier);
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

i64 gcd(i64 a, i64 b) {
    return toSigned(gcdU(uabs(a), uabs(b)), "НОД не помещается в 64 бита");
}

i64 lcm(i64 a, i64 b) {
    if (a == 0 || b == 0) return 0;
    u64 x = uabs(a), y = uabs(b);
    u64 part = x / gcdU(x, y);
    if (part > static_cast<u64>(LLONG_MAX) / y) throw std::overflow_error("НОК не помещается в 64 бита");
    return static_cast<i64>(part * y);
}

i64 gcd(const std::vector<i64>& numbers) {
    if (numbers.empty()) throw std::invalid_argument("нужно хотя бы одно число");
    i64 result = 0;
    for (i64 x : numbers) result = gcd(result, x);
    return result;
}

i64 lcm(const std::vector<i64>& numbers) {
    if (numbers.empty()) throw std::invalid_argument("нужно хотя бы одно число");
    i64 result = 1;
    for (i64 x : numbers) result = lcm(result, x);
    return result;
}

std::vector<i64> multiplesInRange(i64 lo, i64 hi, i64 k) {
    if (k == 0) throw std::invalid_argument("кратных нулю не бывает: на ноль делить нельзя");
    if (k == LLONG_MIN) throw std::overflow_error("число слишком велико по модулю");
    if (lo > hi) std::swap(lo, hi);
    if (k < 0) k = -k;

    // Первое кратное, не меньшее lo.
    i64 r = lo % k;
    i64 first = lo - r;  // при r <= 0 это уже кратное >= lo
    if (r > 0) {
        if (first > LLONG_MAX - k) return {};
        first += k;
    }
    if (first > hi) return {};

    u64 count = (static_cast<u64>(hi) - static_cast<u64>(first)) / static_cast<u64>(k) + 1;
    if (count > MAX_RESULTS) {
        throw std::length_error("в последовательности " + std::to_string(count) +
                                " чисел, это больше лимита " + std::to_string(MAX_RESULTS) +
                                " — сузьте диапазон");
    }

    std::vector<i64> result;
    result.reserve(static_cast<std::size_t>(count));
    for (i64 x = first;; x += k) {
        result.push_back(x);
        if (x > hi - k) break;  // следующий шаг вышел бы за hi (и, возможно, за LLONG_MAX)
    }
    return result;
}

std::vector<i64> divisibleByAll(i64 lo, i64 hi, const std::vector<i64>& ks) {
    if (ks.empty()) throw std::invalid_argument("нужен хотя бы один делитель");
    for (i64 k : ks) {
        if (k == 0) throw std::invalid_argument("среди делителей есть ноль, а на ноль делить нельзя");
    }
    if (lo > hi) std::swap(lo, hi);

    // Число делится на все ks тогда и только тогда, когда делится на их НОК.
    i64 common;
    try {
        common = lcm(ks);
    } catch (const std::overflow_error&) {
        // НОК больше любого 64-битного числа — в диапазоне подходит разве что ноль.
        if (lo <= 0 && hi >= 0) return {0};
        return {};
    }
    return multiplesInRange(lo, hi, common);
}

std::vector<i64> primesInRange(i64 lo, i64 hi) {
    if (lo > hi) std::swap(lo, hi);
    if (hi < 2) return {};
    if (lo < 2) lo = 2;
    if (hi - lo >= MAX_PRIME_SPAN) {
        throw std::length_error("диапазон слишком длинный: для поиска простых допускается не более " +
                                std::to_string(MAX_PRIME_SPAN) + " чисел");
    }

    std::vector<i64> result;

    // Для очень больших границ решето потребовало бы слишком много «базовых» простых,
    // поэтому там каждое число проверяется отдельно.
    const i64 sieveLimit = 1000000000000LL;  // 10^12
    if (hi > sieveLimit) {
        for (i64 x = lo;; ++x) {
            if (isPrime(x)) result.push_back(x);
            if (x == hi) break;
        }
        return result;
    }

    // Сегментированное решето Эратосфена: сначала простые до sqrt(hi),
    // затем ими вычёркиваются составные на отрезке [lo, hi].
    i64 root = isqrt(hi);
    std::vector<char> smallComposite(static_cast<std::size_t>(root) + 1, 0);
    std::vector<char> composite(static_cast<std::size_t>(hi - lo) + 1, 0);
    for (i64 p = 2; p <= root; ++p) {
        if (smallComposite[static_cast<std::size_t>(p)]) continue;
        for (i64 q = p * p; q <= root; q += p) smallComposite[static_cast<std::size_t>(q)] = 1;
        i64 start = std::max(p * p, (lo + p - 1) / p * p);
        for (i64 q = start; q <= hi; q += p) composite[static_cast<std::size_t>(q - lo)] = 1;
    }
    for (i64 x = lo; x <= hi; ++x) {
        if (!composite[static_cast<std::size_t>(x - lo)]) result.push_back(x);
    }
    return result;
}

}  // namespace dv
