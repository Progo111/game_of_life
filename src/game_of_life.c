#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CHAR_LIFE '*'
#define CHAR_DEAD ' '
#define N 25
#define M 80
#define SLEEP 1000000

void output(char **arr);
void top();
void info();

int tick(char **, char **);
int check(char **, int, int, int);
void game(char **, char **);

void completion(char **);
void swap(char **, char **);
int matrix_comparison(char **, char **);

void input_user(char **matrix);
void output_user(char **arr, int x, int y);
void info_user();

// главная функция
int main(int argc, char **argv) {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    int flag = 1;
    char **matrix = malloc(N * M * sizeof(char) + N * sizeof(char *));
    char *row = (char *)(matrix + N);
    for (int i = 0; i < N; ++i) matrix[i] = row + M * i;
    char **buffer_matrix = malloc(N * M * sizeof(char) + N * sizeof(char *));
    row = (char *)(buffer_matrix + N);
    for (int i = 0; i < N; ++i) buffer_matrix[i] = row + M * i;
    // чтение из файла начальной конфигурации

    if (argv[1][0] == 'f') {
        int index = 0;
        char *buffer = matrix[index];
        while (fgets(buffer, M + 1, stdin) != NULL) {
            buffer = matrix[++index];
        }
        if (freopen("/dev/tty", "r", stdin) == NULL) {
            flag = 0;
        }
    } else if (argc == 2 && argv[1][0] == 'u') {
        completion(matrix);
        input_user(matrix);
    } else {
        flag = 0;
    }

    if (flag) {
        completion(matrix);
        output(matrix);
        game(matrix, buffer_matrix);
    }
    endwin();
    free(matrix);
    free(buffer_matrix);
    return 0;
}

void game(char **arr, char **map) {  //основная функция работы игры
    int time_sleep = SLEEP;
    while (1) {
        //вывод начальной конфигурации
        output(arr);
        if (tick(arr, map) == 0) break;  // один такт игры

        // отрисовка второго поколения
        printw("\n");
        char p = getch();
        if (p == 'w') time_sleep -= 100000;
        if (time_sleep < 0) time_sleep = 100;
        if (p == 's') time_sleep += 100000;
        if (time_sleep > 4000000) time_sleep = 4000000;
        if (p == 'q') break;
        usleep(time_sleep);
    }
}
int check(char **matrix, int y, int x, int status) {
    // проверка соседей текущей клетки
    // возврат 1, если клетка в следующем поколении будет жить
    // возврат 0, если клетка умрет
    char char_life = CHAR_LIFE;
    int life = 0, value_life = 0;
    if (matrix[(y + N - 1) % N][(x + M - 1) % M] == char_life) ++value_life;
    if (matrix[(y + N - 1) % N][(x + M) % M] == char_life) ++value_life;
    if (matrix[(y + N - 1) % N][(x + M + 1) % M] == char_life) ++value_life;
    if (matrix[(y + N) % N][(x + M - 1) % M] == char_life) ++value_life;
    if (matrix[(y + N) % N][(x + M + 1) % M] == char_life) ++value_life;
    if (matrix[(y + N + 1) % N][(x + M - 1) % M] == char_life) ++value_life;
    if (matrix[(y + N + 1) % N][(x + M) % M] == char_life) ++value_life;
    if (matrix[(y + N + 1) % N][(x + M + 1) % M] == char_life) ++value_life;

    // условия жизни смерти из условий README
    if (status == 1 && value_life < 2)
        life = 0;
    else if (status == 1 && value_life >= 2 && value_life <= 3)
        life = 1;
    else if (status == 1 && value_life > 3)
        life = 0;
    else if (status == 0 && value_life == 3)
        life = 1;
    return life;
}
int tick(char **matrix, char **new_matrix) {
    // функция одного такта игры
    // идет по матрице поколения и отправляет их на проверку для отрисовки
    // будущего поколения
    char char_life = CHAR_LIFE, char_dead = CHAR_DEAD;

    int status, value_life = 0;

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            if (matrix[i][j] == char_life)
                status = 1;
            else
                status = 0;
            if (check(matrix, i, j, status)) {
                new_matrix[i][j] = char_life;
                ++value_life;
            } else {
                new_matrix[i][j] = char_dead;
            }
        }
    }
    // проверить на повтор
    if (matrix_comparison(new_matrix, matrix) == 0) {
        swap(new_matrix, matrix);
    } else {
        value_life = 0;
    }

    return value_life;
}

void top() {
    for (int i = 0; i < M + 2; ++i) {
        printw("#");
        if (i == M + 1) {
            printw("\n");
        }
    }
    printw("#                          G a m e   o f   L i f e                               #\n");
}

void info() {
    printw("To increase the speed, press 'w'\nTo reduce the speed, press 's'\nTo exit, press 'q'");
}
void output(char **arr) {
    clear();
    top();
    for (int i = 0; i < M + 2; ++i) {
        printw("#");
        if (i == M + 1) {
            printw("\n");
        }
    }

    for (int i = 0; i < N; ++i) {
        printw("#");
        for (int j = 0; j < M; ++j) {
            printw("%c", arr[i][j]);
        }
        printw("#\n");
    }
    for (int i = 0; i < M + 2; ++i) {
        printw("#");
        if (i == M + 1) {
            printw("\n");
        }
    }
    info();
    refresh();
}

void completion(char **matrix) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            if (matrix[i][j] != CHAR_LIFE) {
                matrix[i][j] = CHAR_DEAD;
            }
        }
    }
}

void swap(char **matrOld, char **matrNew) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            char tmp = matrNew[i][j];
            matrNew[i][j] = matrOld[i][j];
            matrOld[i][j] = tmp;
        }
    }
}


int matrix_comparison(char **matrix_first, char **matrix_second) {
    int result = 1;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            if (matrix_first[i][j] != matrix_second[i][j]) {
                result = 0;
                break;
            }
        }
        if (result == 0) {
            break;
        }
    }
    return result;
}

void input_user(char **matrix) {
    int x = 1, y = 1;
    while (1) {
        char p = getch();
        if (p == 'w' && y > 1) y--;
        if (p == 's' && y < 25) y++;
        if (p == 'd' && x < 80) x++;
        if (p == 'a' && x > 1) x--;
        if (p == '\n' && matrix[y - 1][x - 1] == ' ') {
            matrix[y - 1][x - 1] = '*';
        } else if (p == '\n') {
            matrix[y - 1][x - 1] = ' ';
        }
        if (p == 'q') break;
        output_user(matrix, x, y);
        usleep(10000);
    }
}

void output_user(char **arr, int x, int y) {
    clear();
    for (int i = 0; i < M + 2; ++i) {
        printw("#");
        if (i == M + 1) {
            printw("\n");
        }
    }

    for (int i = 0; i < N; ++i) {
        printw("#");
        for (int j = 0; j < M; ++j) {
            printw("%c", arr[i][j]);
        }
        printw("#\n");
    }
    for (int i = 0; i < M + 2; ++i) {
        printw("#");
        if (i == M + 1) {
            printw("\n");
        }
    }
    info_user();
    move(y, x);
    refresh();
}

void info_user() {
    printw(
        "To move, use 'w', 'a', 's', 'd'\nPress 'enter' to put or remove the cell\nPress 'q' to exit input "
        "mode");
}