/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Starts up SDL and creates window
bool init_getting_an_image_on_the_screen();

//Loads media
bool loadMedia_getting_an_image_on_the_screen();

//Frees media and shuts down SDL
void close_getting_an_image_on_the_screen();

//The window we'll be rendering to
SDL_Window* gWindow_getting_an_image_on_the_screen = NULL;
	
//The surface contained by the window
SDL_Surface* gScreenSurface_getting_an_image_on_the_screen = NULL;

//The image we will load and show on the screen
SDL_Surface* gHelloWorld_getting_an_image_on_the_screen = NULL;

bool init_getting_an_image_on_the_screen()
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
		gWindow_getting_an_image_on_the_screen = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_getting_an_image_on_the_screen == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Get window surface
			gScreenSurface_getting_an_image_on_the_screen = SDL_GetWindowSurface( gWindow_getting_an_image_on_the_screen);
		}
	}

	return success;
}

bool loadMedia_getting_an_image_on_the_screen()
{
	//Loading success flag
	bool success = true;

	//Load splash image
	gHelloWorld_getting_an_image_on_the_screen = SDL_LoadBMP( "hello_world.bmp" );
	if( gHelloWorld_getting_an_image_on_the_screen == NULL )
	{
		printf( "Unable to load image %s! SDL Error: %s\n", "02_getting_an_image_on_the_screen/hello_world.bmp", SDL_GetError() );
		success = false;
	}

	return success;
}

void close_getting_an_image_on_the_screen()
{
	//Deallocate surface
	SDL_FreeSurface( gHelloWorld_getting_an_image_on_the_screen);
	gHelloWorld_getting_an_image_on_the_screen = NULL;

	//Destroy window
	SDL_DestroyWindow( gWindow_getting_an_image_on_the_screen);
	gWindow_getting_an_image_on_the_screen = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

int main_getting_an_image_on_the_screen( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init_getting_an_image_on_the_screen() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia_getting_an_image_on_the_screen() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{
			//Apply the image
			SDL_BlitSurface( gHelloWorld_getting_an_image_on_the_screen, NULL, gScreenSurface_getting_an_image_on_the_screen, NULL );
			
			//Update the surface
			SDL_UpdateWindowSurface( gWindow_getting_an_image_on_the_screen);

			//Wait two seconds
			SDL_Delay( 2000 );
		}
	}

	//Free resources and close SDL
	close_getting_an_image_on_the_screen();

	return 0;
}