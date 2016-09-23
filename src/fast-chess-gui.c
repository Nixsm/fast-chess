/*
 ============================================================================
 Name        : fast-chess-gui.c
 Author      : Frederico Jordan <fredericojordan@gmail.com>
 Version     :
 Copyright   : Copyright (c) 2016 Frederico Jordan
 Description : Graphical user interface for simple chess game!
 ============================================================================
 */

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include "fast-chess.h"

int TILE_SIDE = 60;

char COLOR_SQUEMES[][6] = {
							{ 240, 217, 181, 181, 136,  99 }, // LICHESS
							{ 255, 255, 255, 240, 240, 240 }, // WHITE
							{ 240, 240, 240, 200, 200, 200 }, // LIGHT_GRAY
							{ 164, 164, 164, 136, 136, 136 }, // LICHESS_GRAY
							{ 140, 184, 219,  91, 131, 159 }, // ROYAL BLUE
							{ 255, 255, 255, 140, 184, 219 }, // WHITE/BLUE
							{ 212, 202, 190, 100,  92,  89 }, // CHESSWEBSITE
						  };
int bgColorNum = -1;

SDL_Window* window = NULL; 			// The window we'll be rendering to
SDL_Surface* screenSurface = NULL; 	// The surface contained by the window
SDL_Renderer* renderer = NULL;      // The main renderer

SDL_Texture *bgTexture;
SDL_Texture *bPawn, *bKnight, *bBishop, *bRook, *bQueen, *bKing;
SDL_Texture *wPawn, *wKnight, *wBishop, *wRook, *wQueen, *wKing;

SDL_Texture * loadImage(char * fileLocation) {
	SDL_Surface* imgSurface = NULL;

    imgSurface = IMG_Load( fileLocation );

    if( imgSurface == NULL ) {
        printf( "Unable to load image %s! SDL Error: %s\n", fileLocation, SDL_GetError() );
        return NULL;
    }

    SDL_Texture * dstTexture = SDL_CreateTextureFromSurface(renderer, imgSurface);
    SDL_FreeSurface( imgSurface );

    return dstTexture;
}

void loadImages(void) {
	bPawn   = loadImage("images/black_pawn.png");
	bKnight = loadImage("images/black_knight.png");
	bBishop = loadImage("images/black_bishop.png");
	bRook   = loadImage("images/black_rook.png");
	bQueen  = loadImage("images/black_queen.png");
	bKing   = loadImage("images/black_king.png");

	wPawn   = loadImage("images/white_pawn.png");
	wKnight = loadImage("images/white_knight.png");
	wBishop = loadImage("images/white_bishop.png");
	wRook   = loadImage("images/white_rook.png");
	wQueen  = loadImage("images/white_queen.png");
	wKing   = loadImage("images/white_king.png");
}

void close() {
    SDL_FreeSurface( screenSurface );
    screenSurface = NULL;

    SDL_DestroyWindow( window );
    window = NULL;
    SDL_Quit();
}

SDL_Rect index2rect(int index) {
	int file = index%8;
	int rank = index/8;

	SDL_Rect tile;
	tile.x = file*TILE_SIDE;
	tile.y = (7-rank)*TILE_SIDE;
	tile.w = TILE_SIDE;
	tile.h = TILE_SIDE;

	return tile;
}

int xy2index(int x, int y) {
	int file = (int) (x/TILE_SIDE);
	int rank = (int) (7 - (y/TILE_SIDE));
	return 8*rank + file;
}

void int2str(int index) {
	int file = index%8;
	int rank = index/8;

	printf("%c%c", FILES[file], RANKS[rank]);
	fflush(stdout);
}

void xy2str(int x, int y){
	int2str(xy2index(x, y));
}

void paintTile(SDL_Surface * destSurface, int position, char color[]) {
	SDL_Rect tile = index2rect(position);
	SDL_FillRect( destSurface, &tile, SDL_MapRGB( destSurface->format, color[0], color[1], color[2] ) );
}

void paintBoard(SDL_Surface * destSurface, char colors[]) {
	SDL_FillRect( destSurface, NULL, SDL_MapRGB( destSurface->format, colors[0], colors[1], colors[2] ) );

	int i;
	for ( i=0; i<NUM_SQUARES; i++)
		if ( index2bb(i) & DARK_SQUARES )
			paintTile(destSurface, i, &colors[3]);
}

void renderBoard(int board[]) {
	SDL_Rect boardRect;
	boardRect.x = 0;
	boardRect.y = 0;
	boardRect.w = 8*TILE_SIDE;
	boardRect.h = 8*TILE_SIDE;
	SDL_RenderCopy(renderer, bgTexture, NULL, &boardRect);

	int i;
	for (i=0; i<NUM_SQUARES; i++) {
		int piece = board[i];

		if ( piece != EMPTY ) {
			SDL_Rect squareRect = index2rect(i);

			switch(piece) {
			case BLACK|PAWN:
				SDL_RenderCopy(renderer, bPawn, NULL, &squareRect);
				break;

			case BLACK|KNIGHT:
				SDL_RenderCopy(renderer, bKnight, NULL, &squareRect);
				break;

			case BLACK|BISHOP:
				SDL_RenderCopy(renderer, bBishop, NULL, &squareRect);
				break;

			case BLACK|ROOK:
				SDL_RenderCopy(renderer, bRook, NULL, &squareRect);
				break;

			case BLACK|QUEEN:
				SDL_RenderCopy(renderer, bQueen, NULL, &squareRect);
				break;

			case BLACK|KING:
				SDL_RenderCopy(renderer, bKing, NULL, &squareRect);
				break;

			case WHITE|PAWN:
				SDL_RenderCopy(renderer, wPawn, NULL, &squareRect);
				break;

			case WHITE|KNIGHT:
				SDL_RenderCopy(renderer, wKnight, NULL, &squareRect);
				break;

			case WHITE|BISHOP:
				SDL_RenderCopy(renderer, wBishop, NULL, &squareRect);
				break;

			case WHITE|ROOK:
				SDL_RenderCopy(renderer, wRook, NULL, &squareRect);
				break;

			case WHITE|QUEEN:
				SDL_RenderCopy(renderer, wQueen, NULL, &squareRect);
				break;

			case WHITE|KING:
				SDL_RenderCopy(renderer, wKing, NULL, &squareRect);
				break;
			}
		}
	}

	SDL_RenderPresent(renderer);
}

void loadBackground(void) {
	const SDL_PixelFormat fmt = *(screenSurface->format);
	SDL_Surface * bgSurface = SDL_CreateRGBSurface(0, 8*TILE_SIDE, 8*TILE_SIDE, fmt.BitsPerPixel, fmt.Rmask, fmt.Gmask, fmt.Bmask, fmt.Amask );
	paintBoard(bgSurface, COLOR_SQUEMES[bgColorNum]);
	bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
	SDL_FreeSurface( bgSurface );
}

void changeColors() {
	int colorCount = (int) (sizeof(COLOR_SQUEMES)/6);
	int newColor = rand() % colorCount;

	while ( newColor == bgColorNum ) { newColor = rand() % colorCount; }

	bgColorNum = newColor;
	loadBackground();
}

BOOL init() {
	srand(time(NULL));

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return FALSE;
    }

	window = SDL_CreateWindow( "Chess Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 8*TILE_SIDE, 8*TILE_SIDE, SDL_WINDOW_SHOWN );

	if( window == NULL ) {
		printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		return FALSE;
	}

	int imgFlags = IMG_INIT_PNG;

	if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
		printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
		return FALSE;
	}

	screenSurface = SDL_GetWindowSurface( window );
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	changeColors();
	loadBackground();
	loadImages();

    return TRUE;
}

void setEndTitle(Game game) {
	if (isCheckmate(game)) {
		if (game.toMove == BLACK)
			SDL_SetWindowTitle(window, "Chess Game - WHITE wins!");
		else
			SDL_SetWindowTitle(window, "Chess Game - BLACK wins!");
	}
	if (isStalemate(game))
		SDL_SetWindowTitle(window, "Chess Game - Draw by stalemate!");
	if (hasInsufficientMaterial(game.board))
		SDL_SetWindowTitle(window, "Chess Game - Draw by insufficient material!!");
	if (isOver75MovesRule(game))
		SDL_SetWindowTitle(window, "Chess Game - Draw by 75-move rule!");
}

void playAs(char color, int AIdepth) {
	printf("Playing as %s!\n", color==WHITE?"WHITE":"BLACK");
	fflush(stdout);

	Game game = getInitialGame();
	renderBoard(game.board);

	BOOL run = TRUE, ongoing = TRUE;
	SDL_Event event;

	int leavingPos = -1, arrivingPos = -1;

	while( run ) {
		renderBoard(game.board);

		if ( hasGameEnded(game) ) {
			ongoing = FALSE;
			setEndTitle(game);
		}

		if ( ongoing && game.toMove == opposingColor(color) ) {
			SDL_SetWindowTitle(window, "Chess Game - Calculating move...");
			game = makeMove(game, getAIMove(game, AIdepth));
			SDL_SetWindowTitle(window, "Chess Game");
			renderBoard(game.board);
		}


		while( SDL_PollEvent( &event ) != 0 ) {

			switch (event.type) {
			case SDL_QUIT:
				run = FALSE;
				break;

			case SDL_MOUSEMOTION:
				break;

			case SDL_MOUSEBUTTONDOWN:
				leavingPos = xy2index(event.motion.x, event.motion.y);
				break;

			case SDL_MOUSEBUTTONUP:
				arrivingPos = xy2index(event.motion.x, event.motion.y);

				if ( ongoing && game.toMove == color ) {
					Move moves[MOVE_BUFFER_SIZE];
					int moveCount = legalMoves(moves, game, game.toMove);

					int i;
					for (i=0; i<moveCount; i++)
						if (generateMove(leavingPos, arrivingPos) == moves[i]) {
							game = makeMove(game, moves[i]);
							renderBoard(game.board);
						}
				}
				break;

			case SDL_KEYDOWN:
				switch( event.key.keysym.sym ) {
				case SDLK_q:
					run = FALSE;
					break;

				case SDLK_c:
					changeColors();
					renderBoard(game.board);
					break;

				default:
					printf("User pressed key: '%s' key acting as '%s' key\n",
							SDL_GetScancodeName(event.key.keysym.scancode), SDL_GetKeyName(event.key.keysym.sym));
					fflush(stdout);
					break;
				}
				break;
			}
		}
	}
}

void playRandomColor(int depth) {
	char colors[] = {WHITE, BLACK};
	char color = colors[rand()%2];
	playAs(color, depth);
}

void playAlone() {
	Game game = getInitialGame();
	renderBoard(game.board);

	BOOL run = TRUE, ongoing = TRUE;
	SDL_Event event;

	int leavingPos = -1, arrivingPos = -1;

	while( run ) {
		renderBoard(game.board);

		if ( hasGameEnded(game) ) {
			ongoing = FALSE;
			setEndTitle(game);
		}

		while( SDL_PollEvent( &event ) != 0 ) {

			switch (event.type) {
			case SDL_QUIT:
				run = FALSE;
				break;

			case SDL_MOUSEMOTION:
				break;

			case SDL_MOUSEBUTTONDOWN:
				leavingPos = xy2index(event.motion.x, event.motion.y);
				break;

			case SDL_MOUSEBUTTONUP:
				arrivingPos = xy2index(event.motion.x, event.motion.y);

				if ( ongoing ) {
					Move moves[MOVE_BUFFER_SIZE];
					int moveCount = legalMoves(moves, game, game.toMove);

					int i;
					for (i=0; i<moveCount; i++)
						if (generateMove(leavingPos, arrivingPos) == moves[i]) {
							game = makeMove(game, moves[i]);
							renderBoard(game.board);
						}
				}
				break;
			}
		}
	}
}

int main( int argc, char* args[] ) {
	if ( !init() )
		return -1;

	playRandomColor(DEFAULT_AI_DEPTH);

	close();

	return 0;
}
