/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL, standard IO, and strings
#include <SDL.h>
#include <stdio.h>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Starts up SDL and creates window
bool init_optimized_surface_loading_and_soft_stretching();

//Loads media
bool loadMedia_optimized_surface_loading_and_soft_stretching();

//Frees media and shuts down SDL
void close_optimized_surface_loading_and_soft_stretching();

//Loads individual image
SDL_Surface* loadSurface_optimized_surface_loading_and_soft_stretching( std::string path );

//The window we'll be rendering to
SDL_Window* gWindow_optimized_surface_loading_and_soft_stretching = NULL;
	
//The surface contained by the window
SDL_Surface* gScreenSurface_optimized_surface_loading_and_soft_stretching = NULL;

//Current displayed image
SDL_Surface* gStretchedSurface_optimized_surface_loading_and_soft_stretching = NULL;

bool init_optimized_surface_loading_and_soft_stretching()
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
		//Create window
		gWindow_optimized_surface_loading_and_soft_stretching = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_optimized_surface_loading_and_soft_stretching == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Get window surface
			gScreenSurface_optimized_surface_loading_and_soft_stretching = SDL_GetWindowSurface( gWindow_optimized_surface_loading_and_soft_stretching);
		}
	}

	return success;
}

bool loadMedia_optimized_surface_loading_and_soft_stretching()
{
	//Loading success flag
	bool success = true;

	//Load stretching surface
	gStretchedSurface_optimized_surface_loading_and_soft_stretching = loadSurface_optimized_surface_loading_and_soft_stretching( "stretch.bmp" );
	if( gStretchedSurface_optimized_surface_loading_and_soft_stretching == NULL )
	{
		printf( "Failed to load stretching image!\n" );
		success = false;
	}

	return success;
}

void close_optimized_surface_loading_and_soft_stretching()
{
	//Free loaded image
	SDL_FreeSurface( gStretchedSurface_optimized_surface_loading_and_soft_stretching);
	gStretchedSurface_optimized_surface_loading_and_soft_stretching = NULL;

	//Destroy window
	SDL_DestroyWindow( gWindow_optimized_surface_loading_and_soft_stretching);
	gWindow_optimized_surface_loading_and_soft_stretching = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

SDL_Surface* loadSurface_optimized_surface_loading_and_soft_stretching( std::string path )
{
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = SDL_LoadBMP( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
	}
	else
	{
		//Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface_optimized_surface_loading_and_soft_stretching->format, NULL );
		if( optimizedSurface == NULL )
		{
			printf( "Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	return optimizedSurface;
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init_optimized_surface_loading_and_soft_stretching() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia_optimized_surface_loading_and_soft_stretching() )
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

				//Apply the image stretched
				SDL_Rect stretchRect;
				stretchRect.x = 0;
				stretchRect.y = 0;
				stretchRect.w = SCREEN_WIDTH;
				stretchRect.h = SCREEN_HEIGHT;
				SDL_BlitScaled( gStretchedSurface_optimized_surface_loading_and_soft_stretching, NULL, gScreenSurface_optimized_surface_loading_and_soft_stretching, &stretchRect );
			
				//Update the surface
				SDL_UpdateWindowSurface( gWindow_optimized_surface_loading_and_soft_stretching);
			}
		}
	}

	//Free resources and close SDL
	close_optimized_surface_loading_and_soft_stretching();

	return 0;
}