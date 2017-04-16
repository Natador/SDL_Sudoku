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

enum states {
	INACTIVE,
	ACTIVE,
	INCORRECT
};

//Tile datatype. Contains the sudoku value, state of the square, rectangular position and size, and the texture
typedef struct {
	int value;
	int state;
	bool initial;
	bool changed;
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
void closeSDL(tile board[][COLS]);
bool initBoard(tile board[][COLS], TTF_Font * fontFam);
void renderNums(tile board[][COLS], TTF_Font * font);
SDL_Texture * loadString(char * message, TTF_Font * font);
SDL_Texture * numToText(int num, TTF_Font * font);
void getGridPos(int x, int y, int * i, int * j);
void renderColors(tile board[][COLS]);
void clearStates(tile board[][COLS]);

int main(int argc, char*argv[]) {
    //Sudoku board, stored as a 2d array of tile structs.
	tile mainboard[ROWS][COLS];

	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;
	
	//TTF font to be used for rendering text and numbers
	TTF_Font *myFont = NULL;
	
	printf("%d\n%d\n%d\n", SDLK_0, SDLK_1, SDLK_2);

    if (!initSDL()) {
		printf("Error! Failed to initialize!\n");
	}
	else {
		int i = 100, j = 100;
		//Loading a specified font to the TTF_Font variable.
		myFont = TTF_OpenFont("OpenSans-Regular.ttf", 48);

		if (myFont == NULL) {
			printf("Font could not be located! TTF error: %s", TTF_GetError());
			quit = true;
		}
		if ( !initBoard(mainboard, myFont) ) {
			printf("Error! Failed to initialize board!\n");
			quit = true;
		}

		//Main loop
		while (!quit) {
			//Polling for events in the event buffer
			while (SDL_PollEvent(&e) != 0) {
				//If the application is closed
				if (e.type == SDL_QUIT)
					quit = true;
				//If a mousekey is clicked
				else if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
					int x, y;
					SDL_GetMouseState(&x, &y);
					getGridPos(x, y, &i, &j);
					clearStates(mainboard);
					if (e.button.button == SDL_BUTTON_LEFT)
						mainboard[i][j].state = ACTIVE;
				}
				else if (e.type == SDL_KEYDOWN) {
					//Change number based on key press
					for (int k = 0; k < 10; k++) {
						if (e.key.keysym.sym == SDLK_0 + k) {
								mainboard[i][j].value = k;
								mainboard[i][j].changed = true;
								break;
						}
					}
				}
			}

			//Clear the screen
			SDL_SetRenderDrawColor(myRender, 255, 255, 255, 255);
			SDL_RenderClear(myRender);

			//Draw a black grid. Fancy math keeps the gridlines square even if the window is rectangular.
			SDL_SetRenderDrawColor(myRender, 0, 0, 0, 0xFF);
			//Draw vertical lines
			for (int i = 0; i <= 9; i++) {
				SDL_RenderDrawLine(myRender, (SCREEN_WIDTH - SCREEN_HEIGHT)/2 + SCREEN_HEIGHT * i / 9, 0, (SCREEN_WIDTH - SCREEN_HEIGHT)/2 + SCREEN_HEIGHT * i / 9, SCREEN_HEIGHT);
			}
			//Horrizontal lines
			for (int i = 0; i <= 9; i++) {
				SDL_RenderDrawLine(myRender, (SCREEN_WIDTH - SCREEN_HEIGHT) / 2, SCREEN_HEIGHT * i / 9, (SCREEN_WIDTH - SCREEN_HEIGHT) / 2 + SCREEN_HEIGHT, SCREEN_HEIGHT * i / 9);
			}

			//Renders the colors of each tile based on its state
			renderColors(mainboard);

			//Render the current board state
			renderNums(mainboard, myFont);

			//Update the screen
			SDL_RenderPresent(myRender);
		}
	}

	//SDL_DestroyTexture(testText);
	closeSDL(mainboard);
	
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
void closeSDL(tile board[][COLS]) {
	//Deallocate textures
	int i, j;
	for (i = 0; i < ROWS; i++) {
		for (j = 0; j < COLS; j++) {
			SDL_DestroyTexture(board[i][j].numTexture);
			board[i][j].numTexture = NULL;
		}
	}
	
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
				//Loading the values into the structure.
				board[i][j].value = boardVals[i][j];

				//Sets the default state to inactive
				board[i][j].state = INACTIVE;

				//Sets initial state
				board[i][j].initial = true;
				
				//Sets changed state
				board[i][j].changed = true;

				//Sets texture
				board[i][j].numTexture = numToText(board[i][j].value, fontFam);

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

//Updates the board with the tiles
void renderNums(tile board[][COLS], TTF_Font * font) {		
		//Rendering the numbers initially
		for (int i = 0; i < ROWS; i++) {
			for (int j = 0; j < COLS; j++) {
				//If the number at that square is a nonzero value, render the texture.
				if (board[i][j].value) {
					if (board[i][j].changed) {
						SDL_DestroyTexture(board[i][j].numTexture);
						board[i][j].numTexture = numToText(board[i][j].value, font);
						board[i][j].changed = false;
					}
					SDL_RenderCopy(myRender, board[i][j].numTexture, NULL, &(board[i][j].square));
				}
			}
		}
}

//Determines the grid row/column from the x and y position of the mouse
void getGridPos(int x, int y, int * i, int * j) {
	int k;

	//Find the row
	for (k = 0; k < 9; k++) {
		if (y >= (k * SCREEN_HEIGHT / 9) && y < ((k + 1) * SCREEN_HEIGHT / 9) )
			*i = k;
	}

	//Find the column
	for (k = 0; k < 9; k++) {
		if (x >= (k * SCREEN_WIDTH / 9) && x < ((k + 1) * SCREEN_WIDTH / 9) )
			*j = k;
	}
}

//Fills the color of each tile with an appropriate color based on its state.
void renderColors(tile board[][COLS]) {
	int i, j;	//Indexing variables
	
	//Loops through the board and fills the color of the rect based on the tile's state.
	for (i = 0; i < ROWS; i++) {
		for (j = 0; j < COLS; j++) {
			switch (board[i][j].state) {
				case ACTIVE:
					SDL_SetRenderDrawColor(myRender, 232, 214, 99, 255);
					SDL_RenderFillRect(myRender, &(board[i][j].square));
					break;
				case INCORRECT:
					SDL_SetRenderDrawColor(myRender, 237, 87, 87, 255);
					SDL_RenderFillRect(myRender, &(board[i][j].square));
					break;
			}
		}
	}
}

//Clears all states on the board
void clearStates(tile board[][COLS]) {
	int i, j;
	for (i = 0; i < ROWS; i++) {
		for (j = 0; j < COLS; j++)
			board[i][j].state = INACTIVE;
	}
}