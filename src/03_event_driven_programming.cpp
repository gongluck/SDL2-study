/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Starts up SDL and creates window
bool init_event_driven_programming();

//Loads media
bool loadMedia_event_driven_programming();

//Frees media and shuts down SDL
void close_event_driven_programming();

//The window we'll be rendering to
SDL_Window* gWindow_event_driven_programming = NULL;
	
//The surface contained by the window
SDL_Surface* gScreenSurface_event_driven_programming = NULL;

//The image we will load and show on the screen
SDL_Surface* gXOut_event_driven_programming = NULL;

bool init_event_driven_programming()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Create window
		gWindow_event_driven_programming = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_event_driven_programming == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Get window surface
			gScreenSurface_event_driven_programming = SDL_GetWindowSurface( gWindow_event_driven_programming);
		}
	}

	return success;
}

bool loadMedia_event_driven_programming()
{
	//Loading success flag
	bool success = true;

	//Load splash image
	gXOut_event_driven_programming = SDL_LoadBMP( "x.bmp" );
	if( gXOut_event_driven_programming == NULL )
	{
		printf( "Unable to load image %s! SDL Error: %s\n", "03_event_driven_programming/x.bmp", SDL_GetError() );
		success = false;
	}

	return success;
}

void close_event_driven_programming()
{
	//Deallocate surface
	SDL_FreeSurface( gXOut_event_driven_programming);
	gXOut_event_driven_programming = NULL;

	//Destroy window
	SDL_DestroyWindow( gWindow_event_driven_programming);
	gWindow_event_driven_programming = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

int main_event_driven_programming( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init_event_driven_programming() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia_event_driven_programming() )
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

				//Apply the image
				SDL_BlitSurface( gXOut_event_driven_programming, NULL, gScreenSurface_event_driven_programming, NULL );
			
				//Update the surface
				SDL_UpdateWindowSurface( gWindow_event_driven_programming);
			}
		}
	}

	//Free resources and close SDL
	close_event_driven_programming();

	return 0;
}