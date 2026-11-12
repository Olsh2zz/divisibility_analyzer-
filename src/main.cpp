// «Анализатор делимости»: меню и сценарии работы с пользователем.
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "console.h"
#include "divisibility.h"
#include "history.h"
#include "rules.h"

namespace {

using dv::i64;

const std::size_t SCREEN_LIST_LIMIT = 100;  // сколько чисел последовательности показывать на экране

History history;

enum class Tone { Neutral, Yes, No };

// Печатает результат и кладёт его в историю.
// shown — укороченный вариант для экрана (если пуст, показывается full).
void report(const std::string& operation, const std::string& full, Tone tone = Tone::Neutral,
            const std::string& shown = "") {
    const std::string& text = shown.empty() ? full : shown;
    std::cout << "Результат: ";
    if (tone == Tone::Yes) std::cout << ui::green(text);
    else if (tone == Tone::No) std::cout << ui::red(text);
    else std::cout << text;
    std::cout << "\n";
    history.add(operation, full);
}

// Результат-последовательность: на экране — начало списка, в истории — список целиком.
void reportSequence(const std::string& operation, const std::vector<i64>& values) {
    std::string total = " (всего: " + std::to_string(values.size()) + ")";
    if (values.empty()) {
        report(operation, "[] — подходящих чисел нет", Tone::No);
        return;
    }
    report(operation, ui::formatList(values) + total, Tone::Neutral,
           ui::formatList(values, SCREEN_LIST_LIMIT) + total);
    if (values.size() > SCREEN_LIST_LIMIT) {
        std::cout << ui::yellow("Показаны первые " + std::to_string(SCREEN_LIST_LIMIT) +
                                " чисел; полный список попадёт в файл при экспорте (пункт 10).")
                  << "\n";
    }
}

std::string join(const std::vector<i64>& values, const std::string& separator) {
    std::string text;
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i > 0) text += separator;
        text += std::to_string(values[i]);
    }
    return text;
}

// ---------- 1. Проверка делимости ----------

void checkDivisibility() {
    i64 a = ui::readInt("Введите число: ");
    i64 b = ui::readInt("Введите делитель: ");
    std::string operation = "Проверка делимости: " + std::to_string(a) + " на " + std::to_string(b);

    bool divisible = dv::isDivisible(a, b);  // при b == 0 бросит исключение
    std::string as = std::to_string(a), bs = std::to_string(b);
    std::string quotient = as + " ÷ " + bs + " = " + std::to_string(a / b);
    if (divisible) {
        report(operation, as + " делится на " + bs + " (" + quotient + ")", Tone::Yes);
    } else {
        report(operation,
               as + " не делится на " + bs + " (" + quotient + ", остаток " + std::to_string(a % b) + ")",
               Tone::No);
    }
}

// ---------- 2. Делители ----------

void findDivisors() {
    i64 n = ui::readInt("Введите число: ");
    std::vector<i64> list = dv::divisors(n);  // для нуля бросит исключение
    std::string result = ui::formatList(list) + " (всего: " + std::to_string(list.size()) + ")";
    report("Делители числа " + std::to_string(n), result);
    if (n < 0) {
        std::cout << "Показаны положительные делители; те же числа со знаком минус тоже подходят.\n";
    }
}

// ---------- 3. Простота ----------

void checkPrime() {
    i64 n = ui::readInt("Введите число: ");
    std::string operation = "Проверка на простоту: " + std::to_string(n);
    std::string ns = std::to_string(n);

    switch (dv::classify(n)) {
        case dv::Primality::Prime:
            report(operation, ns + " — простое число", Tone::Yes);
            break;
        case dv::Primality::Composite:
            report(operation,
                   ns + " — составное число (наименьший делитель больше 1: " +
                       std::to_string(dv::factorize(n).front().first) + ")",
                   Tone::No);
            break;
        case dv::Primality::Neither:
            report(operation,
                   ns + " — ни простое, ни составное (простыми и составными бывают только натуральные числа больше 1)",
                   Tone::No);
            break;
    }
}

// ---------- 4. Последовательности ----------

void readRange(i64& lo, i64& hi) {
    lo = ui::readInt("Диапазон: от ");
    hi = ui::readInt("          до ");
    if (lo > hi) {
        std::swap(lo, hi);
        std::cout << "Границы переставлены: от " << lo << " до " << hi << ".\n";
    }
}

std::string rangeText(i64 lo, i64 hi) {
    return " от " + std::to_string(lo) + " до " + std::to_string(hi);
}

void generateSequence() {
    std::cout << "\n  1. Числа, кратные заданному\n"
              << "  2. Числа, делящиеся на несколько заданных\n"
              << "  3. Простые числа\n"
              << "  0. Назад\n";
    i64 choice = ui::readIntInRange("Какую последовательность построить? ", 0, 3);
    if (choice == 0) return;

    i64 lo = 0, hi = 0;
    readRange(lo, hi);

    if (choice == 1) {
        i64 k = ui::readInt("Кратные какому числу: ");
        reportSequence("Кратные " + std::to_string(k) + rangeText(lo, hi), dv::multiplesInRange(lo, hi, k));
    } else if (choice == 2) {
        std::vector<i64> ks = ui::readIntList("Делители через пробел (например: 3 5): ", 1);
        reportSequence("Кратные " + join(ks, " и ") + rangeText(lo, hi), dv::divisibleByAll(lo, hi, ks));
    } else {
        reportSequence("Простые числа" + rangeText(lo, hi), dv::primesInRange(lo, hi));
    }
}

// ---------- 5. НОД и НОК ----------

void findGcdLcm() {
    std::vector<i64> numbers = ui::readIntList("Введите два или больше чисел через пробел: ", 2);
    std::string args = "(" + join(numbers, ", ") + ")";

    std::string result = "НОД" + args + " = " + std::to_string(dv::gcd(numbers)) + "\n";
    try {
        result += "НОК" + args + " = " + std::to_string(dv::lcm(numbers));
    } catch (const std::overflow_error&) {
        result += "НОК" + args + " не помещается в 64 бита";
    }
    std::cout << result << "\n";
    history.add("НОД и НОК чисел " + join(numbers, ", "), result);
}

// ---------- 6. Разложение на множители ----------

void factorizeNumber() {
    i64 n = ui::readInt("Введите число: ");
    std::string result = dv::factorizationToString(n);  // для нуля бросит исключение
    if (dv::isPrime(n)) result += " (простое число, дальше не раскладывается)";
    report("Разложение на простые множители: " + std::to_string(n), result);
}

// ---------- 7. Признаки делимости ----------

void checkRules() {
    std::string number = ui::readNumberString("Введите число (можно очень длинное): ");
    std::vector<dv::RuleResult> results = dv::checkAllRules(number);

    const std::string header = " " + ui::padRight("На", 4) + "| " + ui::padRight("Делится", 8) + "| Почему";
    const std::string line = "-----+---------+" + std::string(40, '-');

    // Таблица собирается дважды: с цветом для экрана и без него для истории и файла.
    std::string plain = header + "\n" + line + "\n";
    std::cout << "\n" << ui::bold(header) << "\n" << line << "\n";
    for (const dv::RuleResult& r : results) {
        std::string divisor = " " + ui::padRight(std::to_string(r.divisor), 4) + "| ";
        std::string answer = ui::padRight(r.divisible ? "да" : "нет", 8);
        std::string rest = "| " + r.explanation;
        plain += divisor + answer + rest + "\n";
        std::cout << divisor << (r.divisible ? ui::green(answer) : ui::red(answer)) << rest << "\n";
    }

    std::vector<i64> matched;
    for (const dv::RuleResult& r : results) {
        if (r.divisible) matched.push_back(r.divisor);
    }
    std::string summary = matched.empty() ? "Не делится ни на одно из чисел 2, 3, 4, 5, 6, 8, 9, 10, 11."
                                          : "Делится на: " + join(matched, ", ") + ".";
    std::cout << summary << "\n";
    history.add("Признаки делимости числа " + number, plain + summary);
}

// ---------- 8. Быстрые шаблоны ----------

std::vector<i64> oddNumbers(i64 lo, i64 hi) {
    std::vector<i64> result;
    for (i64 x = lo; x <= hi; ++x) {
        if (x % 2 != 0) result.push_back(x);
    }
    return result;
}

void quickTemplates() {
    std::cout << "\n  1. Чётные числа от 1 до 100\n"
              << "  2. Нечётные числа от 1 до 100\n"
              << "  3. Простые числа до 100\n"
              << "  4. Простые числа до 1000\n"
              << "  5. Кратные 3 и 5 от 1 до 100\n"
              << "  6. Кратные 10 от 1 до 1000\n"
              << "  0. Назад\n";
    switch (ui::readIntInRange("Выберите шаблон: ", 0, 6)) {
        case 1: reportSequence("Шаблон: чётные числа от 1 до 100", dv::multiplesInRange(1, 100, 2)); break;
        case 2: reportSequence("Шаблон: нечётные числа от 1 до 100", oddNumbers(1, 100)); break;
        case 3: reportSequence("Шаблон: простые числа до 100", dv::primesInRange(1, 100)); break;
        case 4: reportSequence("Шаблон: простые числа до 1000", dv::primesInRange(1, 1000)); break;
        case 5: reportSequence("Шаблон: кратные 3 и 5 от 1 до 100", dv::divisibleByAll(1, 100, {3, 5})); break;
        case 6: reportSequence("Шаблон: кратные 10 от 1 до 1000", dv::multiplesInRange(1, 1000, 10)); break;
        default: break;
    }
}

// ---------- 9–10. История и экспорт ----------

void showHistory() {
    if (history.empty()) {
        std::cout << "История пуста: ещё не выполнено ни одной операции.\n";
        return;
    }
    std::cout << "\n";
    for (std::size_t i = 0; i < history.entries().size(); ++i) {
        const HistoryEntry& e = history.entries()[i];
        std::cout << ui::bold(std::to_string(i + 1) + ". [" + e.time + "] " + e.operation) << "\n";
        // Длинные списки на экране обрезаются, целиком они есть в файле экспорта.
        const std::size_t limit = 2000;
        if (e.result.size() > limit) {
            std::cout << "   (длинный результат, " << e.result.size() << " байт — целиком будет в файле экспорта)\n";
        } else {
            std::size_t pos = 0;
            while (pos < e.result.size()) {
                std::size_t end = e.result.find('\n', pos);
                if (end == std::string::npos) end = e.result.size();
                std::cout << "   " << e.result.substr(pos, end - pos) << "\n";
                pos = end + 1;
            }
        }
    }
    std::cout << "\n  1. Очистить историю\n  0. Назад\n";
    if (ui::readIntInRange("Ваш выбор: ", 0, 1) == 1) {
        history.clear();
        std::cout << "История очищена.\n";
    }
}

void exportHistory() {
    if (history.empty()) {
        std::cout << "Экспортировать нечего: история пуста.\n";
        return;
    }
    std::string path = ui::readLine("Имя файла (Enter — results.txt): ");
    if (path.empty()) path = "results.txt";
    if (history.exportTo(path)) {
        std::cout << ui::green("Сохранено операций: " + std::to_string(history.entries().size()) +
                               ", файл: " + path)
                  << "\n";
    } else {
        std::cout << ui::red("Ошибка: не удалось записать файл «" + path + "»") << "\n";
    }
}

// ---------- Меню ----------

void printMenu() {
    std::cout << "\n"
              << ui::bold("=== Анализатор делимости ===") << "\n"
              << "1. Проверить делимость\n"
              << "2. Найти делители числа\n"
              << "3. Проверить на простоту\n"
              << "4. Сгенерировать последовательность\n"
              << "5. Найти НОД и НОК\n"
              << "6. Разложить на простые множители\n"
              << "7. Признаки делимости\n"
              << "8. Быстрые шаблоны\n"
              << "9. История операций\n"
              << "10. Экспорт результатов в файл\n"
              << "0. Выход\n\n";
}

void run(i64 choice) {
    switch (choice) {
        case 1: checkDivisibility(); break;
        case 2: findDivisors(); break;
        case 3: checkPrime(); break;
        case 4: generateSequence(); break;
        case 5: findGcdLcm(); break;
        case 6: factorizeNumber(); break;
        case 7: checkRules(); break;
        case 8: quickTemplates(); break;
        case 9: showHistory(); break;
        case 10: exportHistory(); break;
        default: break;
    }
}

}  // namespace

int main() {
    ui::setupConsole();
    try {
        while (true) {
            printMenu();
            i64 choice = ui::readIntInRange("Выберите действие: ", 0, 10);
            if (choice == 0) break;
            try {
                run(choice);
            } catch (const std::exception& e) {
                // Сюда попадают деление на ноль, слишком длинные диапазоны и т.п.
                std::cout << ui::red(std::string("Ошибка: ") + e.what()) << "\n";
            }
        }
    } catch (const ui::InputClosed&) {
        std::cout << "\nВвод завершён.\n";
    }
    std::cout << "До свидания!\n";
    return 0;
}
