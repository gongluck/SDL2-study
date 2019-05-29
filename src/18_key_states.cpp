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
class LTexture_key_states
{
	public:
		//Initializes variables
		LTexture_key_states();

		//Deallocates memory
		~LTexture_key_states();

		//Loads image at specified path
		bool loadFromFile( std::string path );
		
		#ifdef _SDL_TTF_H
		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
		#endif
		
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
bool init_key_states();

//Loads media
bool loadMedia_key_states();

//Frees media and shuts down SDL
void close_key_states();

//The window we'll be rendering to
SDL_Window* gWindow_key_states = NULL;

//The window renderer
SDL_Renderer* gRenderer_key_states = NULL;

//Scene textures
LTexture_key_states gPressTexture_key_states;
LTexture_key_states gUpTexture_key_states;
LTexture_key_states gDownTexture_key_states;
LTexture_key_states gLeftTexture_key_states;
LTexture_key_states gRightTexture_key_states;

LTexture_key_states::LTexture_key_states()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture_key_states::~LTexture_key_states()
{
	//Deallocate
	free();
}

bool LTexture_key_states::loadFromFile( std::string path )
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
        newTexture = SDL_CreateTextureFromSurface( gRenderer_key_states, loadedSurface );
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

#ifdef _SDL_TTF_H
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

void LTexture_key_states::free()
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

void LTexture_key_states::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture_key_states::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture_key_states::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture_key_states::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
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
	SDL_RenderCopyEx( gRenderer_key_states, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture_key_states::getWidth()
{
	return mWidth;
}

int LTexture_key_states::getHeight()
{
	return mHeight;
}

bool init_key_states()
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
		gWindow_key_states = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_key_states == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer_key_states = SDL_CreateRenderer( gWindow_key_states, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer_key_states == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer_key_states, 0xFF, 0xFF, 0xFF, 0xFF );

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

bool loadMedia_key_states()
{
	//Loading success flag
	bool success = true;

	//Load press texture
	if( !gPressTexture_key_states.loadFromFile( "press.png" ) )
	{
		printf( "Failed to load press texture!\n" );
		success = false;
	}
	
	//Load up texture
	if( !gUpTexture_key_states.loadFromFile( "up.png" ) )
	{
		printf( "Failed to load up texture!\n" );
		success = false;
	}

	//Load down texture
	if( !gDownTexture_key_states.loadFromFile( "down.png" ) )
	{
		printf( "Failed to load down texture!\n" );
		success = false;
	}

	//Load left texture
	if( !gLeftTexture_key_states.loadFromFile( "left.png" ) )
	{
		printf( "Failed to load left texture!\n" );
		success = false;
	}

	//Load right texture
	if( !gRightTexture_key_states.loadFromFile( "right.png" ) )
	{
		printf( "Failed to load right texture!\n" );
		success = false;
	}

	return success;
}

void close_key_states()
{
	//Free loaded images
	gPressTexture_key_states.free();
	gUpTexture_key_states.free();
	gDownTexture_key_states.free();
	gLeftTexture_key_states.free();
	gRightTexture_key_states.free();

	//Destroy window	
	SDL_DestroyRenderer( gRenderer_key_states);
	SDL_DestroyWindow( gWindow_key_states);
	gWindow_key_states = NULL;
	gRenderer_key_states = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init_key_states() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia_key_states() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//Current rendered texture
			LTexture_key_states* currentTexture = NULL;

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

				//Set texture based on current keystate
				const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );
				if( currentKeyStates[ SDL_SCANCODE_UP ] )
				{
					currentTexture = &gUpTexture_key_states;
				}
				else if( currentKeyStates[ SDL_SCANCODE_DOWN ] )
				{
					currentTexture = &gDownTexture_key_states;
				}
				else if( currentKeyStates[ SDL_SCANCODE_LEFT ] )
				{
					currentTexture = &gLeftTexture_key_states;
				}
				else if( currentKeyStates[ SDL_SCANCODE_RIGHT ] )
				{
					currentTexture = &gRightTexture_key_states;
				}
				else
				{
					currentTexture = &gPressTexture_key_states;
				}

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer_key_states, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer_key_states);

				//Render current texture
				currentTexture->render( 0, 0 );

				//Update screen
				SDL_RenderPresent( gRenderer_key_states);
			}
		}
	}

	//Free resources and close SDL
	close_key_states();

	return 0;
}