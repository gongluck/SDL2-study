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
bool init_texture_loading_and_rendering();

//Loads media
bool loadMedia_texture_loading_and_rendering();

//Frees media and shuts down SDL
void close_texture_loading_and_rendering();

//Loads individual image as texture
SDL_Texture* loadTexture_texture_loading_and_rendering( std::string path );

//The window we'll be rendering to
SDL_Window* gWindow_texture_loading_and_rendering = NULL;

//The window renderer
SDL_Renderer* gRenderer_texture_loading_and_rendering = NULL;

//Current displayed texture
SDL_Texture* gTexture_texture_loading_and_rendering = NULL;

bool init_texture_loading_and_rendering()
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
		gWindow_texture_loading_and_rendering = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_texture_loading_and_rendering == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer_texture_loading_and_rendering = SDL_CreateRenderer( gWindow_texture_loading_and_rendering, -1, SDL_RENDERER_ACCELERATED );
			if( gRenderer_texture_loading_and_rendering == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer_texture_loading_and_rendering, 0xFF, 0xFF, 0xFF, 0xFF );

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

bool loadMedia_texture_loading_and_rendering()
{
	//Loading success flag
	bool success = true;

	//Load PNG texture
	gTexture_texture_loading_and_rendering = loadTexture_texture_loading_and_rendering( "texture.png" );
	if( gTexture_texture_loading_and_rendering == NULL )
	{
		printf( "Failed to load texture image!\n" );
		success = false;
	}

	return success;
}

void close_texture_loading_and_rendering()
{
	//Free loaded image
	SDL_DestroyTexture( gTexture_texture_loading_and_rendering);
	gTexture_texture_loading_and_rendering = NULL;

	//Destroy window	
	SDL_DestroyRenderer( gRenderer_texture_loading_and_rendering);
	SDL_DestroyWindow( gWindow_texture_loading_and_rendering);
	gWindow_texture_loading_and_rendering = NULL;
	gRenderer_texture_loading_and_rendering = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

SDL_Texture* loadTexture_texture_loading_and_rendering( std::string path )
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
        newTexture = SDL_CreateTextureFromSurface( gRenderer_texture_loading_and_rendering, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	return newTexture;
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init_texture_loading_and_rendering() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia_texture_loading_and_rendering() )
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
				SDL_RenderClear( gRenderer_texture_loading_and_rendering);

				//Render texture to screen
				SDL_RenderCopy( gRenderer_texture_loading_and_rendering, gTexture_texture_loading_and_rendering, NULL, NULL );

				//Update screen
				SDL_RenderPresent( gRenderer_texture_loading_and_rendering);
			}
		}
	}

	//Free resources and close SDL
	close_texture_loading_and_rendering();

	return 0;
}