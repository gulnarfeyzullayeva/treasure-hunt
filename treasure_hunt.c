#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define ROWS 8
#define COLS 8
#define TREASURES 5
#define TRAPS 3
#define STARTING_LIVES 3
#define EMPTY 0
#define TREASURE 1
#define TRAP -1
#define HIDDEN 0
#define REVEALED 1
// these characters are used when the board is printed for the player
#define HIDDEN_SYMBOL '#'
#define TREASURE_SYMBOL 'T'
#define TRAP_SYMBOL 'X'
#define EMPTY_SYMBOL '.'   
#define LINE_SIZE 100

// the board array tells us what is hidden in each cell and the revealed array tells us whether the player has already opened that cell
void initializeBoards(int board[][COLS], int revealed[][COLS]) {
    int row, col;
    for (row = 0; row < ROWS; row++) {
        for (col = 0; col < COLS; col++) {
            board[row][col] = EMPTY;
            revealed[row][col] = HIDDEN;
        }
    }
}

// this function checks if a position is inside the board 
int isInside(int row, int col) {
    if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
        return 1;
    }
    return 0;
}

void placeObjects(int board[][COLS]) {
    int row, col;
    int placed = 0;
    while (placed < TREASURES) {  // keep choosing random cells until all five treasures have been placed, if a chosen cell is already occupied,counter doesn't increase and the loop tries another random position
        row = rand() % ROWS;
        col = rand() % COLS;
        if (board[row][col] == EMPTY) {
            board[row][col] = TREASURE;
            placed++;
        }
    }
    placed = 0;  //the treasure count is finished, so I start the counter from zero again because now it will count three traps instead
    while (placed < TRAPS) {
        row = rand() % ROWS;
        col = rand() % COLS;
        if (board[row][col] == EMPTY) {
            board[row][col] = TRAP;
            placed++;
        }
    }
}

// when the player opens an empty cell this function finds out how many treasures are in the cells around it. The selected cell itself is not a neighbor. The function only counts treasures
int countNearbyTreasures(int board[][COLS], int row, int col) {
    int dr, dc;
    int r, c;
    int count = 0;
    for (dr = -1; dr <= 1; dr++) {
        for (dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) {
                continue;  //when both changes are zero the position is the center cell itself
            }
            r = row + dr;
            c = col + dc;
            if (isInside(r, c) && board[r][c] == TREASURE) {
                count++;
            }
        }
    }
    return count;
}

// It counts treasures in the scanner area whether the cells are hidden or already revealed
int countTreasuresInRegion(int board[][COLS], int row, int col) {
    int dr, dc;
    int r, c;
    int count = 0;
    for (dr = -1; dr <= 1; dr++) {
        for (dc = -1; dc <= 1; dc++) {
            r = row + dr;
            c = col + dc;
            if (isInside(r, c) && board[r][c] == TREASURE) {
                count++;
            }
        }
    }
    return count;
}

// this function checks the same scanner area, but counts traps instead of treasures
int countTrapsInRegion(int board[][COLS], int row, int col) {
    int dr, dc;
    int r, c;
    int count = 0;
    for (dr = -1; dr <= 1; dr++) {
        for (dc = -1; dc <= 1; dc++) {
            r = row + dr;
            c = col + dc;
            if (isInside(r, c) && board[r][c] == TRAP) {
                count++;
            }
        }
    }
    return count;
}

// this function prints the board with row and column numbers, so the player can easily choose a position. During the game showAll is 0 and hidden cells are shown as #
void printBoard(int board[][COLS], int revealed[][COLS], int showAll) {
    int row, col;
    printf("\n    ");
    for (col = 0; col < COLS; col++) {
        printf("%d ", col);
    }
    printf("\n");
    for (row = 0; row < ROWS; row++) {
        printf("%d | ", row);
        for (col = 0; col < COLS; col++) {
            if (showAll == 1) {  //when the game is over the player is allowed to see everything
                if (board[row][col] == TREASURE) {
                    printf("%c ", TREASURE_SYMBOL);
                }
                else if (board[row][col] == TRAP) {
                    printf("%c ", TRAP_SYMBOL);
                }
                else {
                    printf("%c ", EMPTY_SYMBOL);
                }
            }
            else {  //while playing it first check whether this cell is still hidden. If it is, it prints # without showing what is actually inside it
                if (revealed[row][col] == HIDDEN) {
                    printf("%c ", HIDDEN_SYMBOL);
                }
                else if (board[row][col] == TREASURE) {
                    printf("%c ", TREASURE_SYMBOL);
                }
                else if (board[row][col] == TRAP) {
                    printf("%c ", TRAP_SYMBOL);
                }
                else {
                    printf("%d ", countNearbyTreasures(board, row, col));
                }
            }
        }
        printf("\n");
    }
    printf("\n");
}

int revealCell(int board[][COLS], int revealed[][COLS], int row, int col) {
    revealed[row][col] = REVEALED;
    return board[row][col];
}

// I use this function whenever the program needs a row or column number. It reads a full line from the player and keeps asking until the input is a whole number
int readInt(const char *prompt) {
    char line[LINE_SIZE];
    int value;
    char extra;
    while (1) {
        printf("%s", prompt);
        if (fgets(line, LINE_SIZE, stdin) == NULL) {
            printf("\nInput ended. Goodbye.\n");
            exit(0);
        }
        if (sscanf(line, "%d %c", &value, &extra) == 1) {
            return value;
        }
        printf("Please enter a whole number.\n");
    }
}

// at the start of a turn the player can choose R or S. This function reads that choice and returns it to main. Lowercase r and s are accepted too but other input is invalid
char readAction(void) {
    char line[LINE_SIZE];
    int i = 0;
    char letter;
    printf("\nAction (R = reveal, S = scanner): ");
    if (fgets(line, LINE_SIZE, stdin) == NULL) {
        printf("\nInput ended. Goodbye.\n");
        exit(0);
    }
    while (line[i] == ' ' || line[i] == '\t') {
        i++;
    }
    letter = line[i];
    if (letter == 'r') {
        letter = 'R';
    }
    else if (letter == 's') {
        letter = 'S';
    }
    if (letter != 'R' && letter != 'S') {
        return '?';
    }
    i++;
    while (line[i] == ' ' || line[i] == '\t') {
        i++;
    }
    if (line[i] != '\n' && line[i] != '\0') {
        return '?';
    }
    return letter;
}

// main function
int main(void) {
    int board[ROWS][COLS];
    int revealed[ROWS][COLS];
    int lives = STARTING_LIVES;
    int treasuresFound = 0;
    int scannerUsed = 0;
    int row, col;
    int result;
    int nearby;
    int treasureCount, trapCount;
    char action;
    srand(time(NULL));
    initializeBoards(board, revealed);
    placeObjects(board);
    printf("================================\n");
    printf("          TREASURE HUNT\n");
    printf("================================\n");
    // the game keeps going only while the player has at least one life and has not found all five treasures. As soon as one of these conditions becomes false, the loop ends and the final result is printed
    while (lives > 0 && treasuresFound < TREASURES) { 
        printBoard(board, revealed, 0);
        printf("Lives: %d    ", lives);
        printf("Treasures: %d/%d    ", treasuresFound, TREASURES);
        if (scannerUsed == 0) {
            printf("Scanner: available\n");
        }
        else {
            printf("Scanner: used\n");
        }
        action = readAction();
        // reveal action
        if (action == 'R') {
            while (1) {
                row = readInt("Row: ");
                col = readInt("Column: ");
                if (!isInside(row, col)) {
                    printf("Invalid coordinates. Coordinates must be from 0 to %d.\n", ROWS - 1);
                    continue;
                }
                if (revealed[row][col] == REVEALED) {
                    printf("This cell has already been explored. Choose another cell.\n");
                    continue;
                }
                result = revealCell(board, revealed, row, col);
                if (result == TREASURE) {
                    printf("You found a treasure!\n");
                    treasuresFound++;
                }
                else if (result == TRAP) {
                    printf("Trap! You lost one life.\n");
                    lives--;
                }
                else {
                    nearby = countNearbyTreasures(board, row, col);
                    if (nearby == 1) {
                        printf("Empty cell. There is 1 nearby treasure.\n");
                    }
                    else {
                        printf("Empty cell. There are %d nearby treasures.\n", nearby);
                    }
                }
                break;
            }
        }
        // scanner action
        else if (action == 'S') {
            if (scannerUsed == 1) {
                printf("The scanner is unavailable. You have already used it.\n");
                continue;
            }
            while (1) {
                row = readInt("Scanner center row: ");
                col = readInt("Scanner center column: ");
                if (!isInside(row, col)) {
                    printf("Invalid coordinates. Coordinates must be from 0 to %d. The scanner is still available.\n", ROWS - 1);
                    continue;
                }
                treasureCount = countTreasuresInRegion(board, row, col);
                trapCount = countTrapsInRegion(board, row, col);
                printf("Scanner result: %d treasure%s and %d trap%s in this region.\n", treasureCount, treasureCount == 1 ? "" : "s", trapCount, trapCount == 1 ? "" : "s");
                scannerUsed = 1;
                break;
            }
        }
        // invalid action
        else {
            printf("Invalid action. Please enter R or S.\n");
        }
    }
    // finding all treasures means the player won, reaching zero lives means the player lost
    printf("\n================================\n");
    if (treasuresFound == TREASURES) {
        printf("CONGRATULATIONS! YOU WIN!\n");
    }
    else if (lives == 0) {
        printf("GAME OVER! YOU LOST!\n");
    }
    printf("================================\n");
    printf("\nFinal board:\n");
    printBoard(board, revealed, 1);
    return 0;
}
