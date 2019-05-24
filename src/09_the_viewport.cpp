/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Starts up SDL and creates window
bool init_the_viewport();

//Loads media
bool loadMedia_the_viewport();

//Frees media and shuts down SDL
void close_the_viewport();

//Loads individual image as texture
SDL_Texture* loadTexture_the_viewport( std::string path );

//The window we'll be rendering to
SDL_Window* gWindow_the_viewport = NULL;

//The window renderer
SDL_Renderer* gRenderer_the_viewport = NULL;

//Current displayed texture
SDL_Texture* gTexture_the_viewport = NULL;

bool init_the_viewport()
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
		gWindow_the_viewport = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_the_viewport == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer_the_viewport = SDL_CreateRenderer( gWindow_the_viewport, -1, SDL_RENDERER_ACCELERATED );
			if( gRenderer_the_viewport == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer_the_viewport, 0xFF, 0xFF, 0xFF, 0xFF );

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

bool loadMedia_the_viewport()
{
	//Loading success flag
	bool success = true;

	//Load texture
	gTexture_the_viewport = loadTexture_the_viewport( "viewport.png" );
	if( gTexture_the_viewport == NULL )
	{
		printf( "Failed to load texture image!\n" );
		success = false;
	}

	//Nothing to load
	return success;
}

void close_the_viewport()
{
	//Free loaded image
	SDL_DestroyTexture( gTexture_the_viewport);
	gTexture_the_viewport = NULL;

	//Destroy window	
	SDL_DestroyRenderer( gRenderer_the_viewport);
	SDL_DestroyWindow( gWindow_the_viewport);
	gWindow_the_viewport = NULL;
	gRenderer_the_viewport = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

SDL_Texture* loadTexture_the_viewport( std::string path )
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
        newTexture = SDL_CreateTextureFromSurface( gRenderer_the_viewport, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	return newTexture;
}

int main_the_viewport( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init_the_viewport() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia_the_viewport() )
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
				SDL_SetRenderDrawColor( gRenderer_the_viewport, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer_the_viewport);

				//Top left corner viewport
				SDL_Rect topLeftViewport;
				topLeftViewport.x = 0;
				topLeftViewport.y = 0;
				topLeftViewport.w = SCREEN_WIDTH / 2;
				topLeftViewport.h = SCREEN_HEIGHT / 2;
				SDL_RenderSetViewport( gRenderer_the_viewport, &topLeftViewport );
				
				//Render texture to screen
				SDL_RenderCopy( gRenderer_the_viewport, gTexture_the_viewport, NULL, NULL );


				//Top right viewport
				SDL_Rect topRightViewport;
				topRightViewport.x = SCREEN_WIDTH / 2;
				topRightViewport.y = 0;
				topRightViewport.w = SCREEN_WIDTH / 2;
				topRightViewport.h = SCREEN_HEIGHT / 2;
				SDL_RenderSetViewport( gRenderer_the_viewport, &topRightViewport );
				
				//Render texture to screen
				SDL_RenderCopy( gRenderer_the_viewport, gTexture_the_viewport, NULL, NULL );


				//Bottom viewport
				SDL_Rect bottomViewport;
				bottomViewport.x = 0;
				bottomViewport.y = SCREEN_HEIGHT / 2;
				bottomViewport.w = SCREEN_WIDTH;
				bottomViewport.h = SCREEN_HEIGHT / 2;
				SDL_RenderSetViewport( gRenderer_the_viewport, &bottomViewport );

				
				//Render texture to screen
				SDL_RenderCopy( gRenderer_the_viewport, gTexture_the_viewport, NULL, NULL );


				//Update screen
				SDL_RenderPresent( gRenderer_the_viewport);
			}
		}
	}

	//Free resources and close SDL
	close_the_viewport();

	return 0;
}