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
const int SCREEN_FPS = 60;

//Texture wrapper class
class LTexture_mutexes_and_conditions
{
	public:
		//Initializes variables
		LTexture_mutexes_and_conditions();

		//Deallocates memory
		~LTexture_mutexes_and_conditions();

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
bool init_mutexes_and_conditions();

//Loads media
bool loadMedia_mutexes_and_conditions();

//Frees media and shuts down SDL
void close_mutexes_and_conditions();

//Our worker functions
int producer_mutexes_and_conditions( void* data );
int consumer_mutexes_and_conditions( void* data );
void produce_mutexes_and_conditions();
void consume_mutexes_and_conditions();

//The window we'll be rendering to
SDL_Window* gWindow_mutexes_and_conditions = NULL;

//The window renderer
SDL_Renderer* gRenderer_mutexes_and_conditions = NULL;

//Scene textures
LTexture_mutexes_and_conditions gSplashTexture_mutexes_and_conditions;

//The protective mutex
SDL_mutex* gBufferLock_mutexes_and_conditions = NULL;

//The conditions
SDL_cond* gCanProduce_mutexes_and_conditions = NULL;
SDL_cond* gCanConsume_mutexes_and_conditions = NULL;

//The "data buffer"
int gData_mutexes_and_conditions = -1;

LTexture_mutexes_and_conditions::LTexture_mutexes_and_conditions()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
	mPixels = NULL;
	mPitch = 0;
}

LTexture_mutexes_and_conditions::~LTexture_mutexes_and_conditions()
{
	//Deallocate
	free();
}

bool LTexture_mutexes_and_conditions::loadFromFile( std::string path )
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
			newTexture = SDL_CreateTexture( gRenderer_mutexes_and_conditions, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h );
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
		
bool LTexture_mutexes_and_conditions::createBlank( int width, int height, SDL_TextureAccess access )
{
	//Create uninitialized texture
	mTexture = SDL_CreateTexture( gRenderer_mutexes_and_conditions, SDL_PIXELFORMAT_RGBA8888, access, width, height );
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

void LTexture_mutexes_and_conditions::free()
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

void LTexture_mutexes_and_conditions::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture_mutexes_and_conditions::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture_mutexes_and_conditions::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture_mutexes_and_conditions::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
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
	SDL_RenderCopyEx( gRenderer_mutexes_and_conditions, mTexture, clip, &renderQuad, angle, center, flip );
}

void LTexture_mutexes_and_conditions::setAsRenderTarget()
{
	//Make self render target
	SDL_SetRenderTarget( gRenderer_mutexes_and_conditions, mTexture );
}

int LTexture_mutexes_and_conditions::getWidth()
{
	return mWidth;
}

int LTexture_mutexes_and_conditions::getHeight()
{
	return mHeight;
}

bool LTexture_mutexes_and_conditions::lockTexture()
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

bool LTexture_mutexes_and_conditions::unlockTexture()
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

void* LTexture_mutexes_and_conditions::getPixels()
{
	return mPixels;
}

void LTexture_mutexes_and_conditions::copyPixels( void* pixels )
{
	//Texture is locked
	if( mPixels != NULL )
	{
		//Copy to locked pixels
		memcpy( mPixels, pixels, mPitch * mHeight );
	}
}

int LTexture_mutexes_and_conditions::getPitch()
{
	return mPitch;
}

Uint32 LTexture_mutexes_and_conditions::getPixel32( unsigned int x, unsigned int y )
{
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32*)mPixels;

    //Get the pixel requested
    return pixels[ ( y * ( mPitch / 4 ) ) + x ];
}

bool init_mutexes_and_conditions()
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
		gWindow_mutexes_and_conditions = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_mutexes_and_conditions == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer_mutexes_and_conditions = SDL_CreateRenderer( gWindow_mutexes_and_conditions, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer_mutexes_and_conditions == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer_mutexes_and_conditions, 0xFF, 0xFF, 0xFF, 0xFF );

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

bool loadMedia_mutexes_and_conditions()
{
	//Create the mutex
	gBufferLock_mutexes_and_conditions = SDL_CreateMutex();
			
	//Create conditions
	gCanProduce_mutexes_and_conditions = SDL_CreateCond();
	gCanConsume_mutexes_and_conditions = SDL_CreateCond();

	//Loading success flag
	bool success = true;
	
	//Load splash texture
	if( !gSplashTexture_mutexes_and_conditions.loadFromFile( "splash.png" ) )
	{
		printf( "Failed to load splash texture!\n" );
		success = false;
	}

	return success;
}

void close_mutexes_and_conditions()
{
	//Free loaded images
	gSplashTexture_mutexes_and_conditions.free();

	//Destroy the mutex
	SDL_DestroyMutex( gBufferLock_mutexes_and_conditions);
	gBufferLock_mutexes_and_conditions = NULL;
			
	//Destroy conditions
	SDL_DestroyCond( gCanProduce_mutexes_and_conditions);
	SDL_DestroyCond( gCanConsume_mutexes_and_conditions);
	gCanProduce_mutexes_and_conditions = NULL;
	gCanConsume_mutexes_and_conditions = NULL;

	//Destroy window	
	SDL_DestroyRenderer( gRenderer_mutexes_and_conditions);
	SDL_DestroyWindow( gWindow_mutexes_and_conditions);
	gWindow_mutexes_and_conditions = NULL;
	gRenderer_mutexes_and_conditions = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int producer_mutexes_and_conditions( void *data )
{
	printf( "\nProducer started...\n" );

	//Seed thread random
	srand( SDL_GetTicks() );
	
	//Produce
	for( int i = 0; i < 5; ++i )
	{
		//Wait
		SDL_Delay( rand() % 1000 );
		
		//Produce
		produce_mutexes_and_conditions();
	}

	printf( "\nProducer finished!\n" );
	
	return 0;

}

int consumer_mutexes_and_conditions( void *data )
{
	printf( "\nConsumer started...\n" );

	//Seed thread random
	srand( SDL_GetTicks() );

	for( int i = 0; i < 5; ++i )
	{
		//Wait
		SDL_Delay( rand() % 1000 );
		
		//Consume
		consume_mutexes_and_conditions();
	}
	
	printf( "\nConsumer finished!\n" );

	return 0;
}

void produce_mutexes_and_conditions()
{
	//Lock
	SDL_LockMutex( gBufferLock_mutexes_and_conditions);
	
	//If the buffer is full
	if( gData_mutexes_and_conditions != -1 )
	{
		//Wait for buffer to be cleared
		printf( "\nProducer encountered full buffer, waiting for consumer to empty buffer...\n" );
		SDL_CondWait( gCanProduce_mutexes_and_conditions, gBufferLock_mutexes_and_conditions);
	}

	//Fill and show buffer
	gData_mutexes_and_conditions = rand() % 255;
	printf( "\nProduced %d\n", gData_mutexes_and_conditions);
	
	//Unlock
	SDL_UnlockMutex( gBufferLock_mutexes_and_conditions);
	
	//Signal consumer
	SDL_CondSignal( gCanConsume_mutexes_and_conditions);
}

void consume_mutexes_and_conditions()
{
	//Lock
	SDL_LockMutex( gBufferLock_mutexes_and_conditions);
	
	//If the buffer is empty
	if( gData_mutexes_and_conditions == -1 )
	{
		//Wait for buffer to be filled
		printf( "\nConsumer encountered empty buffer, waiting for producer to fill buffer...\n" );
		SDL_CondWait( gCanConsume_mutexes_and_conditions, gBufferLock_mutexes_and_conditions);
	}

	//Show and empty buffer
	printf( "\nConsumed %d\n", gData_mutexes_and_conditions);
	gData_mutexes_and_conditions = -1;
	
	//Unlock
	SDL_UnlockMutex( gBufferLock_mutexes_and_conditions);
	
	//Signal producer
	SDL_CondSignal( gCanProduce_mutexes_and_conditions);
}

int main_mutexes_and_conditions( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init_mutexes_and_conditions() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia_mutexes_and_conditions() )
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
			SDL_Thread* producerThread = SDL_CreateThread( producer_mutexes_and_conditions, "Producer", NULL );
			SDL_Thread* consumerThread = SDL_CreateThread( consumer_mutexes_and_conditions, "Consumer", NULL );
			
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
				SDL_SetRenderDrawColor( gRenderer_mutexes_and_conditions, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer_mutexes_and_conditions);

				//Render splash
				gSplashTexture_mutexes_and_conditions.render( 0, 0 );

				//Update screen
				SDL_RenderPresent( gRenderer_mutexes_and_conditions);
			}

			//Wait for producer and consumer to finish
			SDL_WaitThread( consumerThread, NULL );
			SDL_WaitThread( producerThread, NULL );
		}
	}

	//Free resources and close SDL
	close_mutexes_and_conditions();

	return 0;
}