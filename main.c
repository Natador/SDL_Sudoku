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

//Tile datatype. Contains the sudoku value, state of the square, rectangular position and size, and the texture
typedef struct {
	int value;		//numeric value stored in the tile
	bool isInitial;	//Was this initially loaded? Can it be modified?
	bool isActive;	//Is the tile active, i.e. was it clicked?
	bool isError;	//Is the tile correct? Is there a game rule conflict?
	bool changed;	//Change flag for updating the number texture
	SDL_Rect square;	//Rectangle containing position and size of each tile
	SDL_Texture * numTexture;	//Pointer to the texture for the number in the tile
} tile;

//Global variables to be universally accessed
//	Variables that start with 'my' are globally accessible.
SDL_Window * myWindow = NULL;	//The window we'll use
SDL_Renderer * myRender = NULL;	//The window renderer

//Function prototypes. Descriptions are listed with each function.
//Initialization functions
bool loadNums(int board[][COLS], char * filename);
bool initSDL();
void closeSDL(tile board[][COLS]);
bool initBoard(tile board[][COLS], TTF_Font * fontFam);
SDL_Texture * loadString(char * message, TTF_Font * font);
SDL_Texture * numToText(int num, TTF_Font * font);

//Rendering functions
void renderNums(tile board[][COLS], TTF_Font * font);
void renderColors(tile board[][COLS]);
void drawGrid(void);

//Main game functions
void getGridPos(int x, int y, int * i, int * j);
void clearActiveStates(tile board[][COLS]);
void checkMove(tile board[][COLS], int row, int col);

//Debugging functions
void printBoard(int board[][COLS]);

int main(int argc, char * argv[]) {
    //Sudoku board, stored as a 2d array of tile structs.
	tile mainboard[ROWS][COLS];

	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;
	
	//TTF font to be used for rendering text and numbers
	TTF_Font *myFont = NULL;

    if (!initSDL()) {
		printf("Error! Failed to initialize!\n");
	}
	else {
		//Index variables for mouse and keyboard events.
		int i = 100, j = 100;
		
		//Flag for an active tile.
		bool tileActive = false;

		//Loading a specified font to the TTF_Font variable.
		myFont = TTF_OpenFont("fonts/OpenSans-Regular.ttf", 48);

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
					
					//Gets the x and y mouse position
					SDL_GetMouseState(&x, &y);

					//Turns the mouse position into grid indicies.
					getGridPos(x, y, &i, &j);

					//Sets all tiles to inactive
					clearActiveStates(mainboard);

					//Changes the current tile to the active state so it is highlighted later
					if (e.button.button == SDL_BUTTON_LEFT){
						mainboard[i][j].isActive = true;
						tileActive = true;
					}
					//Right click causes the tile to become inactive
					else {
						if (mainboard[i][j].isActive) {
							mainboard[i][j].isActive = false;
						}
						tileActive = false;
					}
					if (tileActive) {
						checkMove(mainboard, i, j);
					}
				}
				//A key was pressed and a previous tile was selected
				else if (e.type == SDL_KEYDOWN && tileActive) {
					//Change number based on key press
					for (int k = 0; k < 10; k++) {
						//If the key is a numeral
						if ( (e.key.keysym.sym == SDLK_0 + k) || (e.key.keysym.sym == SDLK_DELETE) || (e.key.keysym.sym == SDLK_SPACE) || (e.key.keysym.sym == SDLK_BACKSPACE) ) {
							//If it is not an initial state
							if (mainboard[i][j].isInitial == false) {
								mainboard[i][j].value = k;

								//Update flag for renderNums
								mainboard[i][j].changed = true;

								//Check to see if the move is valid and set the state of the tile accordingly
								checkMove(mainboard, i, j);
								break;
							}
						}
					}
				}
			}

			//Clear the screen
			SDL_SetRenderDrawColor(myRender, 255, 255, 255, 255);
			SDL_RenderClear(myRender);

			//Draw the grid
			drawGrid();

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
	FILE * fp = NULL;
	int i, j;
	char ch;
	bool success = true;

	//Open the file
	fp = fopen(filename, "r");

	//If the file was not opened
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

	//Close the file.
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
			if (board[i][j].numTexture != NULL) {
				SDL_DestroyTexture(board[i][j].numTexture);
				board[i][j].numTexture = NULL;
			}
		}
	}
	
	SDL_DestroyRenderer(myRender);
	SDL_DestroyWindow(myWindow);
	myWindow = NULL;
	myRender = NULL;

	//Quit SDL subsystems
	//IMG_Quit();
	TTF_Quit();
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
	if (!loadNums(boardVals, "boards/newboard.txt")) {
		printf("Error loading numbers from textfile!\n");
		success = false;
	}
	else {
		printBoard(boardVals);
		//Iterates through each tile in the struct array.
		for (i = 0; i < ROWS; i++) {
			for (j = 0; j < COLS; j++) {
				//Loading the values into the structure.
				board[i][j].value = boardVals[i][j];

				//Sets the default active state of the tile to false.
				board[i][j].isActive = false;

				//Sets the default error state to false.
				board[i][j].isError = false;

				//Sets changed state to false because they aren't changed.
				board[i][j].changed = false;

				//If the value is nonzero, set the texture
				if(board[i][j].value) {
					//Sets initial state to true so it cannot be modified
					board[i][j].isInitial = true;
				}
				else {
					//Sets initial state
					board[i][j].isInitial = false;
				}

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
				//If the number at that square is a nonzero value.
				if (board[i][j].value) {
					//If the value was changed by an event
					if (board[i][j].changed) {
						//Destroy the old texture
						SDL_DestroyTexture(board[i][j].numTexture);

						//Make a new texture
						board[i][j].numTexture = numToText(board[i][j].value, font);

						//The texture has been updated, so it is no longer in need of change.
						board[i][j].changed = false;
					}
					//Render the texture to the back buffer in the correct rect.
					SDL_RenderCopy(myRender, board[i][j].numTexture, NULL, &(board[i][j].square));
				}
			}
		}
}

//Draws the black grid on the board
void drawGrid(void) {
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

//Fills the color of each tile with an appropriate cbolor based on its state.
void renderColors(tile board[][COLS]) {
	int i, j;	//Indexing variables
	
	//Loops through the board and fills the color of the rect based on the tile's state.
	for (i = 0; i < ROWS; i++) {
		for (j = 0; j < COLS; j++) {
			//If the tile was an initial tile, fill blue
			if (board[i][j].isInitial) {
				SDL_SetRenderDrawColor(myRender, 130, 177, 255, 255);
				SDL_RenderFillRect(myRender, &(board[i][j].square));
			}
			//If the tile is active, fill yellowish
			else if (board[i][j].isActive) {
				if (board[i][j].isError) {
					SDL_SetRenderDrawColor(myRender, 232, 130, 99, 255);
				}
				else {
					SDL_SetRenderDrawColor(myRender, 232, 214, 99, 255);
				}
				SDL_RenderFillRect(myRender, &(board[i][j].square));
			}
			//If the tile is an error, fill redish
			else if (board[i][j].isError) {
				SDL_SetRenderDrawColor(myRender, 237, 87, 87, 255);
				SDL_RenderFillRect(myRender, &(board[i][j].square));
			}
			//No else condition because the renderer automatically clears all rects to white each loop.
		}
	}
}

//Clears all active states on the board.
void clearActiveStates(tile board[][COLS]) {
	int i, j;
	for (i = 0; i < ROWS; i++) {
		for (j = 0; j < COLS; j++){
			board[i][j].isActive = false;
		}
	}
}

//Checks in the correct row, column, and subsection to see if the digit is repeated.
//	Flags the tile at [row][col] as INCORRECT if so
void checkMove(tile board[][COLS], int row, int col) {
	int i, j, subrow, subcol;
	bool isFine = true;

	//Checking if the value is nonzero
	if (board[row][col].value) {
		//Checking the values within the row of the given square
		for (j = 0; j < COLS; j++) {
			if (board[row][j].value == board[row][col].value && j != col) {
				board[row][col].isError = true;
				printf("Row confliction!\n");
				isFine = false;
			}
		}

		//Checking the values within the column of the given square
		for (i = 0; i < ROWS; i++) {
			if (board[i][col].value == board[row][col].value && i != row) {
				board[row][col].isError = true;
				printf("Column confliction!\n");
				isFine = false;
			}
		}

		//Checking the subsquare which contains the [row][col] value.
		subrow = row / 3;
		subcol = col / 3;
		for (i = 0; i < 3; i++) {
			for (j = 0; j < 3; j++) {
				if (board[row][col].value == board[subrow*3 + i][subcol*3 + j].value && (subrow*3 + i != row) && (subcol*3 + j != col)) {
					board[row][col].isError = true;
					printf("Subsquare confliction!\n");
					isFine = false;
				}				
			}
		}
	}
	//If there were no errors, reset the error state to false.
	//Useful when correcting an error in the board.
	if (isFine) {
		board[row][col].isError = false;
	}
}
