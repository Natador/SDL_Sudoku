/*
    Nathaniel Cantwell
    Honors project for ECE 26400
    This project implements sudoku in the C language with the SDL library
*/

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
//#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define ROWS 9
#define COLS 9
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 640

typedef struct {
	int value;
	bool initial;
	SDL_Rect square;
	SDL_Texture * numTexture;
} tile;

//Global variables to be universally accessed
//	Variables that start with 'my' are globally accessible.
SDL_Window* myWindow = NULL;	//The window we'll use
SDL_Renderer* myRender = NULL;	//The window renderer

//Function prototypes. Descriptions are listed with each function.
bool loadNums(int board[][COLS], char * filename);
void printBoard(int board[][COLS]);
bool initSDL();
void closeSDL();
bool initBoard(tile board[][COLS], TTF_Font * fontFam);
SDL_Texture * loadString(char * message, TTF_Font * font);
SDL_Texture * numToText(int num, TTF_Font * font);

int main(int argv, char*args[]) {
    //Sudoku board, stored as a 2d array of tile structs.
	tile mainboard[ROWS][COLS];

	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;

	SDL_Texture * testText = NULL;
	
	//TTF font to be used for rendering text and numbers
	TTF_Font *myFont = NULL;
	
    if (!initSDL()) {
		printf("Error! Failed to initialize!\n");
	}
	else {
		//Loading a specified font to the TTF_Font variable.
		myFont = TTF_OpenFont("OpenSans-Regular.ttf", 48);

		if (myFont == NULL) {
			printf("Font could not be located! TTF error: %s", TTF_GetError());
			quit = true;
		}
		else if ( !initBoard(mainboard, myFont) ) {
			printf("Error! Grid failed to initialize!\n");
			quit = true;
		}

		//Main loop
		while (!quit) {
			//Polling for events in the event buffer
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
			//Horrizontal lines
			for (int i = 0; i <= 9; i++) {
				SDL_RenderDrawLine(myRender, 0, SCREEN_HEIGHT * i / 9, SCREEN_WIDTH, SCREEN_HEIGHT * i / 9);
			}
			if ( SDL_RenderCopy(myRender, mainboard[1][1].numTexture, NULL, &(mainboard[1][1].square)) ) {
				printf("Failed to render texture! Error: %s\n", SDL_GetError() );
				quit = true;
			}
			 /*
			for (int i = 0; i < ROWS; i++) {
				for (int j = 0; j < COLS; j++) {
					//If it is an initial, nonzero value.
					if (mainboard[i][j].initial) {
						SDL_RenderCopy(myRender, mainboard[i][j].numTexture, NULL, &(mainboard[i][j].square));
					}
				}
			}
			*/

			/*
			if (testText == NULL) {
				printf("Texture problems, dawg.\n");
				break;
			}
			else {

				//Render red filled square

				SDL_RenderCopy(myRender, testText, NULL, &grid[2][1]);
			}
			*/
			//Update the screen
			SDL_RenderPresent(myRender);
		}
	}

	//SDL_DestroyTexture(testText);
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

//Loads numbers from a text file into a 2d int array.
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
		for (i = 0; i < ROWS; i++) {
			for (j = 0; j < COLS; j++) {
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

				//Initialize SDL_ttf
				if (TTF_Init() == -1) {
					printf("SDL_ttf failed to initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
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

//Returns a texture containing a string with ttf font
SDL_Texture * loadString(char * message, TTF_Font * font) {
	//TTF_Font *myFont = TTF_OpenFont("Sans.ttf", 50);

	//A black color for the text
	SDL_Color Black = { 0, 0, 0 };

	//The texture containing the message which we want to return
	SDL_Texture *textOut = NULL;
	
	//Creating the surface containing the message
	SDL_Surface *surfaceMsg = NULL; 

	surfaceMsg = TTF_RenderText_Solid(font, message, Black);
	if (surfaceMsg == NULL) {
		printf("Surface message could not be loaded! TTF Error: %s\n", TTF_GetError());
	}
	else {
		//Turning the surface with the message into a texture
		textOut = SDL_CreateTextureFromSurface(myRender, surfaceMsg);
		if (textOut == NULL) {
			printf("Texture could not be created.\n");
		}
		SDL_FreeSurface(surfaceMsg);
	}
	return textOut;
}

//Returns a texture containing a number from the given TTF font. Calls loadString.
SDL_Texture * numToText(int num, TTF_Font * font) {
	//String to contain the number. 10 for safety.
	char number[10];

	SDL_Texture * numTextOut = NULL;

	//Converting the integer, num, to a string in 'number'
	if (sprintf(number, "%d", num) < 0) {
		printf("Failed sprintf for some reason.\n");
	}
	else {
		numTextOut = loadString(number, font);
		if (numTextOut == NULL) {
			printf("There were errors in numToText. Error: %s\n", SDL_GetError());
		}
	}
	return numTextOut;
}

//Initializes the 2d array of tile structs with a loaded board, initializes textures as well.
bool initBoard(tile board[][COLS], TTF_Font * fontFam) {
	int i, j;	//Looping variables
	bool success = true; //Flag for successful instantiation
	int boardVals[ROWS][COLS];

	//Loading the numbers from a textfile into boardVals variable.
	if (!loadNums(boardVals, "newboard.txt")) {
		printf("Error loading numbers from textfile!\n");
		success = false;
	}
	else {
		//Iterates through each tile in the struct array.
		for (i = 0; i < ROWS; i++) {
			for (j = 0; j < COLS; j++) {
				//Loading the values into the struct array and determining if it's an initial value
				if ( (board[i][j].value = boardVals[i][j]) == 0 ) {
					board[i][j].initial = true;

					//If it is an initial, nonzero value, load a texture into the member
					board[i][j].numTexture = numToText(board[i][j].value, fontFam);
				}
				else {
					board[i][j].initial = false;
					board[i][j].numTexture = NULL;
				}

				//Initializing the SDL_Rect member in each element
				board[i][j].square.x = SCREEN_WIDTH * j / 9 + 1;
				board[i][j].square.y = SCREEN_HEIGHT * i / 9 + 1;
				board[i][j].square.w = SCREEN_WIDTH / 9 - 1;
				board[i][j].square.h = SCREEN_HEIGHT / 9 - 1;
			}
		}
	}
	return success;
}