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
#include <windows.h>
#include <mmsystem.h>

#define ROWS 20
#define COLS 40
#define EMPTY L' '
#define APPLE L'Q'
#define SNAKE L'■'

typedef struct {
    int x, y;
} Point;

void setupMap(wchar_t map[ROWS][COLS]);
void printMap(wchar_t map[ROWS][COLS], int score);
void moveSnake(Point snake[], int snakeSize, char direction);
void generateFood(Point *food, Point snake[], int snakeSize);
void updateGame(Point snake[], int *pSnakeSize, Point *pApple, int *pScore, char direction, bool *pGameOver, wchar_t map[ROWS][COLS]);
void userIput(char input, char *pDirection);
bool isWall(wchar_t character);
void setCursorPosition(int x, int y);
void setupConsole();


int main(){
    _setmode(_fileno(stdout), _O_U16TEXT);
    setlocale(LC_ALL, "");
    srand(time(NULL));

    wchar_t map[ROWS][COLS];
    Point snake[ROWS * COLS];
    Point apple;
    int snakeSize = 3;
    char direction = 'd';
    bool gameOver = false;
    int score = 0;
    
    setupConsole();
    setupMap(map);
    
    // Initialize snake's starting position in the middle
    snake[0] = (Point){ROWS / 2, COLS / 2};
    snake[1] = (Point){ROWS / 2, COLS / 2 - 1};
    snake[2] = (Point){ROWS / 2, COLS / 2 - 2};
    generateFood(&apple, snake, snakeSize);

     while (!gameOver) {

        // --- RENDER/DRAW PHASE ---
        wchar_t displayMap[ROWS][COLS];
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                displayMap[i][j] = map[i][j];
            }
        }

        displayMap[apple.x][apple.y] = APPLE;
        // Draw the snake on the canvas
        for (int i = 0; i < snakeSize; i++) {
            displayMap[snake[i].x][snake[i].y] = SNAKE;
        }

        printMap(displayMap, score);

        // HANDLE USER INPUT (NON-BLOCKING)
        if (_kbhit()) { // Check if a key has been pressed
            char input = _getch(); // Get the key without waiting
            userIput(input, &direction);
        }

        updateGame(snake, &snakeSize, &apple, &score, direction, &gameOver, map);
        
        // Control the speed
        Sleep(150);
    }
    _getch();
    return 0;
}

void setupMap(wchar_t map[ROWS][COLS]) {

        wchar_t Layout[ROWS][COLS] = {
        L"┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓",
        L"┃                                      ┃",
        L"┃                                      ┃",
        L"┃                                      ┃",
        L"┃                                      ┃",
        L"┃                                      ┃",
        L"┃                                      ┃",
        L"┃                                      ┃",
        L"┃                                      ┃",
        L"┃                                      ┃",
        L"┃                                      ┃",
        L"┃                                      ┃",
        L"┃                                      ┃",
        L"┃                                      ┃",
        L"┃                                      ┃",
        L"┃                                      ┃",
        L"┃                                      ┃",
        L"┃                                      ┃",
        L"┃                                      ┃",
        L"┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛",

    };

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {   
            map[i][j] = Layout[i][j];
        }
    }
}

void printMap(wchar_t map[ROWS][COLS], int score) {
    setCursorPosition(0, 0); // Reset cursor to top-left for smooth animation
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {   
            wprintf(L"%lc", map[i][j]);
        }
        wprintf(L"\n");
    }
    wprintf(L"SCORE: %d\n", score);
}

void moveSnake(Point snake[], int snakeSize, char direction) {
    // Calculate the new position for the head
    Point newHead = snake[0];
    switch (direction) {
        case 'w': newHead.x--; break;
        case 's': newHead.x++; break;
        case 'a': newHead.y--; break;
        case 'd': newHead.y++; break;
    }

    // Move the snake's body forward (the "caterpillar" effect)
    for (int i = snakeSize - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    // Move the snake's head to the new position
    snake[0] = newHead;
}

void generateFood(Point *apple, Point snake[], int snakeSize) {
    bool onSnake;
    do {
        onSnake = false;
        // Generate random coordinates inside the walls
        apple->x = rand() % (ROWS - 2) + 1;
        apple->y = rand() % (COLS - 2) + 1;

        // Check if the food spawned on the snake
        for (int i = 0; i < snakeSize; i++) {
            if (apple->x == snake[i].x && apple->y == snake[i].y) {
                onSnake = true;
                break;
            }
        }
    } while (onSnake);
}

// --- CHECK FOR COLLISIONS ---
void updateGame(Point snake[], int *pSnakeSize, Point *pApple, int *pScore, char direction, bool *pGameOver, wchar_t map[ROWS][COLS]) {
        Point nextHead = snake[0];

        // Calculate where the head WILL BE on the next frame
        switch (direction) {
            case 'w': nextHead.x--; break;
            case 's': nextHead.x++; break;
            case 'a': nextHead.y--; break;
            case 'd': nextHead.y++; break;
        }
        // Wall Collision Check
        // Using the isWall function
        if (isWall(map[nextHead.x][nextHead.y])) {
            *pGameOver = true;
            return; // Exit this function
        }
        // Self Collision Check
        // Loop through the snake's body to see if the next spot is part of itself
        for (int i = 0; i < *pSnakeSize; i++) {
            if (nextHead.x == snake[i].x && nextHead.y == snake[i].y) {
                *pGameOver = true;
                return; // Exit this functin 
            }
        }

    // Check for food, but don't respawn it yet
    bool foodWasEaten = false;
    // Food Check
    if (nextHead.x == pApple->x && nextHead.y == pApple->y) {
        // Store the current tail's position BEFORE growing the snake
        Point tail_position = snake[*pSnakeSize - 1];
        *pSnakeSize += 3;
        foodWasEaten = true;
        
        // Place the new segments at the old tail's position.
        // This gives them a safe, valid starting point.
        snake[*pSnakeSize - 3] = tail_position;
        snake[*pSnakeSize - 2] = tail_position;
        snake[*pSnakeSize - 1] = tail_position;

        *pScore += 10;
        generateFood(pApple, snake, *pSnakeSize);
        PlaySound("creamy", NULL, SND_FILENAME | SND_ASYNC);
    }

    moveSnake(snake, *pSnakeSize, direction);

     // If food was eaten, generate a new one AFTER the snake has moved
    if (foodWasEaten) {
        generateFood(pApple, snake, *pSnakeSize);
    }

}

void userIput(char input, char *pDirection) {

    // Change direction, but don't let the snake reverse on itself.
    // For example, if moving up ('w'), you can't go down ('s').
    if ((input == 'w' || input == 'W') && *pDirection != 's') *pDirection = 'w';
    else if ((input == 's' || input == 'S') && *pDirection != 'w') *pDirection = 's';
    else if ((input == 'a' || input == 'A') && *pDirection != 'd') *pDirection = 'a';
    else if ((input == 'd' || input == 'D') && *pDirection != 'a') *pDirection = 'd';

}

bool isWall(wchar_t character) {
    return character == L'┏' || character == L'┓' || character == L'┗' || character == L'┛' || character == L'━' || character == L'┃';
}

void setCursorPosition(int x, int y) {
    COORD pos = {(short)x, (short)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void setupConsole() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}
