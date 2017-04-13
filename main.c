/*
    Nathaniel Cantwell
    Honors project for ECE 26400
    This project implements sudoku in the C language with the SDL library
*/

#include <stdio.h>
#include <SDL2/SDL.h>

#define ROWS 9
#define COLS 9

void printBoard(int board[][COLS]);

int main(int argv, char*args[]) {
    int mainboard[9][9] = {0};
    printBoard(mainboard);
    return 0;
}

//Prints the sudoku board to the console for debugging
void printBoard(int board[][COLS]) {
    int i, j, k;
    for (i = 0; i < ROWS; i++) {
        if (i % 3 == 0 && i > 0) {
            for (k = 0; k < COLS + 2; k++)
                printf("--");
            printf("\n");
        }
        for (j = 0; j < COLS; j++) {
            if ((j % 3 == 0) && (j > 0))
                printf(" |");
            printf(" %d", board[i][j]);

        }
        printf("\n");
    }
}