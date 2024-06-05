#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <unistd.h>
#include <getopt.h>

// Глобальная переменная для хранения искомой последовательности байтов
const char *target_bytes;
// Переменная для включения режима отладки
int debug_mode = 0;

// Прототипы функций
void print_help();
void print_version();
int search_in_file(const char *file_path);
int process_file(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

int main(int argc, char *argv[]) {
    int option_index = 0;
    int c;


    // Определение длинных и коротких опций для getopt_long
    struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };

    // Проверка наличия переменной окружения LAB11DEBUG для включения режима отладки
    if (getenv("LAB11DEBUG") != NULL) {
        debug_mode = 1;
    }

    // Обработка опций командной строки
    while ((c = getopt_long(argc, argv, "hv", long_options, &option_index)) != -1) {
        switch (c) {
            case 'h':
                print_help();
            return EXIT_SUCCESS;
            case 'v':
                print_version();
            return EXIT_SUCCESS;
            case '?':
                    return EXIT_FAILURE;
            default:
                abort();
        }
    }

    // Проверка наличия обязательных аргументов <директория> и <последовательность байтов>
    if (optind + 2 > argc) {
        fprintf(stderr, "Использование: ./lab11aakN3250 [опции] <директория> <последовательность байтов>\n");
        return EXIT_SUCCESS;
    }

    // Получение значений аргументов
    const char *dir_path = argv[optind];
    target_bytes = argv[optind + 1];
    // Запуск рекурсивного обхода файловой системы с использованием nftw
    int res = nftw(dir_path, process_file, 20, FTW_PHYS);

    // Проверка на ошибки выполнения nftw
    if (res < 0) {
        perror("nftw");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// Функция для вывода справки по использованию программы
void print_help() {
    printf("Использование: ./lab11aakN3250 [опции] <директория> <последовательность байтов>\n");
    printf("Программа выполняет рекурсивный поиск заданной последовательности файлов.\n");
    printf("Опции:\n");
    printf("  -h, --help     Вывод инструкций по использованию\n");
    printf("  -v, --version  Вывод информации о версии\n");
}

// Функция для вывода информации о версии программы
void print_version() {
    printf("lab11aakN3250 v1.0\n");
    printf("Автор: Коротыкина Анастасия Александровна, N3250 \nВариант 4\n");
}

// Функция для поиска заданной последовательности байтов в файле
int search_in_file(const char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        perror("fopen");
        return 0;
    }

    // Определение размера файла
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Выделение памяти для чтения файла и дополнительного нулевого символа
    char *buffer = (char *)malloc(file_size + 1);
    if (buffer == NULL) {
        perror("malloc");
        fclose(file);
        return 0;
    }

    // Чтение содержимого файла в буфер
    size_t readsize = fread(buffer, 1, file_size, file);
    buffer[readsize] = '\0'; // Добавление нулевого символа в конец буфера
    fclose(file);

    // Поиск заданной последовательности байтов в буфере
    int found = (strstr(buffer, target_bytes) != NULL);
    // Вывод отладочной информации, если включен режим отладки
    if (debug_mode) {
        if (found) {
            fprintf(stderr, "Found target bytes in file %s", file_path);
        }
        else {
            fprintf(stderr, "Target bytes not found in file %s\n", file_path);
        };
    };
    free(buffer);
    return found;
}

// Функция для обработки каждого файла, найденного nftw
int process_file(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F) {
        if (search_in_file(path)) {
            printf("%s\n", path);
        }
    }
    return 0;
}