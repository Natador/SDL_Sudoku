/*
    Nathaniel Cantwell
    Honors project for ECE 26400
    This project implements sudoku in the C language with the SDL library
*/

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define ROWS 9
#define COLS 9
#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 720

//Global variables to be universally accessed
//	Variables that start with 'my' are globally accessible.
SDL_Window* myWindow = NULL;	//The window we'll use
SDL_Renderer* myRender = NULL;	//The window renderer

bool loadNums(int board[][COLS], char * filename);
void printBoard(int board[][COLS]);
bool initSDL();
void closeSDL();

int main(int argv, char*args[]) {
    //Sudoku board, stored as a 2d array
	int mainboard[9][9] = {0};

	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;

	//Load the numbers from newboard.txt into the array
	if (!loadNums(mainboard, "newboard.txt")) {
		printf("Error loading numbers.\n");
		quit = 1;
	}

    if (!initSDL()) {
		printf("Error! Failed to initialize!\n");
	}
	else {
		//Main loop
		while (!quit) {
			while (SDL_PollEvent(&e) != 0) {
				//If the application is closed
				if (e.type == SDL_QUIT)
					quit = true;
			}

			//Clear the screen
			SDL_SetRenderDrawColor(myRender, 255, 255, 255, 255);
			SDL_RenderClear(myRender);

			//Draw a black grid
			SDL_SetRenderDrawColor(myRender, 0, 0, 0, 0xFF);

			//Draw vertical lines
			for (int i = 0; i <= 9; i++) {
				SDL_RenderDrawLine(myRender, SCREEN_WIDTH * i / 9, 0, SCREEN_WIDTH * i / 9, SCREEN_HEIGHT);
			}

			for (int i = 0; i <= 9; i++) {
				SDL_RenderDrawLine(myRender, 0, SCREEN_HEIGHT * i / 9, SCREEN_WIDTH, SCREEN_HEIGHT * i / 9);
			}


			//Render red filled square
			SDL_Rect fillRect = { SCREEN_WIDTH * 6 / 9 + 1, SCREEN_HEIGHT * 0 / 9 + 1, SCREEN_WIDTH / 9 - 1, SCREEN_HEIGHT / 9 - 1 };
			SDL_SetRenderDrawColor(myRender, 90, 152, 252, 255);
			SDL_RenderFillRect(myRender, &fillRect);

			//Update the screen
			SDL_RenderPresent(myRender);
		}
	}

	closeSDL();
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

//Loads numbers from a text file into the board
bool loadNums(int board[][COLS], char * filename) {
	FILE * fp;
	int i, j;
	char ch;
	bool success = true;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Error opening file %s\n", filename);
		success = false;
	}
	else {
		for (i = 0; i < 9; i++) {
			for (j = 0; j < 9; j++) {
				if (fscanf(fp, "%d", &board[i][j]) != 1) {
					printf("Error reading number in file!\n");
					success = false;
					break;
				}
			}
			ch = getc(fp);
			if (ch == EOF);
			else if (ch != '\n') { 
				printf("Error reading sudoku board from file!\n");
				success = false;
				break;
			}
		}
	}
	if ( fclose(fp) ) {
		printf("Error closing file %s!\n", filename);
		success = 0;
	}
	return success;
}

//Initializes SDL and crap
bool initSDL() {
	bool success = true; //Init flag

	//Initalize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL failed to initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		//Create the window
		myWindow = SDL_CreateWindow( "Sudoku Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

		if (myWindow == NULL) {
			printf("Window couldn't be created! SDL error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			//Make the window renderer
			myRender = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED);
			if (myRender == NULL) {
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else {
				//Initialize the renderer color
				SDL_SetRenderDrawColor(myRender, 0xFF, 0xFF, 0xFF, 0xFF);

				/*
				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if ( !(IMG_INIT(imgFlags) & imgFlags) ) {
					printf("SDL_image failed to initialize! SDL error %s\n", IMG_GetError());
					success = false;
				}
				*/
			
			}
		}
	}
	return success;
}

//Deallocates memory for textures and closes SDL subsystems
void closeSDL() {
	SDL_DestroyRenderer(myRender);
	SDL_DestroyWindow(myWindow);
	myWindow = NULL;
	myRender = NULL;

	//Quit SDL subsystems
	//IMG_Quit();
	SDL_Quit();
}
