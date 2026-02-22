#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#define BUFFER_SIZE 32
#define MAX_STR_VALUE 256

typedef struct {
    char key[32];
    union {
        unsigned long long num;  // для числового значения
        char str[MAX_STR_VALUE]; // для строчного значения
    } value;
    int found;
    int is_string;  // 1 - строка, 0 - число
} Argument;

// Парсер аргументов из строки
int parse_arguments(const char* args_str, Argument* args, int num_args, ...) {
    if (!args_str || !*args_str) return 0;

    va_list va;
    va_start(va, num_args);
    for (int i = 0; i < num_args; i++) {
        strcpy(args[i].key, va_arg(va, const char*));
        args[i].found = 0;
        args[i].is_string = 0;  // по умолчанию число
    }
    va_end(va);

    char* str = _strdup(args_str);
    char* context = NULL;
    char* token = strtok_s(str, " \t", &context);

    while (token) {
        char* eq_pos = strchr(token, '=');
        if (!eq_pos) {
            free(str);
            return 0;
        }

        *eq_pos = '\0';
        char* key = token;
        char* value_str = eq_pos + 1;

        // Определяем тип значения (число или строка)
        int is_numeric = 1;
        for (char* p = value_str; *p && is_numeric; p++) {
            if (!isdigit(*p)) is_numeric = 0;
        }

        int found = 0;
        for (int i = 0; i < num_args; i++) {
            if (strcmp(key, args[i].key) == 0) {
                if (args[i].found) {
                    free(str);
                    return 0; // Дубликат
                }

                if (is_numeric) {
                    // Числовое значение
                    char* endptr;
                    unsigned long long val = strtoull(value_str, &endptr, 10);
                    if (*endptr != '\0') {
                        free(str);
                        return 0;
                    }
                    args[i].value.num = val;
                    args[i].is_string = 0;
                }
                else {
                    // Строковое значение
                    if (strlen(value_str) >= MAX_STR_VALUE) {
                        free(str);
                        return 0; // Слишком длинная строка
                    }
                    strcpy(args[i].value.str, value_str);
                    args[i].is_string = 1;
                }

                args[i].found = 1;
                found = 1;
                break;
            }
        }

        if (!found) {
            free(str);
            return 0; // Неизвестный ключ
        }

        token = strtok_s(NULL, " \t", &context);
    }

    free(str);

    // Проверяем, что все аргументы найдены
    for (int i = 0; i < num_args; i++) {
        if (!args[i].found) return 0;
    }

    return 1;
}

// НОД (Алгоритм Евклида)
unsigned long long gcd(unsigned long long a, unsigned long long b) {
    while (b) {
        unsigned long long t = b;
        b = a % b;
        a = t;
    }
    return a;
}

// Проверка на взаимную простоту
int is_coprime(unsigned long long a, unsigned long long b) {
    return gcd(a, b) == 1;
}

// Разделение на простые множители
typedef struct {
    unsigned long long factor;
    int count;
} Factor;

int factorize(unsigned long long n, Factor* factors) {
    int count = 0;

    if (n % 2 == 0) {
        factors[count].factor = 2;
        factors[count].count = 0;
        while (n % 2 == 0) {
            factors[count].count++;
            n /= 2;
        }
        count++;
    }

    for (unsigned long long i = 3; i * i <= n; i += 2) {
        if (n % i == 0) {
            factors[count].factor = i;
            factors[count].count = 0;
            while (n % i == 0) {
                factors[count].count++;
                n /= i;
            }
            count++;
        }
    }

    if (n > 1) {
        factors[count].factor = n;
        factors[count].count = 1;
        count++;
    }

    return count;
}

// Проверка на переполнение
int would_overflow(unsigned long long a, unsigned long long b) {
    if (a == 0 || b == 0) return 0;
    return a > (18446744073709551615ULL / b);
}

// Команда GET_C
void cmd_get_c(const char* args_str, FILE* output) {
    Argument args[3];

    if (!parse_arguments(args_str, args, 3, "cmin", "cmax", "m")) {
        fprintf(output, "incorrect command\n");
        return;
    }

    unsigned long long cmin = args[0].value.num;
    unsigned long long cmax = args[1].value.num;
    unsigned long long m = args[2].value.num;

    // Проверка условий
    if (cmin == 0 || cmax == 0 || m == 0 || cmin >= m || cmax >= m || cmin > cmax) {
        fprintf(output, "no solution\n");
        return;
    }

    int found = 0;
    for (unsigned long long c = cmin; c <= cmax; c++) {
        if (c < m && c > 0 && is_coprime(c, m)) {
            if (found) fprintf(output, " ");
            fprintf(output, "%llu", c);
            found = 1;
        }
    }

    if (!found) {
        fprintf(output, "no solution");
    }
    fprintf(output, "\n");
}

// Команда GET_A
void cmd_get_a(const char* args_str, FILE* output) {
    Argument args[1];

    if (!parse_arguments(args_str, args, 1, "m")) {
        fprintf(output, "incorrect command\n");
        return;
    }

    unsigned long long m = args[0].value.num;

    if (m == 0 || m > 18446744073709551615ULL) {
        fprintf(output, "no solution\n");
        return;
    }

    // Разложение на простые множители
    Factor factors[64];
    int factor_count = factorize(m, factors);

    if (factor_count == 0) {
        fprintf(output, "no solution\n");
        return;
    }

    // Ищем минимальное a
    for (unsigned long long a = 2; a < m; a++) {
        int valid = 1;

        // Проверяем условие для всех простых делителей
        for (int i = 0; i < factor_count && valid; i++) {
            if ((a - 1) % factors[i].factor != 0) {
                valid = 0;
                break;
            }
        }

        // Если m делится на 4, проверяем дополнительное условие
        if (valid && (m % 4 == 0)) {
            if ((a - 1) % 4 != 0) {
                valid = 0;
            }
        }

        if (valid) {
            fprintf(output, "%llu\n", a);
            return;
        }
    }
    fprintf(output, "no solution\n");
}

// Команда LCG
void cmd_lcg(const char* args_str, FILE* output) {
    Argument args[5];

    if (!parse_arguments(args_str, args, 5, "a", "x0", "c", "m", "n")) {
        fprintf(output, "incorrect command\n");
        return;
    }

    unsigned long long a = args[0].value.num;
    unsigned long long x0 = args[1].value.num;
    unsigned long long c = args[2].value.num;
    unsigned long long m = args[3].value.num;
    unsigned long long n = args[4].value.num;

    // Проверка условий
    if (m == 0 || n == 0) {
        fprintf(output, "no solution\n");
        return;
    }

    if (a >= m || x0 >= m || c >= m) {
        fprintf(output, "no solution\n");
        return;
    }

    unsigned long long x = x0;
    for (unsigned long long i = 0; i < n; i++) {
        if (i > 0) fprintf(output, " ");
        fprintf(output, "%llu", x);

        // x = (a * x + c) % m с проверкой переполнения
        if (would_overflow(a, x)) {
            // Используем алгоритм для больших чисел
            unsigned long long result = 0;
            unsigned long long multiplier = a;
            unsigned long long multiplicand = x;

            while (multiplicand > 0) {
                if (multiplicand & 1) {
                    result = (result + multiplier) % m;
                }
                multiplier = (multiplier * 2) % m;
                multiplicand >>= 1;
            }
            x = (result + c) % m;
        }
        else {
            x = (a * x + c) % m;
        }
    }
    fprintf(output, "\n");
}

// Команда TEST
void cmd_test(const char* args_str, FILE* output) {
    Argument args[1];

    if (!parse_arguments(args_str, args, 1, "inp")) {
        fprintf(output, "incorrect command\n");
        return;
    }

    // Открываем файл с последовательностью
    FILE* seq_file = fopen(args[0].value.str, "r");
    if (!seq_file) {
        fprintf(output, "incorrect command\n");
        return;
    }

    // Читаем числа из файла
    unsigned long long* numbers = NULL;
    size_t count = 0;
    size_t capacity = 0;
    unsigned long long num;

    while (fscanf(seq_file, "%llu", &num) == 1) {
        if (count >= capacity) {
            capacity = capacity == 0 ? 64 : capacity * 2;
            unsigned long long* tmp = realloc(numbers, capacity * sizeof(unsigned long long));
            if (!tmp) {
                free(numbers);
                fclose(seq_file);
                fprintf(output, "incorrect command\n");
                return;
            }
            numbers = tmp;
        }
        numbers[count++] = num;
    }
    fclose(seq_file);

    // Простой тест на равномерность распределения
    // Разбиваем на 10 интервалов
    unsigned long long min_val = numbers[0], max_val = numbers[0];
    for (size_t i = 1; i < count; i++) {
        if (numbers[i] < min_val) min_val = numbers[i];
        if (numbers[i] > max_val) max_val = numbers[i];
    }

    // Если все числа одинаковые
    if (min_val == max_val) {
        free(numbers);
        fprintf(output, "not random: all values are equal\n");
        return;
    }

    unsigned long long range = max_val - min_val + 1;
    int intervals = 10;
    unsigned long long interval_size = range / intervals;
    if (interval_size == 0) interval_size = 1;

    int* observed = calloc(intervals, sizeof(int));
    if (!observed) {
        free(numbers);
        fprintf(output, "incorrect command\n");
        return;
    }

    for (size_t i = 0; i < count; i++) {
        int interval = (numbers[i] - min_val) / interval_size;
        if (interval >= intervals) interval = intervals - 1;
        observed[interval]++;
    }

    // Вычисляем chi-square
    double expected = (double)count / intervals;
    double chi_square = 0;
    for (int i = 0; i < intervals; i++) {
        double diff = observed[i] - expected;
        chi_square += (diff * diff) / expected;
    }

    free(observed);
    free(numbers);

    // Критическое значение для 9 степеней свободы (p=0.05)
    double critical_value = 16.919;

    fprintf(output, "Chi-square test results:\n");
    fprintf(output, "Number of values: %zu\n", count);
    fprintf(output, "Chi-square value: %g\n", chi_square);
    fprintf(output, "Critical value (a=0.05, df=9): %g\n", critical_value);

    if (chi_square < critical_value) {
        fprintf(output, "Conclusion: sequence appears random\n");
    }
    else {
        fprintf(output, "Conclusion: sequence does not appear random\n");
    }
}

// Чтение строки неопределенной длины
char* read_line(FILE* file) {
    char* str = NULL;
    size_t len = 0;
    char buffer[BUFFER_SIZE];

    while (fgets(buffer, BUFFER_SIZE, file)) {
        size_t chunk_len = strlen(buffer);

        if (chunk_len && buffer[chunk_len - 1] == '\n') {
            buffer[--chunk_len] = '\0';
        }

        char* tmp = realloc(str, len + chunk_len + 1);
        if (!tmp) {
            free(str);
            return NULL;
        }
        str = tmp;

        memcpy(str + len, buffer, chunk_len);
        len += chunk_len;
        str[len] = '\0';

        if (chunk_len < BUFFER_SIZE - 1)
            break;
    }

    if (len == 0) {
        free(str);
        return NULL;
    }

    return str;
}

// Выделение команды из строки
char* extract_command(const char* line)
{
    if (!line || !*line) return NULL;

    while (*line && isspace(*line)) line++;
    if (!*line) return NULL;

    const char* start = line;
    while (*line && !isspace(*line)) line++;

    size_t len = line - start;
    char* cmd = malloc(len + 1);
    if (!cmd) return NULL;

    strncpy(cmd, start, len);
    cmd[len] = '\0';
    return cmd;
}

// Проверка наличия такой команды
int is_valid_command(const char* cmd)
{
    const char* valid_commands[] = { "get_c", "get_a", "lcg", "test", NULL };

    for (int i = 0; valid_commands[i]; i++)
    {
        if (strcmp(cmd, valid_commands[i]) == 0)
            return 1;
    }
    return 0;
}

// Получение аргументов из строки
const char* get_arguments(const char* line)
{
    while (*line && !isspace(*line)) line++;
    while (*line && isspace(*line)) line++;
    return line;
}

int main()
{
    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");

    if (!input || !output) {
        if (input) fclose(input);
        if (output) fclose(output);
        return 1;
    }

    char* line = read_line(input);
    if (!line) {
        fprintf(output, "incorrect command\n");
        fclose(input);
        fclose(output);
        return 0;
    }

    char* cmd = extract_command(line);
    if (!cmd || !is_valid_command(cmd))
    {
        fprintf(output, "incorrect command\n");
        free(cmd);
        free(line);
        fclose(input);
        fclose(output);
        return 0;
    }

    const char* args = get_arguments(line);

    if (strcmp(cmd, "get_c") == 0)
    {
        cmd_get_c(args, output);
    }
    else if (strcmp(cmd, "get_a") == 0)
    {
        cmd_get_a(args, output);
    }
    else if (strcmp(cmd, "lcg") == 0)
    {
        cmd_lcg(args, output);
    }
    else if (strcmp(cmd, "test") == 0)
    {
        cmd_test(args, output);
    }

    free(cmd);
    free(line);
    fclose(input);
    fclose(output);
    return 0;
}