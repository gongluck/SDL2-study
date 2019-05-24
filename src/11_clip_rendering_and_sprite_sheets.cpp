/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Texture wrapper class
class LTexture_clip_rendering_and_sprite_sheets
{
	public:
		//Initializes variables
		LTexture_clip_rendering_and_sprite_sheets();

		//Deallocates memory
		~LTexture_clip_rendering_and_sprite_sheets();

		//Loads image at specified path
		bool loadFromFile( std::string path );

		//Deallocates texture
		void free();

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
bool init_clip_rendering_and_sprite_sheets();

//Loads media
bool loadMedia_clip_rendering_and_sprite_sheets();

//Frees media and shuts down SDL
void close_clip_rendering_and_sprite_sheets();

//The window we'll be rendering to
SDL_Window* gWindow_clip_rendering_and_sprite_sheets = NULL;

//The window renderer
SDL_Renderer* gRenderer_clip_rendering_and_sprite_sheets = NULL;

//Scene sprites
SDL_Rect gSpriteClips_clip_rendering_and_sprite_sheets[ 4 ];
LTexture_clip_rendering_and_sprite_sheets gSpriteSheetTexture_clip_rendering_and_sprite_sheets;


LTexture_clip_rendering_and_sprite_sheets::LTexture_clip_rendering_and_sprite_sheets()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture_clip_rendering_and_sprite_sheets::~LTexture_clip_rendering_and_sprite_sheets()
{
	//Deallocate
	free();
}

bool LTexture_clip_rendering_and_sprite_sheets::loadFromFile( std::string path )
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
        newTexture = SDL_CreateTextureFromSurface( gRenderer_clip_rendering_and_sprite_sheets, loadedSurface );
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

void LTexture_clip_rendering_and_sprite_sheets::free()
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

void LTexture_clip_rendering_and_sprite_sheets::render( int x, int y, SDL_Rect* clip )
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
	SDL_RenderCopy( gRenderer_clip_rendering_and_sprite_sheets, mTexture, clip, &renderQuad );
}

int LTexture_clip_rendering_and_sprite_sheets::getWidth()
{
	return mWidth;
}

int LTexture_clip_rendering_and_sprite_sheets::getHeight()
{
	return mHeight;
}

bool init_clip_rendering_and_sprite_sheets()
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
		gWindow_clip_rendering_and_sprite_sheets = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_clip_rendering_and_sprite_sheets == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer_clip_rendering_and_sprite_sheets = SDL_CreateRenderer( gWindow_clip_rendering_and_sprite_sheets, -1, SDL_RENDERER_ACCELERATED );
			if( gRenderer_clip_rendering_and_sprite_sheets == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer_clip_rendering_and_sprite_sheets, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_mage Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia_clip_rendering_and_sprite_sheets()
{
	//Loading success flag
	bool success = true;

	//Load sprite sheet texture
	if( !gSpriteSheetTexture_clip_rendering_and_sprite_sheets.loadFromFile( "dots.png" ) )
	{
		printf( "Failed to load sprite sheet texture!\n" );
		success = false;
	}
	else
	{
		//Set top left sprite
		gSpriteClips_clip_rendering_and_sprite_sheets[ 0 ].x =   0;
		gSpriteClips_clip_rendering_and_sprite_sheets[ 0 ].y =   0;
		gSpriteClips_clip_rendering_and_sprite_sheets[ 0 ].w = 100;
		gSpriteClips_clip_rendering_and_sprite_sheets[ 0 ].h = 100;

		//Set top right sprite
		gSpriteClips_clip_rendering_and_sprite_sheets[ 1 ].x = 100;
		gSpriteClips_clip_rendering_and_sprite_sheets[ 1 ].y =   0;
		gSpriteClips_clip_rendering_and_sprite_sheets[ 1 ].w = 100;
		gSpriteClips_clip_rendering_and_sprite_sheets[ 1 ].h = 100;
		
		//Set bottom left sprite
		gSpriteClips_clip_rendering_and_sprite_sheets[ 2 ].x =   0;
		gSpriteClips_clip_rendering_and_sprite_sheets[ 2 ].y = 100;
		gSpriteClips_clip_rendering_and_sprite_sheets[ 2 ].w = 100;
		gSpriteClips_clip_rendering_and_sprite_sheets[ 2 ].h = 100;

		//Set bottom right sprite
		gSpriteClips_clip_rendering_and_sprite_sheets[ 3 ].x = 100;
		gSpriteClips_clip_rendering_and_sprite_sheets[ 3 ].y = 100;
		gSpriteClips_clip_rendering_and_sprite_sheets[ 3 ].w = 100;
		gSpriteClips_clip_rendering_and_sprite_sheets[ 3 ].h = 100;
	}

	return success;
}

void close_clip_rendering_and_sprite_sheets()
{
	//Free loaded images
	gSpriteSheetTexture_clip_rendering_and_sprite_sheets.free();

	//Destroy window	
	SDL_DestroyRenderer( gRenderer_clip_rendering_and_sprite_sheets);
	SDL_DestroyWindow( gWindow_clip_rendering_and_sprite_sheets);
	gWindow_clip_rendering_and_sprite_sheets = NULL;
	gRenderer_clip_rendering_and_sprite_sheets = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init_clip_rendering_and_sprite_sheets() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia_clip_rendering_and_sprite_sheets() )
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
				SDL_SetRenderDrawColor( gRenderer_clip_rendering_and_sprite_sheets, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer_clip_rendering_and_sprite_sheets);

				//Render top left sprite
				gSpriteSheetTexture_clip_rendering_and_sprite_sheets.render( 0, 0, &gSpriteClips_clip_rendering_and_sprite_sheets[ 0 ] );

				//Render top right sprite
				gSpriteSheetTexture_clip_rendering_and_sprite_sheets.render( SCREEN_WIDTH - gSpriteClips_clip_rendering_and_sprite_sheets[ 1 ].w, 0, &gSpriteClips_clip_rendering_and_sprite_sheets[ 1 ] );

				//Render bottom left sprite
				gSpriteSheetTexture_clip_rendering_and_sprite_sheets.render( 0, SCREEN_HEIGHT - gSpriteClips_clip_rendering_and_sprite_sheets[ 2 ].h, &gSpriteClips_clip_rendering_and_sprite_sheets[ 2 ] );

				//Render bottom right sprite
				gSpriteSheetTexture_clip_rendering_and_sprite_sheets.render( SCREEN_WIDTH - gSpriteClips_clip_rendering_and_sprite_sheets[ 3 ].w, SCREEN_HEIGHT - gSpriteClips_clip_rendering_and_sprite_sheets[ 3 ].h, &gSpriteClips_clip_rendering_and_sprite_sheets[ 3 ] );

				//Update screen
				SDL_RenderPresent( gRenderer_clip_rendering_and_sprite_sheets);
			}
		}
	}

	//Free resources and close SDL
	close_clip_rendering_and_sprite_sheets();

	return 0;
}