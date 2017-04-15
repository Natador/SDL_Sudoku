/*
    Nathaniel Cantwell
    Honors project for ECE 26400
    This project implements sudoku in the C language with the SDL library
*/

#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#define ROWS 9
#define COLS 9
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

//Global variables to be universally accessed
SDL_Window* myWindow = NULL;	//The window we'll use
SDL_Renderer* myRender = NULL;	//The window renderer

void printBoard(int board[][COLS]);
bool initSDL();

int main(int argv, char*args[]) {
    int mainboard[9][9] = {0};
	int x, y, n, count = 0;
    printBoard(mainboard);
	while (1) {
		printf("Please enter a move\n");
		if(scanf("%d %d %d", &x, &y, &n) != 3)
				break;
		mainboard[x-1][y-1] = n;
		printBoard(mainboard);
	}
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

//Initializes SDL and crap
bool id initSDL() {
	bool success = true; //Init flag

	//Initalize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		//Create the window
		myWindow = SDL_CreateWindow( "Sudoku Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

		if (myWindow == NULL) {
			printf("Window couldn't be created! SDL error: %s", SDL_GetError());
			success = false;
		}
		else {
			//Make the window renderer
			myRender = SDL_CreateRenderer(myWindow, -1, SDL_RENDER_ACCELERATED);
			if (myRender == NULL) {
				printf("Renderer could not be created! SDL Error: %s", SDL_GetError());
				success = false;
			}
			else {
				//Initialize the renderer color
				SDL_SetRenderDrawColor(myRender, 0xFF, 0xFF, 0xFF, 0xFF)
			}
		}
	}
}
