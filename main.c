#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <wchar.h>
#include <locale.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#define ROWS 20
#define COLS 40
#define EMPTY L' '
#define APPLE L'Q'
#define SNAKE L'■'

typedef struct {
    int x, y;
} Point;

void setupMap(wchar_t map[ROWS][COLS]);
void draw(wchar_t map[ROWS][COLS]);
void snakeStart(Point *snake, wchar_t map[ROWS][COLS]);
bool isWall(wchar_t character);
void generateFood(Point *food, Point snake[], int snakeSize);

int main() {
    Point snake;
    wchar_t map[ROWS][COLS];
    setlocale(LC_ALL, ""); // Because I'm using Wchar, this line is needed to print them
    srand(time(NULL)); // This randomizes even more the apple's new positions

    snakeStart(&snake, map);

    while (true) {
        // Reset the map (Clear the old snake position)
        setupMap(map);

        // Put the snake on the map at its new position
        map[snake.x][snake.y] = SNAKE;

        // Draw the map
        system("clear"); // Wipe the old frame
        draw(map); // Draw the new frame

        // 5. Wait for 0.1 seconds (100,000 microseconds)
        usleep(100000); 
    }

    return 0;
}

void setupMap(wchar_t map[ROWS][COLS]) {

        wchar_t Layout[ROWS][COLS + 1] = {
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

void draw(wchar_t map[ROWS][COLS]){
    
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {   
            wprintf(L"%lc", map[i][j]);
        }
        wprintf(L"\n");
    }
}

// Function to star the snake
void snakeStart(Point *snake, wchar_t map[ROWS][COLS]){

    // Create the snake variable in the struct
    snake->x = 10; // Row 10
    snake->y = 20; // Column 20

    // Put the snake icon into the map at those coordinates
    map[snake->x][snake->y] = SNAKE;

}

// Function to detect the walls
bool isWall(wchar_t character) {
    return character == L'┏' || character == L'┓' || character == L'┗' || character == L'┛' || character == L'━' || character == L'┃';
}


// Function to generate food
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
