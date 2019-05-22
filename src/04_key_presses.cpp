/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL, standard IO, and strings
#include <SDL.h>
#include <stdio.h>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Key press surfaces constants
enum KeyPressSurfaces
{
	KEY_PRESS_SURFACE_DEFAULT,
	KEY_PRESS_SURFACE_UP,
	KEY_PRESS_SURFACE_DOWN,
	KEY_PRESS_SURFACE_LEFT,
	KEY_PRESS_SURFACE_RIGHT,
	KEY_PRESS_SURFACE_TOTAL
};

//Starts up SDL and creates window
bool init_key_presses();

//Loads media
bool loadMedia_key_presses();

//Frees media and shuts down SDL
void close_key_presses();

//Loads individual image
SDL_Surface* loadSurface_key_presses( std::string path );

//The window we'll be rendering to
SDL_Window* gWindow_key_presses = NULL;
	
//The surface contained by the window
SDL_Surface* gScreenSurface_key_presses = NULL;

//The images that correspond to a keypress
SDL_Surface* gKeyPressSurfaces_key_presses[ KEY_PRESS_SURFACE_TOTAL ];

//Current displayed image
SDL_Surface* gCurrentSurface_key_presses = NULL;

bool init_key_presses()
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
		gWindow_key_presses = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_key_presses == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Get window surface
			gScreenSurface_key_presses = SDL_GetWindowSurface( gWindow_key_presses);
		}
	}

	return success;
}

bool loadMedia_key_presses()
{
	//Loading success flag
	bool success = true;

	//Load default surface
	gKeyPressSurfaces_key_presses[ KEY_PRESS_SURFACE_DEFAULT ] = loadSurface_key_presses( "press.bmp" );
	if( gKeyPressSurfaces_key_presses[ KEY_PRESS_SURFACE_DEFAULT ] == NULL )
	{
		printf( "Failed to load default image!\n" );
		success = false;
	}

	//Load up surface
	gKeyPressSurfaces_key_presses[ KEY_PRESS_SURFACE_UP ] = loadSurface_key_presses( "up.bmp" );
	if( gKeyPressSurfaces_key_presses[ KEY_PRESS_SURFACE_UP ] == NULL )
	{
		printf( "Failed to load up image!\n" );
		success = false;
	}

	//Load down surface
	gKeyPressSurfaces_key_presses[ KEY_PRESS_SURFACE_DOWN ] = loadSurface_key_presses( "down.bmp" );
	if( gKeyPressSurfaces_key_presses[ KEY_PRESS_SURFACE_DOWN ] == NULL )
	{
		printf( "Failed to load down image!\n" );
		success = false;
	}

	//Load left surface
	gKeyPressSurfaces_key_presses[ KEY_PRESS_SURFACE_LEFT ] = loadSurface_key_presses( "left.bmp" );
	if( gKeyPressSurfaces_key_presses[ KEY_PRESS_SURFACE_LEFT ] == NULL )
	{
		printf( "Failed to load left image!\n" );
		success = false;
	}

	//Load right surface
	gKeyPressSurfaces_key_presses[ KEY_PRESS_SURFACE_RIGHT ] = loadSurface_key_presses( "right.bmp" );
	if( gKeyPressSurfaces_key_presses[ KEY_PRESS_SURFACE_RIGHT ] == NULL )
	{
		printf( "Failed to load right image!\n" );
		success = false;
	}

	return success;
}

void close_key_presses()
{
	//Deallocate surfaces
	for( int i = 0; i < KEY_PRESS_SURFACE_TOTAL; ++i )
	{
		SDL_FreeSurface( gKeyPressSurfaces_key_presses[ i ] );
		gKeyPressSurfaces_key_presses[ i ] = NULL;
	}

	//Destroy window
	SDL_DestroyWindow( gWindow_key_presses);
	gWindow_key_presses = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

SDL_Surface* loadSurface_key_presses( std::string path )
{
	//Load image at specified path
	SDL_Surface* loadedSurface = SDL_LoadBMP( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
	}

	return loadedSurface;
}


int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init_key_presses() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia_key_presses() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//Set default current surface
			gCurrentSurface_key_presses = gKeyPressSurfaces_key_presses[ KEY_PRESS_SURFACE_DEFAULT ];

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
					//User presses a key
					else if( e.type == SDL_KEYDOWN )
					{
						//Select surfaces based on key press
						switch( e.key.keysym.sym )
						{
							case SDLK_UP:
							gCurrentSurface_key_presses = gKeyPressSurfaces_key_presses[ KEY_PRESS_SURFACE_UP ];
							break;

							case SDLK_DOWN:
							gCurrentSurface_key_presses = gKeyPressSurfaces_key_presses[ KEY_PRESS_SURFACE_DOWN ];
							break;

							case SDLK_LEFT:
                            gCurrentSurface_key_presses = gKeyPressSurfaces_key_presses[ KEY_PRESS_SURFACE_LEFT ];
							break;

							case SDLK_RIGHT:
                            gCurrentSurface_key_presses = gKeyPressSurfaces_key_presses[ KEY_PRESS_SURFACE_RIGHT ];
							break;

							default:
                            gCurrentSurface_key_presses = gKeyPressSurfaces_key_presses[ KEY_PRESS_SURFACE_DEFAULT ];
							break;
						}
					}
				}

				//Apply the current image
				SDL_BlitSurface( gCurrentSurface_key_presses, NULL, gScreenSurface_key_presses, NULL );
			
				//Update the surface
				SDL_UpdateWindowSurface( gWindow_key_presses);
			}
		}
	}

	//Free resources and close SDL
	close_key_presses();

	return 0;
}