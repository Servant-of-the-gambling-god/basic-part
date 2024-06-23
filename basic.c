// basic part

#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <unistd.h>
#include <signal.h>

#define ROW 26 // Number of rows in the game area
#define COL 12 // Number of columns in the game area

#define DOWN 80 // Key code for the down arrow
#define LEFT 75 // Key code for the left arrow
#define RIGHT 77 // Key code for the right arrow
#define UP 72 // Key code for the up arrow

#define SPACE 32 // Key code for the space bar
#define ESC 27 // Key code for the Esc key

struct Face {
    int data[COL + 10][ROW]; // Indicates if a block exists at a specific position (1 for yes, 0 for no)
    int color[COL + 10][ROW]; // Records the color code of the block at a specific position
} face;

struct Block {
    int space[4][4]; // Stores the shape of a block
} block[7][4]; // Stores the 7 types of Tetris blocks and their 4 rotations each

void HideCursor(); // Hide the cursor
void MoveCursor(int x, int y); // Move the cursor to a specific position
void InitInterface(); // Initialize the game interface
void InitBlock(); // Initialize block shapes
void color(int num); // Set color
void DrawBlock(int shape, int form, int x, int y); // Draw a block
void DrawSpace(int shape, int form, int x, int y); // Draw spaces to clear block
int IsLegal(int shape, int form, int x, int y); // Check if a move is legal
int JudeFunc(); // Check scoring and game over
void StartGame(); // Main game logic function
void RdGrade(); // Read the highest score from file
void WrGrade(); // Update the highest score to file
int PutWhere(int shape, int form, int x, int y); // Determine where the block will be placed
void MoveWhere(int shape, int form, int x, int y); // Show where the block will be moved

int max, grade; // Global variables for max score and current score
int down_reaction_time = 0; // Time delay for the block to move down

int main() {
    #pragma warning (disable:4996) // Disable warnings
    system("cls");
    max = 0, grade = 0; // Initialize variables
    system("title Tetris"); // Set the console window title
    system("mode con lines=26 cols=45"); // Set the console window size
    HideCursor(); // Hide the cursor
    RdGrade(); // Read the highest score from the file
    InitInterface(); // Initialize the interface
    InitBlock(); // Initialize the block shapes
    srand((unsigned int)time(NULL)); // Set the seed for random number generation
    StartGame(); // Start the game
    return 0;
}

void HideCursor() {
    CONSOLE_CURSOR_INFO curInfo;
    curInfo.dwSize = 1;
    curInfo.bVisible = FALSE;
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorInfo(handle, &curInfo);
}

void MoveCursor(int x, int y) {
    COORD pos;
    pos.X = x;
    pos.Y = y;
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(handle, pos);
}

void InitInterface() {
    color(7);
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL + 10; j++) {
            if (j == 0 || j == COL - 1 || j == COL + 9) {
                face.data[j][i] = 1;
                MoveCursor(2 * j, i);
                printf("■");
            } else if (i == ROW - 1) {
                face.data[j][i] = 1;
                printf("■");
            } else {
                face.data[j][i] = 0;
            }
        }
    }
    for (int i = COL; i < COL + 10; i++) {
        face.data[i][8] = 1;
        MoveCursor(2 * i, 8);
        printf("■");
    }
    for (int i = COL; i < COL + 10; i++) {
        face.data[i][8] = 1;
        MoveCursor(2 * i, 16);
        printf("■");
    }
    MoveCursor(2 * COL, 0 );
    printf("Next block:");

    MoveCursor(2 * COL, 9);
    printf("Change:");

    MoveCursor(2 * COL, 19);
    printf("  Record:  %d", max);

    MoveCursor(2 * COL, 22);
    printf("  Grade:  ");
}

void InitBlock() {
    // Initialize T-shape
    for (int i = 0; i <= 2; i++) block[0][0].space[1][i] = 1;
    block[0][0].space[2][1] = 1;

    // Initialize L-shape
    for (int i = 1; i <= 3; i++) block[1][0].space[i][1] = 1;
    block[1][0].space[3][2] = 1;

    // Initialize J-shape
    for (int i = 1; i <= 3; i++) block[2][0].space[i][2] = 1;
    block[2][0].space[3][1] = 1;

    for (int i = 0; i <= 1; i++) {
        // Initialize Z-shape
        block[3][0].space[1][i] = 1;
        block[3][0].space[2][i + 1] = 1;
        // Initialize S-shape
        block[4][0].space[1][i + 1] = 1;
        block[4][0].space[2][i] = 1;
        // Initialize O-shape
        block[5][0].space[1][i + 1] = 1;
        block[5][0].space[2][i + 1] = 1;
    }

    // Initialize I-shape
    for (int i = 0; i <= 3; i++) block[6][0].space[i][1] = 1;

    int temp[4][4];
    for (int shape = 0; shape < 7; shape++) {
        for (int form = 0; form < 3; form++) {
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    temp[i][j] = block[shape][form].space[i][j];
                }
            }
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    block[shape][form + 1].space[i][j] = temp[3 - j][i];
                }
            }
        }
    }
}

void color(int c)
{
    switch (c)
    {
    case 0:
        c = 13; 
        break;
    case 1:
        c = 1;
        break;
    case 2:
        c = 6; 
        break;
    case 3:
        c = 4; 
        break;
    case 4:
        c = 10; 
        break;
    case 5:
        c = 14; 
        break;
    case 6:
        c = 11; 
        break;
    default:
        c = 7;
        break;
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c); 
}


void DrawBlock(int shape, int form, int x, int y) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (block[shape][form].space[i][j] == 1) {
                MoveCursor(2 * (x + j), y + i);
                printf("■");
            }
        }
    }
}

void DrawSpace(int shape, int form, int x, int y) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (block[shape][form].space[i][j] == 1) {
                MoveCursor(2 * (x + j), y + i);
                printf("  ");
            }
        }
    }
}

int IsLegal(int shape, int form, int x, int y) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if ((block[shape][form].space[i][j] == 1) && (face.data[x + j][y + i] == 1 || face.data[x + j][y + i] == 2 || face.data[x + j][y + i] == 3 || face.data[x + j][y + i] == 15)) {
                return 0;
            }
        }
    }
    return 1;
}

int JudeFunc() {
    for (int i = ROW - 2; i > 4; i--) {
        int sum = 0;
        for (int j = 1; j < COL - 1; j++) {
            sum += face.data[j][i];
        }
        if (sum == 0) break;
        if (sum == COL - 2 || sum == COL + 12) {
            grade += 5;
            color(7);
            MoveCursor(2 * COL + 11, 22);
            printf("%d", grade);
            for (int j = 1; j < COL - 1; j++) {
                face.data[j][i] = 0;
                MoveCursor(2 * j, i);
                printf("  ");
            }
            for (int m = i; m > 1; m--) {
                sum = 0;
                for (int n = 1; n < COL - 1; n++) {
                    sum += face.data[n][m - 1];
                    face.data[n][m] = face.data[n][m - 1];
                    face.color[n][m] = face.color[n][m - 1];
                    if (face.data[n][m] == 1) {
                        MoveCursor(2 * n, m);
                        color(face.color[n][m]);
                        printf("■");
                    } else {
                        MoveCursor(2 * n, m);
                        printf("  ");
                    }
                }
                if (sum == 0) return 1;
            }
        }
    }
    for (int j = 1; j < COL - 1; j++) {
        if (face.data[j][1] == 1) {
            Sleep(1000);
            system("cls");
            color(7);
            MoveCursor(2 * (COL / 3), 5);
            if (grade > max) {
                printf("New high score: %d", grade);
                WrGrade();
            } else if (grade == max) {
                printf("Tied with the high score, keep going!");
            } else {
                printf("Grade : %d", grade);
                MoveCursor(2 * (COL / 3), 8);
                printf("Record : %d", max);
            }
            MoveCursor(2 * (COL / 3), 2);
            printf("GAME OVER");
            while (1) {
                char ch;
                MoveCursor(2 * (COL / 3), 11);
                printf("Play again? (y/n):");
                scanf("%c", &ch);
                if (ch == 'y' || ch == 'Y') {
                    system("cls");
                    main();
                } else if (ch == 'n' || ch == 'N') {
                    MoveCursor(2 * (COL / 3), 13);
                    exit(0);
                } 
            }
        }
    }
    return 0;
}

int PutWhere(int shape, int form, int x, int y) {
    while (1) {
        if (IsLegal(shape, form, x, y + 1) == 1) {
            y++;
        } else {
            break;
        }
    }
    color(11);
    DrawBlock(shape, form, x, y);
    return y;
}

void MoveWhere(int shape, int form, int x, int y) {
    DrawSpace(shape, form, x, y);
}

void StartGame() {
    usleep(1000);

    int shape = rand() % 7, form = rand() % 4;
    int change_shape = -1, change_form = -1;
    int record_space_block_Y;

    while (1) {
       
        
        int Is_change = 0;
        int nextShape, nextForm;
        nextShape = rand() % 7, nextForm = rand() % 4;
        int t = 0;
        int x = COL / 2 - 2, y = 0;
        color(nextShape);
        DrawBlock(nextShape, nextForm, COL + 2, 3);
    
        record_space_block_Y = PutWhere(shape, form, x, y);

        while (1) {
            color(shape);
            DrawBlock(shape, form, x, y);
 
            if (t == 0) {
                t = 15000;
            }
            while (--t) {
                if (kbhit() != 0) break;
            }
            if (t == 0) {
                if (IsLegal(shape, form, x, y + 1) == 0) {
                    color(shape);
                    DrawBlock(shape, form, x, y);
                    for (int i = 0; i < 4; i++) {
                        for (int j = 0; j < 4; j++) {
                            if (block[shape][form].space[i][j] == 1) {
                                face.data[x + j][y + i] = 1;
                                face.color[x + j][y + i] = shape;
                            }
                        }
                    }

                    while (JudeFunc());
                    //score_threshold = clear_topmost_BOMB_row(score_threshold); // Clear topmost BOMB row if score threshold is met
                    break;
                } else {
                    DrawSpace(shape, form, x, y);
                    y++;
                }
            } else {
                char ch = getch();
                switch (ch) {
                    case 'c':
                    case 'C':
                        if (Is_change == 0) {
                            DrawSpace(shape, form, x, y);
                            MoveWhere(shape, form, x, record_space_block_Y);
 
                            if (change_shape == -1 && change_form == -1) {
                                DrawSpace(nextShape, nextForm, COL + 2, 3);
                                change_shape = shape, change_form = form;
                                shape = nextShape, form = nextForm;
                                nextShape = rand() % 7, nextForm = rand() % 4;
                                color(nextShape);
                                DrawBlock(nextShape, nextForm, COL + 2, 3);
 
                                color(change_shape);
                                DrawBlock(change_shape, change_form, COL + 2, 10);
                                x = COL / 2 - 2, y = 0;
 
                                color(shape);
                                DrawBlock(shape, form, x, y);
                                record_space_block_Y = PutWhere(shape, form, x, y);
                            } else {
                                DrawSpace(change_shape, change_form, COL + 2, 10);
                                int tmp_shape, tmp_form;
                                tmp_shape = shape, tmp_form = form;
                                shape = change_shape, form = change_form;
                                change_shape = tmp_shape, change_form = tmp_form;
                                color(change_shape);
                                DrawBlock(change_shape, change_form, COL + 2, 10);
                                x = COL / 2 - 2, y = 0;
                                color(shape);
                                DrawBlock(shape, form, x, y);
                                record_space_block_Y = PutWhere(shape, form, x, y);
                            }
                            Is_change++;
                        }
                        break;
                    case DOWN:
                        down_reaction_time = 200;
                        if (IsLegal(shape, form, x, y + 1) == 1) {
                            color(shape);
                            DrawSpace(shape, form, x, y);
                            y++;
                        } else {
                            color(shape);
                            DrawBlock(shape, form, x, y);
                        }
                        break;
                    case LEFT:
                        if (IsLegal(shape, form, x - 1, y) == 1) {
                            MoveWhere(shape, form, x, record_space_block_Y);
                            color(shape);
                            DrawSpace(shape, form, x, y);
                            x--;
                            record_space_block_Y = PutWhere(shape, form, x, y);
                        }
                        break;
                    case RIGHT:
                        if (IsLegal(shape, form, x + 1, y) == 1) {
                            MoveWhere(shape, form, x, record_space_block_Y);
                            DrawSpace(shape, form, x, y);
                            x++;
                            record_space_block_Y = PutWhere(shape, form, x, y);
                        }
                        break;
                    case SPACE:
                        MoveWhere(shape, form, x, record_space_block_Y);
                        while (1) {
                            if (IsLegal(shape, form, x, y + 1) == 1) {
                                DrawSpace(shape, form, x, y);
                                y++;
                            } else {
                                break;
                            }
                        }
                        color(shape);
                        DrawBlock(shape, form, x, y);
                        break;
                    case UP:
                        // Check rotation near edges and adjust position if necessary
                        {
                            int new_form = (form + 1) % 4;
                            int temp_x = x;
                            DrawSpace(shape, form, x, y);  // Clear the current block before rotating
 
                            MoveWhere(shape, form, x, record_space_block_Y);
                            int legal = IsLegal(shape, new_form, x, y);
                            if (legal == 0) {
                                // Try adjusting the position to the left
                                if (IsLegal(shape, new_form, x - 1, y) == 1) {
                                    temp_x = x - 1;
                                    legal = 1;
                                }
                                // Try adjusting the position to the right
                                else if (IsLegal(shape, new_form, x + 1, y) == 1) {
                                    temp_x = x + 1;
                                    legal = 1;
                                }
                                // Try adjusting the position two steps to the left
                                else if (IsLegal(shape, new_form, x - 2, y) == 1) {
                                    temp_x = x - 2;
                                    legal = 1;
                                }
                                // Try adjusting the position two steps to the right
                                else if (IsLegal(shape, new_form, x + 2, y) == 1) {
                                    temp_x = x + 2;
                                    legal = 1;
                                }
                                
                            }
                            if (legal == 1) {
                                x = temp_x;
                                //MoveWhere(shape, form, x, record_space_block_Y);
                                color(shape);
                                DrawSpace(shape, form, x, y);
                                form = new_form;
                                record_space_block_Y = PutWhere(shape, form, x, y);
                            }
                        }
                        break;
                    case ESC:
                        system("cls");
                        color(7);
                        MoveCursor(2 * (COL / 3), 2);
                        printf("Game Over");
                        MoveCursor(COL, 5);
                        exit(0);
                    case 's':
                    case 'S':
                        system("pause>nul");
                        break;
                    case 'r':
                    case 'R':
                        system("cls");
                        main();
                } 
            }
        }
        shape = nextShape, form = nextForm;
        DrawSpace(nextShape, nextForm, COL + 2, 3);
    }
}

void RdGrade() {
    FILE* pf = fopen("tetris_high_score.txt", "r");
    if (pf == NULL) {
        pf = fopen("tetris_high_score.txt", "w");
        fwrite(&grade, sizeof(int), 1, pf);
    }
    fseek(pf, 0, SEEK_SET);
    fread(&max, sizeof(int), 1, pf);
    fclose(pf);
}

void WrGrade() {
    FILE* pf = fopen("tetris_high_score.txt", "w");
    if (pf == NULL) {
        printf("Failed to save high score\n");
        exit(0);
    }
    fwrite(&grade, sizeof(int), 1, pf);
    fclose(pf);
}
