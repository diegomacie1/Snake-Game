#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <wchar.h>
#include <locale.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

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
void handleInput(char *direction);
void generateFood(Point *food, Point snake[], int snakeSize);
void enableRawMode();
void disableRawMode();
int kbhit();

int main() {
    Point snake;
    Point apple;
    wchar_t map[ROWS][COLS];
    char direction = 's'; // Default starting direction (Down)

    setlocale(LC_ALL, ""); // Because I'm using Wchar, this line is needed to print them
    wprintf(L"\x1b[?25l"); // To hide the cursor
    srand(time(NULL)); // This randomizes even more the apple's new positions

    enableRawMode();

    setupMap(map);
    snakeStart(&snake, map);
    generateFood(&apple, &snake, 1);
    draw(map);
    system("clear");

    while (true) {

        handleInput(&direction); // Pass the address of 'direction'

        if (direction == 'w') snake.x--;
        else if (direction == 's') snake.x++;
        else if (direction == 'a') snake.y--;
        else if (direction == 'd') snake.y++;

        setupMap(map); // Reset the map (Clear the old snake position)

        map[snake.x][snake.y] = SNAKE; // Puts the snake on the map at its new position
        map[apple.x][apple.y] = APPLE; // Puts the apple

        if (snake.x == apple.x && snake.y == apple.y) { // Check for collision with Apple
            generateFood(&apple, &snake, 1); // Generates a new one.
            // More to come
        }

        wprintf(L"\x1b[H"); // stops the flickering
        draw(map); // Draw the new frame

        usleep(200000); // 5. Wait for 0.2 seconds (200,000 microseconds)
    }

    disableRawMode(); // Turns off game mode
    wprintf(L"\x1b[?25h"); // Shows the cursor again
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

// We pass 'direction' as a pointer so we can modify the original variable
void handleInput(char *direction) {
    if (kbhit()) {
        char input = getchar(); // Read the key
        
        // Only update if it is a valid movement key
        if (input == 'w' || input == 'a' || input == 's' || input == 'd') {
            *direction = input; // Update the value at the pointer's address
        }
    }
}

// Enable "Raw Mode" (disable waiting for Enter)
void enableRawMode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term); // Get current settings
    term.c_lflag &= ~(ICANON | ECHO); // Disable buffering (ICANON) and local echo (ECHO)
    tcsetattr(STDIN_FILENO, TCSANOW, &term); // Apply settings
}

// Disable "Raw Mode" (restore settings when game ends)
void disableRawMode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO); // Re-enable them
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Check if a key has been pressed
int kbhit() {
    int bytesWaiting;
    ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}
