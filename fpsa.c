#include <math.h>
#include <inttypes.h>
#include <SDL.h>
//~ #include <SDL_image.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "vec2d.h"
#include "mymaths.h"

#define TITLE "DDA Intersection Checking Algorithm"

//Screen dimension constants
//~ int const SCREEN_WIDTH = 640;
//~ int const SCREEN_HEIGHT = 480;
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// World constants
int const CENTER_X = SCREEN_WIDTH / 2;
int const CENTER_Y = SCREEN_HEIGHT / 2;
#define GRID_SIZE 64	// The size of grids
#define DOT_SIZE 6
#define FPS 30
int const FRAMETIME = (1000 / FPS);	// Minimum to display each frame
#define WALL_HEIGHT SCREEN_HEIGHT

#define PLAYER_SPEED 150
#define FOV 75.0			// Needs to be double!
#define PLAYER_ROT 45		// Rate of turn (rotation speed)

#define MAP_WIDTH (SCREEN_WIDTH / GRID_SIZE)	// Map width in blocks
#define MAP_HEIGHT (SCREEN_HEIGHT / GRID_SIZE)	// Map height in blocks
//~ char maparray123[SCREEN_WIDTH * SCREEN_HEIGHT / (GRID_SIZE * GRID_SIZE)];
#define MAP_SIZE (MAP_WIDTH * MAP_HEIGHT)


// Intersection constants
//~ #define 	        NO_INTERSECTION 	0
//~ #define 	HORIZONTAL_INTERSECTION 	1
//~ #define		  VERTICAL_INTERSECTION 	2

enum IntersectionTypes {
	NO_INTERSECTION,
	HORIZONTAL_INTERSECTION,
	VERTICAL_INTERSECTION
};

// Struct for storing ray intersection with the map.
typedef struct {
	Vec2D location;		// Location of intersection
	int type;			// Type of intersection (horiz/vert/none)
	int mapIndex;		// Index of the map the intersection occured.
} RayWallIntersect;

bool init();		//Starts up SDL and creates window
bool loadMedia();	//Loads media
void Quit();		//Frees media and shuts down SDL

//Loads individual image as texture
SDL_Texture* loadTexture(char const * path);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		return false;
	}
	
	//Set texture filtering to linear
	if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) ) {
		printf( "Warning: Linear texture filtering not enabled!" );
	}

	//Create window
	gWindow = SDL_CreateWindow( TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
	if( gWindow == NULL ) {
		printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
		return false;
	}

	//Create renderer for window
	gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
	if( gRenderer == NULL ) {
		printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
		return false;
	}
	
	//Initialize renderer color
	SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

	//Initialize PNG loading
	//~ int imgFlags = IMG_INIT_PNG;
	//~ if( !( IMG_Init( imgFlags ) & imgFlags ) )
	//~ {
		//~ printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
		//~ success = false;
	//~ }
	

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Nothing to load
	return success;
}

void destroy()
{
	// Don't call this function 'close' since that's a POSIX function.
	// Otherwise, SEGFAULT
	
	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	//~ IMG_Quit();
	SDL_Quit();
	
	printf("Good bye!\n");
}

//~ SDL_Texture* loadTexture( char const * path )
//~ {
	//~ //The final texture
	//~ SDL_Texture* newTexture = NULL;

	//~ //Load image at specified path
	//~ SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	//~ if( loadedSurface == NULL )
	//~ {
		//~ printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	//~ }
	//~ else
	//~ {
		//~ //Create texture from surface pixels
        //~ newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		//~ if( newTexture == NULL )
		//~ {
			//~ printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		//~ }

		//~ //Get rid of old loaded surface
		//~ SDL_FreeSurface( loadedSurface );
	//~ }

	//~ return newTexture;
//~ }

int screenToMapIndex(Vec2D* pvec)
{
	int mapx, mapy;
	
	mapx = pvec->x / GRID_SIZE;
	mapy = pvec->y / GRID_SIZE;
	
	// Make sure we don't get the same index from more two different vectors (one-to-one)
	if (mapx < 0 || mapx >= MAP_WIDTH)
		return -1;
	if (mapy < 0 || mapy >= MAP_HEIGHT)
		return -1;
	
	return MAP_WIDTH * mapy + mapx;
}

bool isMapIndexValid(int index) {
	return (index < 0 || index >= MAP_SIZE) ? false : true;
}

void generateMap(char* maparray)
{
	memset(maparray, ' ', MAP_SIZE * sizeof(char));
	maparray[MAP_WIDTH * 0 + 0] = 'W';	// 'W' for wall
	maparray[MAP_WIDTH * 1 + 0] = 'W';
	maparray[MAP_WIDTH * 2 + 0] = 'W';
	maparray[MAP_WIDTH * 2 + 1] = 'W';
	maparray[MAP_WIDTH * 2 + 2] = 'W';
	maparray[MAP_WIDTH * 2 + 3] = 'W';
	maparray[MAP_WIDTH * (MAP_HEIGHT / 2) + MAP_WIDTH / 2] = 'W';
	maparray[MAP_WIDTH * (MAP_HEIGHT - 2) + (MAP_WIDTH - 2)] = 'W';
}

void printMap(char const * const maparray)
{
	printf("maparray size: %lu\n", sizeof(maparray));
	printf("Map contents:\n");
	for (int row = 0; row < MAP_HEIGHT; row++) {
		for (int col = 0; col < MAP_WIDTH; col++) {
			putchar(maparray[MAP_WIDTH * row + col]);
		}
		putchar('\n');
	}
}


Vec2D findNearestVertGridIntersect(Vec2D const* const origin, Vec2D const* const heading)
{
	Vec2D A;	// Vertical intersect
	
	if (heading->x > 0) {
		A.x = ceil(origin->x / GRID_SIZE) * GRID_SIZE;
	} else {
		A.x = floor(origin->x / GRID_SIZE) * GRID_SIZE;
	}
	A.y = (heading->y / heading->x) * (A.x - origin->x) + origin->y;
	
	return A;
}

Vec2D findNearestHorizGridIntersect(Vec2D const* const origin, Vec2D const* const heading)
{
	Vec2D B;	// Horizontal intersect
	
	if (heading->y > 0) {
		// alt. way of getting y intersection at top of P
		B.y = floor(origin->y / GRID_SIZE) * GRID_SIZE + GRID_SIZE;	
	} else {
		B.y = floor(origin->y / GRID_SIZE) * GRID_SIZE;
	}
	B.x = (heading->x / heading->y) * (B.y - origin->y) + origin->x;
	
	return B;
}

Vec2D calculateXYSteps(Vec2D const* const heading) {
	Vec2D xySteps;
	xySteps.y = GRID_SIZE * heading->y / fabs(heading->x);
	xySteps.x = GRID_SIZE * heading->x / fabs(heading->y);
	return xySteps;
}

void findWallIntersect(	RayWallIntersect* const rwi,
						char const* const maparray,
						Vec2D const* const xySteps,				// <- this, 
						Vec2D const* const origin, 				// <- this,
						Vec2D const* const firstVertIntersect,	// <- this,
						Vec2D const* const firstHorizIntersect)	// <- and this can be figured out using a heading vector
																// but I think this is faster because we don't have to
																// recalculate everything twice... though I wonder about
																// cache access performace degradation...
/* rwi		= The destination of intersection results.
 * maparray	= Map array the current game world is using.
 * xySteps	= Vector containing x and y steps.
 * origin	= The vector from which the ray was cast from (used for 
 * selecting which vector to return if we have two intersections 
 * (vert and horiz).
 * firstVertIntersect	= The first vertical intersection of the grid
 * nearest to the origin.
 * firstHorizIntersect	= The first horizontal intersection of the grid
 * nearest to the origin.
 */
{	
	double xstep = xySteps->x;
	double ystep = xySteps->y;
	
	Vec2D hx, vx;		// horizontal and vertical intersections
	double xdir, ydir;	// x/y direction to adjust points hx/vx to move across grid cells.
	int xoffset, yoffset;	// x/y offsets to make sure current locations on the map touch a grid cell
	int hIndex = -1;		// Map index a horiz. intersection occured.
	int vIndex = -1;		// Map index a vert. intersection occured.
	
	// Flags for deterwining the type of grid intersection
	bool hIntersected = false;	// Horizontal
	bool vIntersected = false;	// Vertical
	
	// [ Check for horizontal intersections first ]
	hx = *firstHorizIntersect;
	// Ensure that the y-coordinate touches the inside of a cell/square based on the sign of ystep.
	yoffset = ystep > 0 ? +1 : -1;
	ydir = yoffset * GRID_SIZE;
	hx.y += yoffset;
	
	// make sure we're only checking within height of the map
	while (0 <= hx.y && hx.y < GRID_SIZE * MAP_HEIGHT) {
		hIndex = screenToMapIndex(&hx);
		if (isMapIndexValid(hIndex) && maparray[hIndex] == 'W') {
			hx.y -= yoffset;	// Remove y-offset since we no longer 
								// need the point vx to detect 
								// horizontal line intersections
			//~ printf("Wall horizontal intersect at map index %d, (%.2f, %.2f)\n", hIndex, hx.x, hx.y);
			hIntersected = true;
			break;
		}
		hx.y += ydir;
		hx.x += xstep;
	}
	
	// [ Check for vertical intersections second ]
	vx = *firstVertIntersect;
	xoffset = xstep > 0 ? +1 : -1;
	xdir = xoffset * GRID_SIZE;
	vx.x += xoffset;
	
	// make sure we're only checking within width of the map
	while (0 <= vx.x && vx.x < GRID_SIZE * MAP_WIDTH) {
		vIndex = screenToMapIndex(&vx);
		if (isMapIndexValid(vIndex) && maparray[vIndex] == 'W') {
			vx.x -= xoffset;	// Remove y-offset since we no longer
								// need the point vx to detect vertical
								// line intersections.
			//~ printf("Wall vertical intersect at map index %d, (%.2f, %.2f)\n", vIndex, vx.x, vx.y);
			vIntersected = true;
			break;
		}
		vx.y += ystep;
		vx.x += xdir;
	}
	
	// [ Select which grid intersect to return ]
	if (hIntersected && vIntersected) {
		// Select the closest relative to P
		if (Vec2D_norm(Vec2D_subtract(hx, *origin)) < Vec2D_norm(Vec2D_subtract(vx, *origin))) {
			rwi->location = hx;
			rwi->type = HORIZONTAL_INTERSECTION;
			rwi->mapIndex = hIndex;
			//~ puts("hx is closest to P. Picking hx.");
		} else {
			rwi->location = vx;
			rwi->type = VERTICAL_INTERSECTION;
			rwi->mapIndex = vIndex;
			//~ puts("vx is closest to P. Picking vx.");
		}
	}
	else if (hIntersected) {
		rwi->location = hx;
		rwi->type = HORIZONTAL_INTERSECTION;
		rwi->mapIndex = hIndex;
		//~ puts("Only horizontal intersection");
	}
	else if (vIntersected) {
		rwi->location = vx;
		rwi->type = VERTICAL_INTERSECTION;
		rwi->mapIndex = vIndex;
		//~ puts("Only vertical intersection");
	}
	else {
		rwi->type = NO_INTERSECTION;
		rwi->mapIndex = -1;
	}
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if(!init()) {
		printf( "Failed to initialize!\n" );
		destroy();
		return 1;
	}
	
	//Load media
	if(!loadMedia()) {
		printf( "Failed to load media!\n" );
		destroy();
		return 1;
	}

	// My variables
	int mousex, mousey;
	Vec2D P = {CENTER_X, CENTER_Y};
	double playerAngle = 0;
	double playerTurnVelocity;
	
	Vec2D H = {};		// Player heading based on playerAngle
	Vec2D A = {};		// First vertical intersect (A,B) relative to P.
	Vec2D B = {};		// First horizontal intersect (A,B) relative to P.
	Vec2D mousevec;		// Contains mouse coordinates.
	Vec2D Vp = {0, 0};	// Velocity of P (zero it so P doesn't run away when we start the program)
	bool drawGreenRay = true;
	double xstep, ystep;
	
	// Create map
	// Make map an array and convert coordinates of ray-wall intersection to indeces of the map array.
	char maparray[MAP_SIZE];	// Flat memory layout is more efficient
	generateMap(maparray);
	printMap(maparray);
	
	
	bool quit = false;	//Main loop flag
	SDL_Event e;		// Event handler
	
	// Update variables
	uint32_t lastTime;
	uint32_t currentTime;
	double  updateTime;

	//While application is running
	while( !quit )
	{
		lastTime = currentTime;
		currentTime = SDL_GetTicks();
		updateTime = (currentTime - lastTime) / 1000.0;
		
		
		// -------------------- EVENTS STUFF --------------------
		// -------------------- EVENTS STUFF --------------------
		// -------------------- EVENTS STUFF --------------------
		
		//Handle events on queue
		while( SDL_PollEvent( &e ) != 0 )
		{
			int key;
			
			//User requests quit
			switch (e.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_MOUSEMOTION:
				mousex = e.motion.x;
				mousey = e.motion.y;
				mousevec = (Vec2D){mousex, mousey};
				//~ printf("Mouse Pos: (%d, %d)\n", mousex, mousey);
				//~ printf("H = (%g, %g)\n", H.x, H.y);
				//~ printf("A = (%g, %g)\n", A.x, A.y);
				//~ printf("B = (%g, %g)\n", B.x, B.y);
				
				// Figure out the map array index based on mouse coordinates.
				int k = screenToMapIndex(&mousevec);
				printf("Mouse's map index: %d ", k);
				printf("%s\n", isMapIndexValid(k) ? "" : "(invalid index)");
				
				break;
			case SDL_KEYDOWN:
				key = e.key.keysym.sym;
				switch(key) {
					case SDLK_ESCAPE:
						quit = true;
						break;
					case SDLK_SPACE:
						printf("Mouse Pos: (%.0f, %.0f)\n", mousevec.x, mousevec.y);
						break;
					case SDLK_w: Vp.y = -PLAYER_SPEED * updateTime; break;
					case SDLK_s: Vp.y = PLAYER_SPEED * updateTime; break;
					case SDLK_a: Vp.x = -PLAYER_SPEED * updateTime; break;
					case SDLK_d: Vp.x = PLAYER_SPEED * updateTime; break;
					case SDLK_LEFT:
						playerTurnVelocity = -PLAYER_ROT * updateTime; break;	// < 0 is anti-clockwise
					case SDLK_RIGHT:
						playerTurnVelocity = +PLAYER_ROT * updateTime; break;	// > 0 is clockwise
					case SDLK_UP:
						Vp = Vec2D_multiply(+PLAYER_SPEED * updateTime, H);
						break;
					case SDLK_DOWN:
						Vp = Vec2D_multiply(-PLAYER_SPEED * updateTime, H);
						break;
					
					case SDLK_e: drawGreenRay = !drawGreenRay; break;
				}
				break;
			case SDL_KEYUP:
				key = e.key.keysym.sym;
				switch(key) {
					case SDLK_w: Vp.y = 0; break;
					case SDLK_s: Vp.y = 0; break;
					case SDLK_a: Vp.x = 0; break;
					case SDLK_d: Vp.x = 0; break;
					case SDLK_UP: 
					case SDLK_DOWN:
						Vp = (Vec2D){}; break;
					case SDLK_LEFT: playerTurnVelocity = 0; break;
					case SDLK_RIGHT: playerTurnVelocity = 0; break;
				}
				break;
			default:
				break;
			}
		}

		// -------------------- LOGIC STUFF --------------------
		// -------------------- LOGIC STUFF --------------------
		// -------------------- LOGIC STUFF --------------------
		
		
		// Update P
		P = Vec2D_add(P, Vp);
		//~ playerAngle += PLAYER_ROT * updateTime;
		playerAngle += playerTurnVelocity;
		double playerRadians = degreesToRadians(playerAngle);
		H = (Vec2D){cos(playerRadians), sin(playerRadians)};
		//~ H = Vec2D_subtract(mousevec, P);
		
		// Determine points A and B which are vital for DDA algorithm.
		A = findNearestVertGridIntersect(&P, &H);
		B = findNearestHorizGridIntersect(&P, &H);
		
		
		// Calculate x-step and y-step for DDA
		Vec2D xySteps = calculateXYSteps(&H);
		ystep = xySteps.y;
		xstep = xySteps.x;
		
		// [ Perform DDA check ]
		Vec2D closestx;		// Closest intersection out of hx and vx.
		int intersectType;
		
		RayWallIntersect intersect;
		findWallIntersect(&intersect, maparray, &xySteps, &P, &A, &B);
		closestx = intersect.location;
		intersectType = intersect.type;

		// -------------------- GRAPHICS STUFF --------------------
		// -------------------- GRAPHICS STUFF --------------------
		// -------------------- GRAPHICS STUFF --------------------

		//Clear screen
		SDL_SetRenderDrawColor( gRenderer, 128, 128, 128, 0xFF );
		SDL_RenderClear( gRenderer );
		
		SDL_Rect fillRect;
		
		// Draw background
		SDL_SetRenderDrawColor(gRenderer, 32, 32, 32, 255);
		fillRect = (SDL_Rect){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2};
		SDL_RenderFillRect(gRenderer, &fillRect);
		// Draw foreground
		SDL_SetRenderDrawColor(gRenderer, 0, 64, 0, 255);
		fillRect = (SDL_Rect){0, CENTER_Y, SCREEN_WIDTH, SCREEN_HEIGHT / 2};
		SDL_RenderFillRect(gRenderer, &fillRect);
		
		
		// This produces fish eye effect. Need to make it 
		double dTheta = FOV / SCREEN_WIDTH;	// rate of change of ray cast angle
		double rayAngle = playerAngle - FOV / 2.0;	// starting ray angle (1/2*FOV clockwise of player angle)
		for (double ray = 0; ray < SCREEN_WIDTH; ++ray) {
			double degrees = rayAngle + ray * dTheta;
			//~ printf("deg: %.2f\n", degrees);
			double radians = degToRad(degrees);
			Vec2D rayHeading = (Vec2D){cos(radians), sin(radians)};
			
			// Determine points A and B which are vital for DDA algorithm.
			Vec2D vgX = findNearestVertGridIntersect(&P, &rayHeading);
			Vec2D hgX = findNearestHorizGridIntersect(&P, &rayHeading);
			
			// Calculate x-step and y-step for DDA
			Vec2D xySteps = calculateXYSteps(&rayHeading);
			
			// [ Perform DDA check ]
			RayWallIntersect intersect;
			findWallIntersect(&intersect, maparray, &xySteps, &P, &vgX, &hgX);
			
			// Draw ray if there's intersection
			if (intersect.type != NO_INTERSECTION) {
				//~ putchar('a');
				double distanceFromP = Vec2D_norm(Vec2D_subtract(intersect.location, P));
				double renderHeight = WALL_HEIGHT * GRID_SIZE / distanceFromP;
				renderHeight = min(renderHeight, SCREEN_HEIGHT);	// So we keep drawing rays if we're distance=0 from wall.
																	// (Most importantly, SDL draws the lines with x<0)
				//~ renderHeight = min(renderHeight, SCREEN_HEIGHT);
				//~ double renderHeight = WALL_HEIGHT / distanceFromP;
				int shade;	// the further away, the darker the colour of the ray
				shade = 255 * GRID_SIZE / distanceFromP;
				shade = min(255, shade);	// Limit shade value since it can explode to thousands and t.f. int overflow
				//~ shade = 255;
				//~ printf("shade: %.2f\n", shade);
				
				SDL_SetRenderDrawColor(gRenderer, 0, 0, shade, 255);
				double topY, bottomY;
				topY = (SCREEN_HEIGHT - renderHeight) / 2;
				bottomY = topY + renderHeight;
				
				SDL_RenderDrawLine(gRenderer, ray, topY, ray, bottomY);
			}
		}
		//~ return 0; // <<==================================TEMP STOP

		// Draw the grid
		SDL_SetRenderDrawColor(gRenderer, 64, 64, 64, 255);
		// Vertical lines
		double x;
		for (x = 0; x < SCREEN_WIDTH; x += GRID_SIZE) {
			SDL_RenderDrawLine(gRenderer, x, 0, x, SCREEN_HEIGHT);
		}
		// Horizontal lines
		double y;
		for (y = 0; y < SCREEN_HEIGHT; y += GRID_SIZE) {
			SDL_RenderDrawLine(gRenderer, 0, y, SCREEN_WIDTH, y);
		}
		
		// Draw ray from P
		if (drawGreenRay) {
			SDL_SetRenderDrawColor(gRenderer, 255, 0, 255, 255);
			SDL_RenderDrawLine(gRenderer, P.x, P.y, mousex, mousey);
		} else {
			SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
			SDL_RenderDrawPoint(gRenderer, P.x, P.y);
		}
		
		// Draw A, the nearest vertical line grid intersection to P, based on heading H
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 255);
		fillRect = (SDL_Rect){A.x - DOT_SIZE / 2, A.y - DOT_SIZE / 2, DOT_SIZE, DOT_SIZE};
		SDL_RenderFillRect(gRenderer, &fillRect);
		
		// Draw B, the nearest horizontal line grid intersection to P, based on heading H
		SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
		fillRect = (SDL_Rect){B.x - DOT_SIZE / 2, B.y - DOT_SIZE / 2, DOT_SIZE, DOT_SIZE};
		SDL_RenderFillRect(gRenderer, &fillRect);

		// Draw the first 10 points that intersect the vertical lines of 
		// of the grid relative to P, based on heading H.
		const int NUMDOTS = 20;
		double xdir = (H.x > 0) ? GRID_SIZE : -GRID_SIZE;
		Vec2D dot = A;	// I can't believe I've been using ints instead of doubles this whole time for the x/y's of the dots!!!
						// (Using variables x and y)
						// Lead me to believe there was something wrong with my maths, when in fact it was just computer
						// performing integer arithmetic rather than float. '-_-
		
		SDL_SetRenderDrawColor(gRenderer, 64, 64, 255, 255);
		//~ SDL_SetRenderDrawColor(gRenderer, 0, 0, 128, 255);
		
		for (int i = 0; i < NUMDOTS; ++i) {
			dot.x += xdir;
			dot.y += ystep;
			fillRect = (SDL_Rect){dot.x - DOT_SIZE / 2.0, dot.y - DOT_SIZE / 2.0, DOT_SIZE, DOT_SIZE};
			SDL_RenderFillRect(gRenderer, &fillRect);
			//~ SDL_RenderDrawPoint( gRenderer, dot.x, dot.y );
		}
		
		dot = B;
		double ydir = (H.y > 0) ? GRID_SIZE : -GRID_SIZE;
		SDL_SetRenderDrawColor(gRenderer, 255, 64, 64, 255);
		//~ SDL_SetRenderDrawColor(gRenderer, 128, 0, 0, 255);
		for (int i = 0; i < NUMDOTS; ++i) {
			dot.x += xstep;
			dot.y += ydir;
			fillRect = (SDL_Rect){dot.x - DOT_SIZE / 2.0, dot.y - DOT_SIZE / 2.0, DOT_SIZE, DOT_SIZE};
			SDL_RenderFillRect(gRenderer, &fillRect);
			//~ SDL_RenderDrawPoint( gRenderer, dot.x, dot.y );
		}
		
		
		// Draw map
		SDL_SetRenderDrawColor(gRenderer, 128, 128, 255, 255);
		for (int row = 0; row < MAP_HEIGHT; ++row) {
			for (int col = 0; col < MAP_WIDTH; ++col) {
				int bi;	// current block index on map
				SDL_Rect bc = {0, 0, GRID_SIZE, GRID_SIZE};	// block coordinates
				bi = MAP_WIDTH * row + col;
				bc.x = col * GRID_SIZE;
				bc.y = row * GRID_SIZE;
				if (maparray[bi] == 'W') {
					SDL_RenderFillRect(gRenderer, &bc);
				}
			}
		}
		
		// Plot the intersection...
		if (intersectType != NO_INTERSECTION) {
			switch (intersectType) {
			case VERTICAL_INTERSECTION:
				SDL_SetRenderDrawColor(gRenderer, 0, 255, 255, 255);
				break;
			case HORIZONTAL_INTERSECTION:
				SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 255);
				break;
			}
			//~ SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
			fillRect = (SDL_Rect){closestx.x - DOT_SIZE / 2.0, closestx.y - DOT_SIZE / 2.0, DOT_SIZE, DOT_SIZE};
			SDL_RenderFillRect(gRenderer, &fillRect);
		}
		

		//Update screen
		SDL_RenderPresent( gRenderer );
		
		uint32_t diff = SDL_GetTicks() - currentTime;
		if (diff < FRAMETIME) {
			SDL_Delay(FRAMETIME - diff);
		}
	}
	

	//Free resources and close SDL
	destroy();

	return 0;
}
