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
#define APPLE L'🍏'
#define SNAKE L'■'

typedef struct {
    int x, y;
} Point;

void setupMap(wchar_t map[ROWS][COLS]);
void draw(wchar_t map[ROWS][COLS], int score);
void snakeStart(Point snake[], wchar_t map[ROWS][COLS]);
bool isWall(wchar_t character);
void handleInput(char *direction);
void generateFood(Point *food, Point snake[], int snakeSize);
int runGameSession();
char gameOverScreen(int score);
void enableRawMode();
void disableRawMode();
int kbhit();

int main() {
    setlocale(LC_ALL, ""); 
    wprintf(L"\x1b[?25l"); 
    srand(time(NULL)); 
    enableRawMode();

    bool keepPlaying = true;

    while (keepPlaying) {
        
        int finalScore = runGameSession(); // Run the game and get the score

        int choice = gameOverScreen(finalScore); // Show Game Over menu
        
        if (choice == 2) {
            keepPlaying = false;
        }
    }
    
    disableRawMode(); 
    wprintf(L"\x1b[?25h"); 
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

void draw(wchar_t map[ROWS][COLS], int score){
    
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {   
            wprintf(L"%lc", map[i][j]);
        }
        wprintf(L"\n");
    }
    wprintf(L"SCORE: %d", score);
}

// Function to star the snake
void snakeStart(Point snake[], wchar_t map[ROWS][COLS]){

    // Create the snake variable in the struct
    snake[0].x = 10; // Row 10
    snake[0].y = 20; // Column 20

    // Put the snake icon into the map at those coordinates
    map[snake[0].x][snake[0].y] = SNAKE;

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
        char key = getchar(); // Read the key pressed
        
        switch (key) {
            case 'a':
                // Only go LEFT if we aren't currently going RIGHT ('d')
                if (*direction != 'd') *direction = 'a';
                break;
            case 'd':
                // Only go RIGHT if we aren't currently going LEFT ('a')
                if (*direction != 'a') *direction = 'd';
                break;
            case 'w':
                // Only go UP if we aren't currently going DOWN ('s')
                if (*direction != 's') *direction = 'w';
                break;
            case 's':
                // Only go DOWN if we aren't currently going UP ('w')
                if (*direction != 'w') *direction = 's';
                break;
            case 'x':
                // Manual exit
                disableRawMode();
                wprintf(L"\x1b[?25h");
                exit(0);
                break;
        }
    }
}

// This function runs ONE full game and returns the final score
int runGameSession() {

    Point snake[ROWS * COLS];
    int snakeSize = 1;
    int score = 5;
    Point apple;
    wchar_t map[ROWS][COLS];
    char direction = 's'; 
    bool isGameRunning = true;

    setupMap(map);
    snakeStart(snake, map);
    generateFood(&apple, snake, snakeSize);

    // Initial Draw
    map[apple.x][apple.y] = APPLE; 
    map[snake[0].x][snake[0].y] = SNAKE; 
    draw(map, score);
    system("clear");

    while (isGameRunning) {
        setupMap(map);

        for (int i = snakeSize - 1; i > 0; i--) { // Update Tail
            snake[i] = snake[i - 1];
        }

        handleInput(&direction); // User input
        
        if (direction == 'w') snake[0].x--; // Move Head
        else if (direction == 's') snake[0].x++;
        else if (direction == 'a') snake[0].y--;
        else if (direction == 'd') snake[0].y++;

        if (isWall(map[snake[0].x][snake[0].y])) { // Wall Collision
            isGameRunning = false;
        }

        for (int i = 1; i < snakeSize; i++) { // Self Collision
            if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
                isGameRunning = false;
            }
        }

        if (!isGameRunning) break; 

        if (snake[0].x == apple.x && snake[0].y == apple.y) { // Eat Apple
            snakeSize++; 
            score += 5;
            generateFood(&apple, snake, snakeSize); 
            
             if (snakeSize >= (ROWS - 2) * (COLS - 2)) { 
                system("clear");
                wprintf(L"YOU WON!\n");
                return snakeSize - 1; // Return score on win
            }
        }
        
        map[apple.x][apple.y] = APPLE; // Draw Frame
        for (int i = 0; i < snakeSize; i++) {
            map[snake[i].x][snake[i].y] = SNAKE;
        }

        wprintf(L"\x1b[H"); 
        draw(map, score); 
        usleep(200000); 
    }

    return score; // Return the final score
}

char gameOverScreen(int score){

    while (kbhit()) // In case the player presses any key after ending the game
    {
        getchar();
    }

    system("clear");
    wprintf(L"\n\n\n\n");
    wprintf(L"\n");
    wprintf(L"   :'######::::::'###::::'##::::'##:'########:::::'#######::'##::::'##:'########:'########:: \n");
    wprintf(L"  '##... ##::::'## ##::: ###::'###: ##.....:::::'##.... ##: ##:::: ##: ##.....:: ##.... ##:    \n");
    wprintf(L"   ##:::..::::'##:. ##:: ####'####: ##:::::::::: ##:::: ##: ##:::: ##: ##::::::: ##:::: ##:    \n");
    wprintf(L"   ##::'####:'##:::. ##: ## ### ##: ######:::::: ##:::: ##: ##:::: ##: ######::: ########::    \n");
    wprintf(L"   ##::: ##:: #########: ##. #: ##: ##...::::::: ##:::: ##:. ##:: ##:: ##...:::: ##.. ##:::    \n");
    wprintf(L"   ##::: ##:: ##.... ##: ##:.:: ##: ##:::::::::: ##:::: ##::. ## ##::: ##::::::: ##::. ##::    \n");
    wprintf(L"  . ######::: ##:::: ##: ##:::: ##: ########::::. #######::::. ###:::: ########: ##:::. ##:    \n");
    wprintf(L"  :......::::..:::::..::..:::::..::........::::::.......::::::...:::::........::..:::::..::    \n");
    wprintf(L"\n");
    wprintf(L"\n\n");
    wprintf(L"      FINAL SCORE: %d\n", score);
    wprintf(L"\n\n      Select: \n\n");
    wprintf(L"      1 - Play Again\n");
    wprintf(L"\n      2 - Quit\n");

    while(true){
        char choice = getchar();
        if (choice == '1') return 1; // Play again
        if (choice == '2') return 2; // Quit
    }

    return getchar();
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
