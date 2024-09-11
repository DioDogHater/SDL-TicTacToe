#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include <windows.h>

#undef main
#define true 1
#define false 0


//Screen dimension constants
unsigned const int SCREEN_WIDTH = 480;
unsigned const int SCREEN_HEIGHT = 480;

unsigned const int GRID_SCALE = 480/3;

//The window we'll be rendering to
SDL_Window* Window = NULL;

//The surface contained by the window
SDL_Surface* WindowSurface = NULL;

//Setup renderer
SDL_Renderer* WindowRenderer;

// Loop condition
int running = true;
//Event handler
SDL_Event e;

//Images
SDL_Surface* imageSigma = NULL;
SDL_Surface* imageBeta = NULL;

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
		return true;
	}else if(boardFull() == true){
		return true;
	}
	return false;
}

void drawToes(){
	for(int y = 0; y < 3; y++){
		for(int x = 0; x < 3; x++){
			if(toes[y][x] == 1){
				SDL_Rect imageBetaRect;
				imageBetaRect.x = x*GRID_SCALE;
				imageBetaRect.y = y*GRID_SCALE;
				imageBetaRect.w = GRID_SCALE;
				imageBetaRect.h = GRID_SCALE;
				SDL_BlitScaled(imageBeta, NULL, WindowSurface, &imageBetaRect);
			}else if(toes[y][x] == -1){
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
	
    //Destroy window
	SDL_DestroyRenderer(WindowRenderer);
    SDL_DestroyWindow( Window );
    Window = NULL;
	WindowRenderer = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}

void render(){
    return;
}

int main(int argv, char* args[]){
    //Start up SDL and create window
    if( !init() ){
        printf( "Failed to initialize!\n" ); return -1;
    }
	
	imageSigma = loadImage("assets/X.bmp");
	imageBeta = loadImage("assets/O.bmp");

    while(running){
        // Handle events
        while(SDL_PollEvent(&e) != 0){
            if(e.type == SDL_QUIT){
                running = false;
            }if(e.type == SDL_MOUSEBUTTONDOWN){
				int mouseX, mouseY;
				SDL_GetMouseState(&mouseX,&mouseY);
				unsigned int gridX = abs(mouseX/GRID_SCALE);
				unsigned int gridY = abs(mouseY/GRID_SCALE);
				if(toes[gridY][gridX] == 0){
					toes[gridY][gridX] = -1;
				}
			}
        }
		// Clear screen
		SDL_RenderClear(WindowRenderer);
		
		// Render current game
		drawToes();
		
		// Update Window visuals
		SDL_UpdateWindowSurface(Window);
    }

    quit();
    return 0;
}
