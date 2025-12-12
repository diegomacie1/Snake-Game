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
#define APPLE L'O'
#define SNAKE L'■'

typedef struct {
    int x, y;
} Point;

void setupMap(wchar_t map[ROWS][COLS]);
void draw(wchar_t map[ROWS][COLS]);
void snakeStart(Point snake[], wchar_t map[ROWS][COLS]);
bool isWall(wchar_t character);
void handleInput(char *direction);
void generateFood(Point *food, Point snake[], int snakeSize);
char gameOverScreen(int score);
void enableRawMode();
void disableRawMode();
int kbhit();

int main() {
    
    setlocale(LC_ALL, ""); // Because I'm using Wchar, this line is needed to print them
    wprintf(L"\x1b[?25l"); // To hide the cursor
    srand(time(NULL)); // This randomizes even more the apple's new positions
    enableRawMode();

    bool isGameRunning = true;
    bool keepPlaying = true;

    while (keepPlaying) {

        Point snake[ROWS * COLS];
        int snakeSize = 1;
        Point apple;
        wchar_t map[ROWS][COLS];
        char direction = 's'; // Default starting direction (Down)
        bool isGameRunning = true;

        setupMap(map); // Reset the map (Clear the old snake position)
        snakeStart(snake, map);
        generateFood(&apple, snake, snakeSize);

        map[apple.x][apple.y] = APPLE; 
        map[snake[0].x][snake[0].y] = SNAKE; 
        draw(map);
        system("clear");

        while (isGameRunning) {

            setupMap(map);

            for (int i = snakeSize - 1; i > 0; i--) {
                snake[i] = snake[i - 1];
            }
    
            handleInput(&direction); // Pass the address of 'direction'
            
            if (direction == 'w') snake[0].x--;
            else if (direction == 's') snake[0].x++;
            else if (direction == 'a') snake[0].y--;
            else if (direction == 'd') snake[0].y++;
    
            if (isWall(map[snake[0].x][snake[0].y])) { // Collision with tthe wall check
                isGameRunning = false;
            }
    
            for (int i = 1; i < snakeSize; i++) { // Check Self Collision
                if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
                    isGameRunning = false;
                }
            }
    
            if (!isGameRunning) break; // Exits the loop
    
            if (snake[0].x == apple.x && snake[0].y == apple.y) { 
                snakeSize++; // Grow!
                generateFood(&apple, snake, snakeSize); 
                
                 if (snakeSize >= (ROWS - 2) * (COLS - 2)) { // Win Condition Check
                    system("clear");
                    wprintf(L"YOU WON!\n");
                    isGameRunning = false; // Breask the loop on win
                }
            }
            map[apple.x][apple.y] = APPLE;
    
            for (int i = 0; i < snakeSize; i++) {
                map[snake[i].x][snake[i].y] = SNAKE;
            }
    
            wprintf(L"\x1b[H"); // stops the flickering
            draw(map); // Draw the new frame
            
            usleep(200000); // 5. Wait for 0.2 seconds (200,000 microseconds)
        }
        
        int choice = gameOverScreen(snakeSize - 1);
    
        if (choice == 2)
        {
            keepPlaying = false;
        }

    }
    
    disableRawMode(); // Turns off game mode
    wprintf(L"\x1b[?25h"); // Shows the cursor again

    return 0;
}
    wchar_t map[ROWS][COLS];

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
    wprintf(L"      Score: %d\n", score);
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
