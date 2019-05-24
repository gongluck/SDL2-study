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

//Texture wrapper class
class LTexture_color_keying
{
	public:
		//Initializes variables
		LTexture_color_keying();

		//Deallocates memory
		~LTexture_color_keying();

		//Loads image at specified path
		bool loadFromFile( std::string path );

		//Deallocates texture
		void free();

		//Renders texture at given point
		void render( int x, int y );

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

//Starts up SDL and creates window
bool init_color_keying();

//Loads media
bool loadMedia_color_keying();

//Frees media and shuts down SDL
void close_color_keying();

//The window we'll be rendering to
SDL_Window* gWindow_color_keying = NULL;

//The window renderer
SDL_Renderer* gRenderer_color_keying = NULL;

//Scene textures
LTexture_color_keying gFooTexture_color_keying;
LTexture_color_keying gBackgroundTexture_color_keying;


LTexture_color_keying::LTexture_color_keying()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture_color_keying::~LTexture_color_keying()
{
	//Deallocate
	free();
}

bool LTexture_color_keying::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

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
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer_color_keying, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

void LTexture_color_keying::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture_color_keying::render( int x, int y )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
	SDL_RenderCopy( gRenderer_color_keying, mTexture, NULL, &renderQuad );
}

int LTexture_color_keying::getWidth()
{
	return mWidth;
}

int LTexture_color_keying::getHeight()
{
	return mHeight;
}

bool init_color_keying()
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
		gWindow_color_keying = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_color_keying == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer_color_keying = SDL_CreateRenderer( gWindow_color_keying, -1, SDL_RENDERER_ACCELERATED );
			if( gRenderer_color_keying == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer_color_keying, 0xFF, 0xFF, 0xFF, 0xFF );

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

bool loadMedia_color_keying()
{
	//Loading success flag
	bool success = true;

	//Load Foo' texture
	if( !gFooTexture_color_keying.loadFromFile( "foo.png" ) )
	{
		printf( "Failed to load Foo' texture image!\n" );
		success = false;
	}
	
	//Load background texture
	if( !gBackgroundTexture_color_keying.loadFromFile( "background.png" ) )
	{
		printf( "Failed to load background texture image!\n" );
		success = false;
	}

	return success;
}

void close_color_keying()
{
	//Free loaded images
	gFooTexture_color_keying.free();
	gBackgroundTexture_color_keying.free();

	//Destroy window	
	SDL_DestroyRenderer( gRenderer_color_keying);
	SDL_DestroyWindow( gWindow_color_keying);
	gWindow_color_keying = NULL;
	gRenderer_color_keying = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main_color_keying( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init_color_keying() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia_color_keying() )
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
				SDL_SetRenderDrawColor( gRenderer_color_keying, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer_color_keying);

				//Render background texture to screen
				gBackgroundTexture_color_keying.render( 0, 0 );

				//Render Foo' to the screen
				gFooTexture_color_keying.render( 240, 190 );

				//Update screen
				SDL_RenderPresent( gRenderer_color_keying);
			}
		}
	}

	//Free resources and close SDL
	close_color_keying();

	return 0;
}