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
bool init_extension_libraries_and_loading_other_image_formats();

//Loads media
bool loadMedia_extension_libraries_and_loading_other_image_formats();

//Frees media and shuts down SDL
void close_extension_libraries_and_loading_other_image_formats();

//Loads individual image
SDL_Surface* loadSurface_extension_libraries_and_loading_other_image_formats( std::string path );

//The window we'll be rendering to
SDL_Window* gWindow_extension_libraries_and_loading_other_image_formats = NULL;
	
//The surface contained by the window
SDL_Surface* gScreenSurface_extension_libraries_and_loading_other_image_formats = NULL;

//Current displayed PNG image
SDL_Surface* gPNGSurface_extension_libraries_and_loading_other_image_formats = NULL;

bool init_extension_libraries_and_loading_other_image_formats()
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
		gWindow_extension_libraries_and_loading_other_image_formats = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_extension_libraries_and_loading_other_image_formats == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Initialize PNG loading
			int imgFlags = IMG_INIT_PNG;
			if( !( IMG_Init( imgFlags ) & imgFlags ) )
			{
				printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
				success = false;
			}
			else
			{
				//Get window surface
				gScreenSurface_extension_libraries_and_loading_other_image_formats = SDL_GetWindowSurface( gWindow_extension_libraries_and_loading_other_image_formats);
			}
		}
	}

	return success;
}

bool loadMedia_extension_libraries_and_loading_other_image_formats()
{
	//Loading success flag
	bool success = true;

	//Load PNG surface
	gPNGSurface_extension_libraries_and_loading_other_image_formats = loadSurface_extension_libraries_and_loading_other_image_formats( "loaded.png" );
	if( gPNGSurface_extension_libraries_and_loading_other_image_formats == NULL )
	{
		printf( "Failed to load PNG image!\n" );
		success = false;
	}

	return success;
}

void close_extension_libraries_and_loading_other_image_formats()
{
	//Free loaded image
	SDL_FreeSurface( gPNGSurface_extension_libraries_and_loading_other_image_formats);
	gPNGSurface_extension_libraries_and_loading_other_image_formats = NULL;

	//Destroy window
	SDL_DestroyWindow( gWindow_extension_libraries_and_loading_other_image_formats);
	gWindow_extension_libraries_and_loading_other_image_formats = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

SDL_Surface* loadSurface_extension_libraries_and_loading_other_image_formats( std::string path )
{
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface_extension_libraries_and_loading_other_image_formats->format, NULL );
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
	if( !init_extension_libraries_and_loading_other_image_formats() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia_extension_libraries_and_loading_other_image_formats() )
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

				//Apply the PNG image
				SDL_BlitSurface( gPNGSurface_extension_libraries_and_loading_other_image_formats, NULL, gScreenSurface_extension_libraries_and_loading_other_image_formats, NULL );
			
				//Update the surface
				SDL_UpdateWindowSurface( gWindow_extension_libraries_and_loading_other_image_formats);
			}
		}
	}

	//Free resources and close SDL
	close_extension_libraries_and_loading_other_image_formats();

	return 0;
}