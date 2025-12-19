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
#define APPLE L'0'
#define SNAKE L'■'
#define COLOR_RESET   L"\x1b[0m"
#define COLOR_RED     L"\x1b[31m"
#define COLOR_GREEN   L"\x1b[32m"
#define COLOR_YELLOW  L"\x1b[33m"
#define COLOR_BLUE    L"\x1b[34m"
#define COLOR_CYAN    L"\x1b[36m"
#define COLOR_WHITE   L"\x1b[37m"
#define COLOR_PINK    L"\x1b[95m"

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
int gameMenu();
void togglePause();
char gameOverScreen(int score);
void showInstructions();
void enableRawMode();
void disableRawMode();
int kbhit();

int main() {
    setlocale(LC_ALL, ""); 
    wprintf(L"\x1b[?25l"); 
    srand(time(NULL)); 
    enableRawMode();

    bool appRunning = true;

    while (appRunning) {

        int menuChoice = gameMenu(); // Show Main Menu

        if (menuChoice == 1) {// PLAY GAME

            int finalScore = runGameSession(); 
            int choice = gameOverScreen(finalScore);
            if (choice == 2) appRunning = false;
        } 
        else if (menuChoice == 2) {
            showInstructions(); // Shows instructions then the loop restarts, showing menu again

        } 
        else if (menuChoice == 3) { // QUIT
            appRunning = false;
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

    wprintf(L"\x1b[H"); // Prevents the flickering
    
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {   
            wchar_t tile = map[i][j];

            if (tile == SNAKE) {
                // Print Snake in GREEN
                wprintf(L"%ls%lc%ls", COLOR_GREEN, tile, COLOR_RESET);
            } 
            else if (tile == APPLE) {
                // Print Apple in RED
                wprintf(L"%ls%lc%ls", COLOR_RED, tile, COLOR_RESET);
            } 
            else if (isWall(tile)) {
                // Print Walls in CYAN (or any color you like)
                wprintf(L"%ls%lc%ls", COLOR_CYAN, tile, COLOR_RESET);
            } 
            else {
                // Print empty space normally
                wprintf(L"%lc", tile);
            }
        }
        wprintf(L"\n");
    }
    
    // Print the score in YELLOW
    wprintf(L"%lsSCORE: %d%ls", COLOR_YELLOW, score, COLOR_RESET);
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

        if (key == 'q') {
            *direction = 'q'; 
            return; // Exit function immediately
        }
        
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

    Point snake[ROWS * COLS]; // Even though the snake will not be the same as the map, it's safer to keep it like this, because C doesn't protect if you write outside the array
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
    char lastDirection = direction;

    while (isGameRunning) {
        setupMap(map);

        for (int i = snakeSize - 1; i > 0; i--) { // Update Tail
            snake[i] = snake[i - 1];
        }

        handleInput(&direction); // User input

        if (direction == 'q') {
            togglePause();       // Enter the pause loop
            direction = lastDirection; // Restore direction to keep moving
        } else {
            lastDirection = direction; // Update valid direction
        }
        
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

            int growthAmount = 3;

            for (int i = 0; i < growthAmount; i++)
            {
                if (snakeSize >= (ROWS - 2) * (COLS - 2)) break; // To check if we hit the max size to prevent crashing
                snake[snakeSize].x = snake[snakeSize - 1].x; // Initialize the new segment to be smae position as the current tail
                snake[snakeSize].y = snake[snakeSize - 1].y; // This prevents the new square spawning in a random coodinate (garbage memory)
                snakeSize++; // Grow size
            }     

            score += 5; // 5 More points
            generateFood(&apple, snake, snakeSize); // Generate new food
            
             if (snakeSize >= (ROWS - 2) * (COLS - 2)) { 
                system("clear");
                wprintf(L"YOU WON!\n");
                return score; // Return score on win
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

int gameMenu() {
    system("clear");
    wprintf(L"%ls\n\n\n", COLOR_GREEN, COLOR_RESET);
    wprintf(L"      S N A K E   G A M E%ls\n\n", COLOR_GREEN, COLOR_RESET);
    wprintf(L"      [1] - Play Game%ls\n", COLOR_GREEN, COLOR_RESET);
    wprintf(L"      [2] - Instructions%ls\n", COLOR_GREEN, COLOR_RESET);
    wprintf(L"      [3] - Quit%ls\n", COLOR_RESET);

    while (true) {
        char key = getchar();
        if (key == '1') return 1; // Play
        if (key == '2') return 2; // Instructions
        if (key == '3') return 3; // Quit
        usleep(100000); 
    }
}

void togglePause() {
    system("clear");
    wprintf(L"\n\n");
    wprintf(L"      == PAUSED ==\n\n");
    wprintf(L"   Press 'q' to Resume\n");
    
    while (true) {
        char key = getchar();
        if (key == 'q') {
            system("clear"); // Clear the "Paused" text before returning
            return;
        }
        usleep(100000); // Small sleep to save CPU while waiting
    }
}

char gameOverScreen(int score){

    while (kbhit()) // In case the player presses any key after ending the game
    {
        getchar();
    }

    system("clear");
    wprintf(L"\n\n\n\n");
    wprintf(L" %ls\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"   :'######::::::'###::::'##::::'##:'########:::::'#######::'##::::'##:'########:'########:: %ls\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"  '##... ##::::'## ##::: ###::'###: ##.....:::::'##.... ##: ##:::: ##: ##.....:: ##.... ##:    %ls\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"   ##:::..::::'##:. ##:: ####'####: ##:::::::::: ##:::: ##: ##:::: ##: ##::::::: ##:::: ##:    %ls\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"   ##::'####:'##:::. ##: ## ### ##: ######:::::: ##:::: ##: ##:::: ##: ######::: ########::    %ls\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"   ##::: ##:: #########: ##. #: ##: ##...::::::: ##:::: ##:. ##:: ##:: ##...:::: ##.. ##:::    %ls\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"   ##::: ##:: ##.... ##: ##:.:: ##: ##:::::::::: ##:::: ##::. ## ##::: ##::::::: ##::. ##::    %ls\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"  . ######::: ##:::: ##: ##:::: ##: ########::::. #######::::. ###:::: ########: ##:::. ##:    %ls\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"  :......::::..:::::..::..:::::..::........::::::.......::::::...:::::........::..:::::..::    %ls\n", COLOR_RED, COLOR_RESET);
    wprintf(L" %ls\n", COLOR_YELLOW, COLOR_RESET);
    wprintf(L"\n\n");
    wprintf(L"      FINAL SCORE: %d%ls\n", score, COLOR_RED, COLOR_RESET);
    wprintf(L"\n\n      Select: \n\n");
    wprintf(L"      1 - Return to the main menu\n");
    wprintf(L"\n      2 - Quit%ls\n", COLOR_RESET);

    while(true){
        char choice = getchar();
        if (choice == '1') return 1; // Play again
        if (choice == '2') return 2; // Quit
    }

    return getchar();
}

void showInstructions() {
    system("clear");
    wprintf(L"%ls\n\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"      ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓%ls\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"      ┃           HOW TO PLAY                ┃%ls\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"      ┃                                      ┃%ls\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"      ┃   [W] - Move UP                      ┃%ls\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"      ┃   [S] - Move DOWN                    ┃%ls\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"      ┃   [A] - Move LEFT                    ┃%ls\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"      ┃   [D] - Move RIGHT                   ┃%ls\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"      ┃                                      ┃%ls\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"      ┃   [Q] - PAUSE Game                   ┃%ls\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"      ┃   [X] - EXIT Immediately (In Game)   ┃%ls\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"      ┃                                      ┃%ls\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"      ┃   Goal: Eat the apples (0) to grow.  ┃%ls\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"      ┃   Don't hit the walls or yourself!   ┃%ls\n", COLOR_CYAN, COLOR_RESET);
    wprintf(L"      ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛%ls\n", COLOR_YELLOW, COLOR_RESET);
    wprintf(L"\n       Press ANY KEY to go back to menu...");

    while (kbhit()) getchar(); // Flush buffer
    getchar(); // Wait for key
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
