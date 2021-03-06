/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL, SDL Threads, SDL_image, standard IO, and, strings
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Texture wrapper class
class LTexture_atomic_operations
{
	public:
		//Initializes variables
		LTexture_atomic_operations();

		//Deallocates memory
		~LTexture_atomic_operations();

		//Loads image at specified path
		bool loadFromFile( std::string path );
		
		#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
		#endif

		//Creates blank texture
		bool createBlank( int width, int height, SDL_TextureAccess = SDL_TEXTUREACCESS_STREAMING );

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

		//Set self as render target
		void setAsRenderTarget();

		//Gets image dimensions
		int getWidth();
		int getHeight();

		//Pixel manipulators
		bool lockTexture();
		bool unlockTexture();
		void* getPixels();
		void copyPixels( void* pixels );
		int getPitch();
		Uint32 getPixel32( unsigned int x, unsigned int y );

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;
		void* mPixels;
		int mPitch;

		//Image dimensions
		int mWidth;
		int mHeight;
};

//Starts up SDL and creates window
bool init_atomic_operations();

//Loads media
bool loadMedia_atomic_operations();

//Frees media and shuts down SDL
void close_atomic_operations();

//Our worker function
int worker_atomic_operations( void* data );

//The window we'll be rendering to
SDL_Window* gWindow_atomic_operations = NULL;

//The window renderer
SDL_Renderer* gRenderer_atomic_operations = NULL;

//Scene textures
LTexture_atomic_operations gSplashTexture_atomic_operations;

//Data access spin lock
SDL_SpinLock gDataLock_atomic_operations = 0;

//The "data buffer"
int gData_atomic_operations = -1;

LTexture_atomic_operations::LTexture_atomic_operations()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
	mPixels = NULL;
	mPitch = 0;
}

LTexture_atomic_operations::~LTexture_atomic_operations()
{
	//Deallocate
	free();
}

bool LTexture_atomic_operations::loadFromFile( std::string path )
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
		//Convert surface to display format
		SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat( loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0 );
		if( formattedSurface == NULL )
		{
			printf( "Unable to convert loaded surface to display format! %s\n", SDL_GetError() );
		}
		else
		{
			//Create blank streamable texture
			newTexture = SDL_CreateTexture( gRenderer_atomic_operations, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h );
			if( newTexture == NULL )
			{
				printf( "Unable to create blank texture! SDL Error: %s\n", SDL_GetError() );
			}
			else
			{
				//Enable blending on texture
				SDL_SetTextureBlendMode( newTexture, SDL_BLENDMODE_BLEND );

				//Lock texture for manipulation
				SDL_LockTexture( newTexture, &formattedSurface->clip_rect, &mPixels, &mPitch );

				//Copy loaded/formatted surface pixels
				memcpy( mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h );

				//Get image dimensions
				mWidth = formattedSurface->w;
				mHeight = formattedSurface->h;

				//Get pixel data in editable format
				Uint32* pixels = (Uint32*)mPixels;
				int pixelCount = ( mPitch / 4 ) * mHeight;

				//Map colors				
				Uint32 colorKey = SDL_MapRGB( formattedSurface->format, 0, 0xFF, 0xFF );
				Uint32 transparent = SDL_MapRGBA( formattedSurface->format, 0x00, 0xFF, 0xFF, 0x00 );

				//Color key pixels
				for( int i = 0; i < pixelCount; ++i )
				{
					if( pixels[ i ] == colorKey )
					{
						pixels[ i ] = transparent;
					}
				}

				//Unlock texture to update
				SDL_UnlockTexture( newTexture );
				mPixels = NULL;
			}

			//Get rid of old formatted surface
			SDL_FreeSurface( formattedSurface );
		}	
		
		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}

	
	//Return success
	return mTexture != NULL;
}
#endif
		
bool LTexture_atomic_operations::createBlank( int width, int height, SDL_TextureAccess access )
{
	//Create uninitialized texture
	mTexture = SDL_CreateTexture( gRenderer_atomic_operations, SDL_PIXELFORMAT_RGBA8888, access, width, height );
	if( mTexture == NULL )
	{
		printf( "Unable to create blank texture! SDL Error: %s\n", SDL_GetError() );
	}
	else
	{
		mWidth = width;
		mHeight = height;
	}

	return mTexture != NULL;
}

void LTexture_atomic_operations::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
		mPixels = NULL;
		mPitch = 0;
	}
}

void LTexture_atomic_operations::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture_atomic_operations::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture_atomic_operations::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture_atomic_operations::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
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
	SDL_RenderCopyEx( gRenderer_atomic_operations, mTexture, clip, &renderQuad, angle, center, flip );
}

void LTexture_atomic_operations::setAsRenderTarget()
{
	//Make self render target
	SDL_SetRenderTarget( gRenderer_atomic_operations, mTexture );
}

int LTexture_atomic_operations::getWidth()
{
	return mWidth;
}

int LTexture_atomic_operations::getHeight()
{
	return mHeight;
}

bool LTexture_atomic_operations::lockTexture()
{
	bool success = true;

	//Texture is already locked
	if( mPixels != NULL )
	{
		printf( "Texture is already locked!\n" );
		success = false;
	}
	//Lock texture
	else
	{
		if( SDL_LockTexture( mTexture, NULL, &mPixels, &mPitch ) != 0 )
		{
			printf( "Unable to lock texture! %s\n", SDL_GetError() );
			success = false;
		}
	}

	return success;
}

bool LTexture_atomic_operations::unlockTexture()
{
	bool success = true;

	//Texture is not locked
	if( mPixels == NULL )
	{
		printf( "Texture is not locked!\n" );
		success = false;
	}
	//Unlock texture
	else
	{
		SDL_UnlockTexture( mTexture );
		mPixels = NULL;
		mPitch = 0;
	}

	return success;
}

void* LTexture_atomic_operations::getPixels()
{
	return mPixels;
}

void LTexture_atomic_operations::copyPixels( void* pixels )
{
	//Texture is locked
	if( mPixels != NULL )
	{
		//Copy to locked pixels
		memcpy( mPixels, pixels, mPitch * mHeight );
	}
}

int LTexture_atomic_operations::getPitch()
{
	return mPitch;
}

Uint32 LTexture_atomic_operations::getPixel32( unsigned int x, unsigned int y )
{
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32*)mPixels;

    //Get the pixel requested
    return pixels[ ( y * ( mPitch / 4 ) ) + x ];
}

bool init_atomic_operations()
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
		gWindow_atomic_operations = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_atomic_operations == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer_atomic_operations = SDL_CreateRenderer( gWindow_atomic_operations, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer_atomic_operations == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer_atomic_operations, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia_atomic_operations()
{
	//Loading success flag
	bool success = true;
	
	//Load splash texture
	if( !gSplashTexture_atomic_operations.loadFromFile( "splash.png" ) )
	{
		printf( "Failed to load splash texture!\n" );
		success = false;
	}

	return success;
}

void close_atomic_operations()
{
	//Free loaded images
	gSplashTexture_atomic_operations.free();

	//Destroy window	
	SDL_DestroyRenderer( gRenderer_atomic_operations);
	SDL_DestroyWindow( gWindow_atomic_operations);
	gWindow_atomic_operations = NULL;
	gRenderer_atomic_operations = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int worker_atomic_operations( void* data )
{
	printf( "%s starting...\n", data );

	//Pre thread random seeding
	srand( SDL_GetTicks() );
	
	//Work 5 times
	for( int i = 0; i < 5; ++i )
	{
		//Wait randomly
		SDL_Delay( 16 + rand() % 32 );
		
		//Lock
		SDL_AtomicLock( &gDataLock_atomic_operations);

		//Print pre work data
		printf( "%s gets %d\n", data, gData_atomic_operations);

		//"Work"
		gData_atomic_operations = rand() % 256;

		//Print post work data
		printf( "%s sets %d\n\n", data, gData_atomic_operations);
		
		//Unlock
		SDL_AtomicUnlock( &gDataLock_atomic_operations);

		//Wait randomly
		SDL_Delay( 16 + rand() % 640 );
	}

	printf( "%s finished!\n\n", data );

	return 0;
}


int main_atomic_operations( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init_atomic_operations() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia_atomic_operations() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//Run the threads
			srand( SDL_GetTicks() );
			SDL_Thread* threadA = SDL_CreateThread( worker_atomic_operations, "Thread A", (void*)"Thread A" );
			SDL_Delay( 16 + rand() % 32 );
			SDL_Thread* threadB = SDL_CreateThread( worker_atomic_operations, "Thread B", (void*)"Thread B" );
			
			//While application is running
			while( quit == false )
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
				SDL_SetRenderDrawColor( gRenderer_atomic_operations, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer_atomic_operations);

				//Render splash
				gSplashTexture_atomic_operations.render( 0, 0 );

				//Update screen
				SDL_RenderPresent( gRenderer_atomic_operations);
			}

			//Wait for threads to finish
			SDL_WaitThread( threadA, NULL );
			SDL_WaitThread( threadB, NULL );
		}
	}

	//Free resources and close SDL
	close_atomic_operations();

	return 0;
}