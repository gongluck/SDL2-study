/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Starts up SDL and creates window
bool init_geometry_rendering();

//Loads media
bool loadMedia_geometry_rendering();

//Frees media and shuts down SDL
void close_geometry_rendering();

//Loads individual image as texture
SDL_Texture* loadTexture_geometry_rendering( std::string path );

//The window we'll be rendering to
SDL_Window* gWindow_geometry_rendering = NULL;

//The window renderer
SDL_Renderer* gRenderer_geometry_rendering = NULL;

bool init_geometry_rendering()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow_geometry_rendering = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_geometry_rendering == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer_geometry_rendering = SDL_CreateRenderer( gWindow_geometry_rendering, -1, SDL_RENDERER_ACCELERATED );
			if( gRenderer_geometry_rendering == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer_geometry_rendering, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia_geometry_rendering()
{
	//Loading success flag
	bool success = true;

	//Nothing to load
	return success;
}

void close_geometry_rendering()
{
	//Destroy window	
	SDL_DestroyRenderer( gRenderer_geometry_rendering);
	SDL_DestroyWindow( gWindow_geometry_rendering);
	gWindow_geometry_rendering = NULL;
	gRenderer_geometry_rendering = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

SDL_Texture* loadTexture_geometry_rendering( std::string path )
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer_geometry_rendering, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	return newTexture;
}

int main_geometry_rendering( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init_geometry_rendering() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia_geometry_rendering() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
				}

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer_geometry_rendering, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer_geometry_rendering);

				//Render red filled quad
				SDL_Rect fillRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
				SDL_SetRenderDrawColor( gRenderer_geometry_rendering, 0xFF, 0x00, 0x00, 0xFF );
				SDL_RenderFillRect( gRenderer_geometry_rendering, &fillRect );

				//Render green outlined quad
				SDL_Rect outlineRect = { SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6, SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3 };
				SDL_SetRenderDrawColor( gRenderer_geometry_rendering, 0x00, 0xFF, 0x00, 0xFF );
				SDL_RenderDrawRect( gRenderer_geometry_rendering, &outlineRect );
				
				//Draw blue horizontal line
				SDL_SetRenderDrawColor( gRenderer_geometry_rendering, 0x00, 0x00, 0xFF, 0xFF );
				SDL_RenderDrawLine( gRenderer_geometry_rendering, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2 );

				//Draw vertical line of yellow dots
				SDL_SetRenderDrawColor( gRenderer_geometry_rendering, 0xFF, 0xFF, 0x00, 0xFF );
				for( int i = 0; i < SCREEN_HEIGHT; i += 4 )
				{
					SDL_RenderDrawPoint( gRenderer_geometry_rendering, SCREEN_WIDTH / 2, i );
				}

				//Update screen
				SDL_RenderPresent( gRenderer_geometry_rendering);
			}
		}
	}

	//Free resources and close SDL
	close_geometry_rendering();

	return 0;
}