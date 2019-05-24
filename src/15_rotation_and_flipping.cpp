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

//Texture wrapper class
class LTexture_rotation_and_flipping
{
	public:
		//Initializes variables
		LTexture_rotation_and_flipping();

		//Deallocates memory
		~LTexture_rotation_and_flipping();

		//Loads image at specified path
		bool loadFromFile( std::string path );

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );
		
		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

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
bool init_rotation_and_flipping();

//Loads media
bool loadMedia_rotation_and_flipping();

//Frees media and shuts down SDL
void close_rotation_and_flipping();

//The window we'll be rendering to
SDL_Window* gWindow_rotation_and_flipping = NULL;

//The window renderer
SDL_Renderer* gRenderer_rotation_and_flipping = NULL;

//Scene texture
LTexture_rotation_and_flipping gArrowTexture_rotation_and_flipping;


LTexture_rotation_and_flipping::LTexture_rotation_and_flipping()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture_rotation_and_flipping::~LTexture_rotation_and_flipping()
{
	//Deallocate
	free();
}

bool LTexture_rotation_and_flipping::loadFromFile( std::string path )
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
        newTexture = SDL_CreateTextureFromSurface( gRenderer_rotation_and_flipping, loadedSurface );
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

void LTexture_rotation_and_flipping::free()
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

void LTexture_rotation_and_flipping::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture_rotation_and_flipping::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture_rotation_and_flipping::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture_rotation_and_flipping::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( gRenderer_rotation_and_flipping, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture_rotation_and_flipping::getWidth()
{
	return mWidth;
}

int LTexture_rotation_and_flipping::getHeight()
{
	return mHeight;
}

bool init_rotation_and_flipping()
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
		gWindow_rotation_and_flipping = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_rotation_and_flipping == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer_rotation_and_flipping = SDL_CreateRenderer( gWindow_rotation_and_flipping, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer_rotation_and_flipping == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer_rotation_and_flipping, 0xFF, 0xFF, 0xFF, 0xFF );

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

bool loadMedia_rotation_and_flipping()
{
	//Loading success flag
	bool success = true;

	//Load arrow
	if( !gArrowTexture_rotation_and_flipping.loadFromFile( "arrow.png" ) )
	{
		printf( "Failed to load arrow texture!\n" );
		success = false;
	}

	return success;
}

void close_rotation_and_flipping()
{
	//Free loaded images
	gArrowTexture_rotation_and_flipping.free();

	//Destroy window	
	SDL_DestroyRenderer( gRenderer_rotation_and_flipping);
	SDL_DestroyWindow( gWindow_rotation_and_flipping);
	gWindow_rotation_and_flipping = NULL;
	gRenderer_rotation_and_flipping = NULL;
	
	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init_rotation_and_flipping() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia_rotation_and_flipping() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//Angle of rotation
			double degrees = 0;

			//Flip type
			SDL_RendererFlip flipType = SDL_FLIP_NONE;

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
					else if( e.type == SDL_KEYDOWN )
					{
						switch( e.key.keysym.sym )
						{
							case SDLK_a:
							degrees -= 60;
							break;
							
							case SDLK_d:
							degrees += 60;
							break;

							case SDLK_q:
							flipType = SDL_FLIP_HORIZONTAL;
							break;

							case SDLK_w:
							flipType = SDL_FLIP_NONE;
							break;

							case SDLK_e:
							flipType = SDL_FLIP_VERTICAL;
							break;
						}
					}
				}

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer_rotation_and_flipping, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer_rotation_and_flipping);

				//Render arrow
				gArrowTexture_rotation_and_flipping.render( ( SCREEN_WIDTH - gArrowTexture_rotation_and_flipping.getWidth() ) / 2, ( SCREEN_HEIGHT - gArrowTexture_rotation_and_flipping.getHeight() ) / 2, NULL, degrees, NULL, flipType );

				//Update screen
				SDL_RenderPresent( gRenderer_rotation_and_flipping);
			}
		}
	}

	//Free resources and close SDL
	close_rotation_and_flipping
    
    ();

	return 0;
}