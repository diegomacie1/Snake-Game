#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>
#include <conio.h>
#include <wchar.h>
#include <locale.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>

#define ROWS 20
#define COLS 40
#define EMPTY L' '
#define POINT L'🍎'
#define SNAKE L'■'

typedef struct {
    int x, y;
} Point;

void setupMap(wchar_t map[ROWS][COLS]);
void printMap(wchar_t map[ROWS][COLS], int score);
bool isWall(wchar_t character);
void setupConsole();


int main(){
    _setmode(_fileno(stdout), _O_U16TEXT);
    setlocale(LC_ALL, "");
    srand(time(NULL));

    wchar_t map[ROWS][COLS];
    Point snake[ROWS * COLS];
    Point food;
    int snakeSize = 3;
    char direction;
    bool gameOver = false;
    int score = 0;

    setupConsole();
    setupMap(map);
    printMap(map, score);

    _getch();
    

    return 0;
}

void setupMap(wchar_t map[ROWS][COLS]) {

        wchar_t Layout[ROWS][COLS] = {
        L"┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓",
        L"┃                                    ┃",
        L"┃                                    ┃",
        L"┃                                    ┃",
        L"┃                                    ┃",
        L"┃                                    ┃",
        L"┃                                    ┃",
        L"┃                                    ┃",
        L"┃                                    ┃",
        L"┃                                    ┃",
        L"┃                                    ┃",
        L"┃                                    ┃",
        L"┃                                    ┃",
        L"┃                                    ┃",
        L"┃                                    ┃",
        L"┃                                    ┃",
        L"┃                                    ┃",
        L"┃                                    ┃",
        L"┃                                    ┃",
        L"┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛",

    };

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {   
            map[i][j] = Layout[i][j];
        }
    }
}

void printMap(wchar_t map[ROWS][COLS], int score) {

    wprintf(L"SCORE: %d\n", score);
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {   
            wprintf(L"%lc", map[i][j]);
        }
        wprintf(L"\n");
    }
}

bool isWall(wchar_t character) {
    return character == L'┏' || character == L'┓' || character == L'┗' || character == L'┛' || character == L'━' || character == L'┃';
}



void setupConsole() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}