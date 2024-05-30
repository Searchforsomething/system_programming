#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <unistd.h>
#include <getopt.h>

const char *target_bytes;

void print_help();
void print_version();
int search_in_file(const char *file_path);
int process_file(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

int main(int argc, char *argv[]) {
    int option_index = 0;
    int c;

    struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };

    while ((c = getopt_long(argc, argv, "hv", long_options, &option_index)) != -1) {
        switch (c) {
            case 'h':
                print_help();
            return EXIT_SUCCESS;
            case 'v':
                print_version();
            return EXIT_SUCCESS;
            case '?':
                // getopt_long already printed an error message.
                    return EXIT_FAILURE;
            default:
                abort();
        }
    }

    if (optind + 2 > argc) {
        fprintf(stderr, "Использование: ./lab11aakN3250 [опции] <директория> <последовательность байтов>\n");
        return EXIT_SUCCESS;
    }

    const char *dir_path = argv[optind];
    target_bytes = argv[optind + 1];
    int res = nftw(dir_path, process_file, 20, FTW_PHYS);

    if (res < 0) {
        perror("nftw");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void print_help() {
    printf("Использование: ./lab11aakN3250 [опции] <директория> <последовательность байтов>\n");
    printf("Программа выполняет рекурсивный поиск заданной последовательности файлов.\n");
    printf("Опции:\n");
    printf("  -h, --help     Вывод инструкций по использованию\n");
    printf("  -v, --version  Вывод информации о версии\n");
}

void print_version() {
    printf("lab11aakN3250 v1.0\n");
    printf("Автор: Коротыкина Анастасия Александровна, N3250 \nВариант 4\n");
}

int search_in_file(const char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        perror("fopen");
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *buffer = (char *)malloc(file_size + 1);
    if (buffer == NULL) {
        perror("malloc");
        fclose(file);
        return 0;
    }

    size_t readsize = fread(buffer, 1, file_size, file);
    buffer[readsize] = '\0';
    fclose(file);

    int found = (strstr(buffer, target_bytes) != NULL);
    free(buffer);
    return found;
}

int process_file(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F) {
        if (search_in_file(path)) {
            printf("%s\n", path);
        }
    }
    return 0;
}
