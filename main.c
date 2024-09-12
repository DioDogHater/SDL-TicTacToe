#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#undef main
#define true 1
#define false 0


//Screen dimension constants
unsigned const int SCREEN_WIDTH = 480;
unsigned const int SCREEN_HEIGHT = 520;

unsigned const int GRID_SCALE = 480/3;

//The window we'll be rendering to
SDL_Window* Window = NULL;

//The surface contained by the window
SDL_Surface* WindowSurface = NULL;

//Setup renderer
SDL_Renderer* WindowRenderer;

// Loop condition
unsigned int running = true;

// Game state
unsigned int state = 0;

//Event handler
SDL_Event e;

//Images
SDL_Surface* imageSigma = NULL;
SDL_Surface* imageBeta = NULL;
SDL_Surface* imageWin = NULL;
SDL_Surface* imageLose = NULL;
SDL_Surface* imageTie = NULL;
SDL_Surface* imagePlay = NULL;
unsigned int textState = 0;

//Tic Tac Toe elements
int toes[3][3] = {{0,0,0},{0,0,0},{0,0,0}};
int wins[8][3][2] = {
	{{0,0},{0,1},{0,2}},
	{{2,0},{2,1},{2,2}},
	{{0,0},{1,0},{2,0}},
	{{2,0},{2,1},{2,2}},
	{{1,0},{1,1},{1,2}},
	{{0,1},{1,1},{2,1}},
	{{0,0},{1,1},{2,2}},
	{{2,0},{1,1},{0,2}}
};

//Tic Tac Toe functions
int getToe(int win, int x){
	return toes[wins[win][x][0]][wins[win][x][1]];
}

void setToe(int win, int x, int val){
	toes[wins[win][x][0]][wins[win][x][1]] = val;
}

int getWinner(){
	for(int win = 0; win < 8; win++){
		int startVal = getToe(win,0);
		for(int x = 1; x < 3; x++){
			if(getToe(win,x) != startVal){break;}
			if(x == 2){return startVal;}
		}
	}
	return 0;
}

int getWinProb(int win, int porc){
	unsigned int quantity = 0;
	unsigned int enemyQuant = 0;
	for(int x = 0; x < 3; x++){
		if(getToe(win,x) == porc){quantity++;}
		if(getToe(win,x) == -porc){enemyQuant++;}
	}
	if(quantity-enemyQuant < 0){return 0;}
	return quantity-enemyQuant;
}

int boardFull(){
	int total = 0;
	for(int y = 0; y < 3; y++){
		for(int x = 0; x < 3; x++){
			if(toes[y][x] != 0){total++;}
		}
	}
	if(total == 9){return true;}
	return false;
}

void doSomething(){
	if(boardFull() == true){return;}
	for(int win = 0; win < 8; win++){
		// Check for any possible wins from the player and stop it
		if(getWinProb(win,1) == 2){
			for(int x = 0; x < 3; x++){
				if(getToe(win,x) == 0){
					setToe(win,x,-1);
					return;
				}
			}
		}int cpuWinProb = getWinProb(win,-1);
		// Check for any possible wins for the cpu and take it
		if(cpuWinProb == 2){
			for(int x = 0; x < 3; x++){
				if(getToe(win,x) == 0){
					setToe(win,x,-1);
					return;
				}
			}
		}// Check for any possible moves that might help the cpu
		if(cpuWinProb == 1){
			for(int x = 0; x < 3; x++){
				if(getToe(win,x) == 0){
					setToe(win,x,-1);
					return;
				}
			}
		}
	}
	rand();
	int randWin = rand()%8;
	while(getWinProb(randWin,1) != 0){randWin = rand()%8;}
	int randMove = rand()%3;
	while(getToe(randWin,randMove) != 0){randMove = rand()%3;}
	setToe(randWin,randMove,-1);
	return;
}

void resetToes(){
	for(int y = 0; y < 3; y++){
		for(int x = 0; x < 3; x++){
			toes[y][x] = 0;
		}
	}
}

int checkWin(){
	int currWin = getWinner();
	if(currWin != 0){
		return currWin;
	}else if(boardFull() == true){
		return 0;
	}
	return -2;
}

void drawToes(){
	for(int y = 0; y < 3; y++){
		for(int x = 0; x < 3; x++){
			if(toes[y][x] == -1){
				SDL_Rect imageBetaRect;
				imageBetaRect.x = x*GRID_SCALE;
				imageBetaRect.y = y*GRID_SCALE;
				imageBetaRect.w = GRID_SCALE;
				imageBetaRect.h = GRID_SCALE;
				SDL_BlitScaled(imageBeta, NULL, WindowSurface, &imageBetaRect);
			}else if(toes[y][x] == 1){
				SDL_Rect imageSigmaRect;
				imageSigmaRect.x = x*GRID_SCALE;
				imageSigmaRect.y = y*GRID_SCALE;
				imageSigmaRect.w = GRID_SCALE;
				imageSigmaRect.h = GRID_SCALE;
				SDL_BlitScaled(imageSigma, NULL, WindowSurface, &imageSigmaRect);
			}
		}
	}
}

int init(){
    //Initialization flag
    int success = true;
    //Initialize SDL
    if( SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL could not initialize! SDL_Error: %s\n",SDL_GetError());
        success = false;
    }else{
        //Create window
        Window = SDL_CreateWindow("SDL Tutorial",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
        if( Window == NULL ){
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            success = false;
        }else{
			//Create Renderer
			WindowRenderer = SDL_CreateRenderer(Window,-1,SDL_RENDERER_ACCELERATED);
			if(WindowRenderer == NULL){
				printf("Renderer could not be created! SDL error: %s\n",SDL_GetError());
				success = false;
			}else{
				SDL_SetRenderDrawColor(WindowRenderer,0xFF,0xFF,0xFF,0xFF);
			}
            //Get window surface
            WindowSurface = SDL_GetWindowSurface(Window);
        }
    }//Return if operation successful
    return success;
}

SDL_Surface* loadImage(char *filePath){
	SDL_Surface* optimizedSurface = NULL;
	SDL_Surface* loadedSurface = SDL_LoadBMP(filePath);
	if(loadedSurface == NULL){
		printf("unable to load image %s! SDL error: %s\n", filePath, SDL_GetError());
		return NULL;
	}else{
		optimizedSurface = SDL_ConvertSurface(loadedSurface,WindowSurface->format,0);
		if(optimizedSurface == NULL){
			printf("unable to optimize image %s! SDL error: %s\n", filePath, SDL_GetError());
			return NULL;
		}
		SDL_FreeSurface(loadedSurface);
	}
	
	printf("Loaded successfully image %s!\n",filePath);
	return optimizedSurface;
}

void quit(){
	//Destroy images
	SDL_FreeSurface(imageSigma);
	imageSigma = NULL;
	SDL_FreeSurface(imageBeta);
	imageBeta = NULL;
	SDL_FreeSurface(imagePlay);
	imagePlay = NULL;
	SDL_FreeSurface(imageLose);
	imageLose = NULL;
	SDL_FreeSurface(imageWin);
	imageWin = NULL;
	SDL_FreeSurface(imageTie);
	imageTie = NULL;
	
    //Destroy window
	SDL_DestroyRenderer(WindowRenderer);
    SDL_DestroyWindow( Window );
    Window = NULL;
	WindowRenderer = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}

void drawText(){
	SDL_Rect textRect;
	textRect.x = 0; textRect.y = 480;
	textRect.w = 0; textRect.h = 0;
	if(textState == 0){
		SDL_BlitSurface(imagePlay, NULL, WindowSurface, &textRect);
	}else if(textState == 1){
		SDL_BlitSurface(imageWin, NULL, WindowSurface, &textRect);
	}else if(textState == 2){
		SDL_BlitSurface(imageLose, NULL, WindowSurface, &textRect);
	}else if(textState == 3){
		SDL_BlitSurface(imageTie, NULL, WindowSurface, &textRect);
	}
}

void render(){
    drawToes();
	drawText();
}

int ifWin(){
	int winCon = checkWin();
	if(winCon != -2){
		state = 1;
		if(winCon == 0){
			textState = 3;
		}else if(winCon == 1){
			textState = 1;
		}else{
			textState = 2;
		}
		return true;
	}
	return false;
}

int main(int argv, char* args[]){
    //Start up SDL and create window
    if(!init()){
        printf("Failed to initialize!\n");
		return -1;
    }
	
	imageSigma = loadImage("assets/X.bmp");
	imageBeta = loadImage("assets/O.bmp");
	imagePlay = loadImage("assets/Play.bmp");
	imageTie = loadImage("assets/Tie.bmp");
	imageLose = loadImage("assets/Lose.bmp");
	imageWin = loadImage("assets/Win.bmp");

    while(running){
        // Handle events
        while(SDL_PollEvent(&e) != 0){
            if(e.type == SDL_QUIT){
                running = false;
            }
			if(e.type == SDL_MOUSEBUTTONDOWN){
				if(state == 1){
					state = 0; textState = 0; resetToes();
				}else{
					int mouseX, mouseY;
					SDL_GetMouseState(&mouseX,&mouseY);
					if(mouseY > 480){continue;}
					unsigned int gridX = abs(mouseX/GRID_SCALE);
					unsigned int gridY = abs(mouseY/GRID_SCALE);
					if(toes[gridY][gridX] == 0){
						if(textState == 0){textState = 4;}
						toes[gridY][gridX] = 1;
						if(ifWin()){continue;}
						doSomething();
						if(ifWin()){continue;}
					}
				}
			}
        }
		// Clear screen
		SDL_RenderClear(WindowRenderer);
		SDL_FillRect(WindowSurface, NULL, SDL_MapRGB(WindowSurface->format,0,0,0));
		
		// Render current game
		render();
		
		// Update Window visuals
		SDL_UpdateWindowSurface(Window);
    }

    quit();
    return 0;
}
