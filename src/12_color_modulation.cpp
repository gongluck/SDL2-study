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
class LTexture_color_modulation
{
	public:
		//Initializes variables
		LTexture_color_modulation();

		//Deallocates memory
		~LTexture_color_modulation();

		//Loads image at specified path
		bool loadFromFile( std::string path );

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL );

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
bool init_color_modulation();

//Loads media
bool loadMedia_color_modulation();

//Frees media and shuts down SDL
void close_color_modulation();

//The window we'll be rendering to
SDL_Window* gWindow_color_modulation = NULL;

//The window renderer
SDL_Renderer* gRenderer_color_modulation = NULL;

//Scene texture
LTexture_color_modulation gModulatedTexture_color_modulation;


LTexture_color_modulation::LTexture_color_modulation()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture_color_modulation::~LTexture_color_modulation()
{
	//Deallocate
	free();
}

bool LTexture_color_modulation::loadFromFile( std::string path )
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
        newTexture = SDL_CreateTextureFromSurface( gRenderer_color_modulation, loadedSurface );
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

void LTexture_color_modulation::free()
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

void LTexture_color_modulation::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture_color_modulation::render( int x, int y, SDL_Rect* clip )
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
	SDL_RenderCopy( gRenderer_color_modulation, mTexture, clip, &renderQuad );
}

int LTexture_color_modulation::getWidth()
{
	return mWidth;
}

int LTexture_color_modulation::getHeight()
{
	return mHeight;
}

bool init_color_modulation()
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
		gWindow_color_modulation = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_color_modulation == NULL )
		{
			printf( "Window could not be created! %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer_color_modulation = SDL_CreateRenderer( gWindow_color_modulation, -1, SDL_RENDERER_ACCELERATED );
			if( gRenderer_color_modulation == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer_color_modulation, 0xFF, 0xFF, 0xFF, 0xFF );

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

bool loadMedia_color_modulation()
{
	//Loading success flag
	bool success = true;

	//Load texture
	if( !gModulatedTexture_color_modulation.loadFromFile( "colors.png" ) )
	{
		printf( "Failed to load colors texture!\n" );
		success = false;
	}
	
	return success;
}

void close_color_modulation()
{
	//Free loaded images
	gModulatedTexture_color_modulation.free();

	//Destroy window	
	SDL_DestroyRenderer( gRenderer_color_modulation);
	SDL_DestroyWindow( gWindow_color_modulation);
	gWindow_color_modulation = NULL;
	gRenderer_color_modulation = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init_color_modulation() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia_color_modulation() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//Modulation components
			Uint8 r = 0;
			Uint8 g = 0;
			Uint8 b = 0;

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
					//On keypress change rgb values
					else if( e.type == SDL_KEYDOWN )
					{
						switch( e.key.keysym.sym )
						{
							//Increase red
							case SDLK_q:
							r += 32;
							break;
							
							//Increase green
							case SDLK_w:
							g += 32;
							break;
							
							//Increase blue
							case SDLK_e:
							b += 32;
							break;
							
							//Decrease red
							case SDLK_a:
							r -= 32;
							break;
							
							//Decrease green
							case SDLK_s:
							g -= 32;
							break;
							
							//Decrease blue
							case SDLK_d:
							b -= 32;
							break;
						}
					}
				}

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer_color_modulation, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer_color_modulation);

				//Modulate and render texture
				gModulatedTexture_color_modulation.setColor( r, g, b );
				gModulatedTexture_color_modulation.render( 0, 0 );

				//Update screen
				SDL_RenderPresent( gRenderer_color_modulation);
			}
		}
	}

	//Free resources and close SDL
	close_color_modulation();

	return 0;
}