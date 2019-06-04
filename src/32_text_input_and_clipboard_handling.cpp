/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, SDL_ttf, standard IO, strings, and string streams
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Texture wrapper class
class LTexture_text_input_and_clipboard_handling
{
	public:
		//Initializes variables
		LTexture_text_input_and_clipboard_handling();

		//Deallocates memory
		~LTexture_text_input_and_clipboard_handling();

		//Loads image at specified path
		bool loadFromFile( std::string path );
		
		#ifdef SDL_TTF_H_
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
bool init_text_input_and_clipboard_handling();

//Loads media
bool loadMedia_text_input_and_clipboard_handling();

//Frees media and shuts down SDL
void close_text_input_and_clipboard_handling();

//The window we'll be rendering to
SDL_Window* gWindow_text_input_and_clipboard_handling = NULL;

//The window renderer
SDL_Renderer* gRenderer_text_input_and_clipboard_handling = NULL;

//Globally used font
TTF_Font *gFont_text_input_and_clipboard_handling = NULL;

//Scene textures
LTexture_text_input_and_clipboard_handling gPromptTextTexture_text_input_and_clipboard_handling;
LTexture_text_input_and_clipboard_handling gInputTextTexture_text_input_and_clipboard_handling;

LTexture_text_input_and_clipboard_handling::LTexture_text_input_and_clipboard_handling()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture_text_input_and_clipboard_handling::~LTexture_text_input_and_clipboard_handling()
{
	//Deallocate
	free();
}

bool LTexture_text_input_and_clipboard_handling::loadFromFile( std::string path )
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
        newTexture = SDL_CreateTextureFromSurface( gRenderer_text_input_and_clipboard_handling, loadedSurface );
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

#ifdef SDL_TTF_H_
bool LTexture_text_input_and_clipboard_handling::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont_text_input_and_clipboard_handling, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer_text_input_and_clipboard_handling, textSurface );
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

void LTexture_text_input_and_clipboard_handling::free()
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

void LTexture_text_input_and_clipboard_handling::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture_text_input_and_clipboard_handling::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture_text_input_and_clipboard_handling::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture_text_input_and_clipboard_handling::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
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
	SDL_RenderCopyEx( gRenderer_text_input_and_clipboard_handling, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture_text_input_and_clipboard_handling::getWidth()
{
	return mWidth;
}

int LTexture_text_input_and_clipboard_handling::getHeight()
{
	return mHeight;
}

bool init_text_input_and_clipboard_handling()
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
		gWindow_text_input_and_clipboard_handling = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_text_input_and_clipboard_handling == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer_text_input_and_clipboard_handling = SDL_CreateRenderer( gWindow_text_input_and_clipboard_handling, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer_text_input_and_clipboard_handling == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer_text_input_and_clipboard_handling, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}

				 //Initialize SDL_ttf
				if( TTF_Init() == -1 )
				{
					printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia_text_input_and_clipboard_handling()
{
	//Loading success flag
	bool success = true;

	//Open the font
	gFont_text_input_and_clipboard_handling = TTF_OpenFont( "lazy.ttf", 28 );
	if( gFont_text_input_and_clipboard_handling == NULL )
	{
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}
	else
	{
		//Render the prompt
		SDL_Color textColor = { 0, 0, 0, 0xFF };
		if( !gPromptTextTexture_text_input_and_clipboard_handling.loadFromRenderedText( "Enter Text:", textColor ) )
		{
			printf( "Failed to render prompt text!\n" );
			success = false;
		}
	}

	return success;
}

void close_text_input_and_clipboard_handling()
{
	//Free loaded images
	gPromptTextTexture_text_input_and_clipboard_handling.free();
	gInputTextTexture_text_input_and_clipboard_handling.free();

	//Free global font
	TTF_CloseFont( gFont_text_input_and_clipboard_handling);
	gFont_text_input_and_clipboard_handling = NULL;

	//Destroy window	
	SDL_DestroyRenderer( gRenderer_text_input_and_clipboard_handling);
	SDL_DestroyWindow( gWindow_text_input_and_clipboard_handling);
	gWindow_text_input_and_clipboard_handling = NULL;
	gRenderer_text_input_and_clipboard_handling = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main_text_input_and_clipboard_handling( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init_text_input_and_clipboard_handling() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia_text_input_and_clipboard_handling() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//Set text color as black
			SDL_Color textColor = { 0, 0, 0, 0xFF };

			//The current input text.
			std::string inputText = "Some Text";
			gInputTextTexture_text_input_and_clipboard_handling.loadFromRenderedText( inputText.c_str(), textColor );

			//Enable text input
			SDL_StartTextInput();

			//While application is running
			while( !quit )
			{
				//The rerender text flag
				bool renderText = false;

				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
					//Special key input
					else if( e.type == SDL_KEYDOWN )
					{
						//Handle backspace
						if( e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0 )
						{
							//lop off character
							inputText.pop_back();
							renderText = true;
						}
						//Handle copy
						else if( e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
						{
							SDL_SetClipboardText( inputText.c_str() );
						}
						//Handle paste
						else if( e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
						{
							inputText = SDL_GetClipboardText();
							renderText = true;
						}
					}
					//Special text input event
					else if( e.type == SDL_TEXTINPUT )
					{
						//Not copy or pasting
						if( !( ( e.text.text[ 0 ] == 'c' || e.text.text[ 0 ] == 'C' ) && ( e.text.text[ 0 ] == 'v' || e.text.text[ 0 ] == 'V' ) && SDL_GetModState() & KMOD_CTRL ) )
						{
							//Append character
							inputText += e.text.text;
							renderText = true;
						}
					}
				}

				//Rerender text if needed
				if( renderText )
				{
					//Text is not empty
					if( inputText != "" )
					{
						//Render new text
						gInputTextTexture_text_input_and_clipboard_handling.loadFromRenderedText( inputText.c_str(), textColor );
					}
					//Text is empty
					else
					{
						//Render space texture
						gInputTextTexture_text_input_and_clipboard_handling.loadFromRenderedText( " ", textColor );
					}
				}

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer_text_input_and_clipboard_handling, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer_text_input_and_clipboard_handling);

				//Render text textures
				gPromptTextTexture_text_input_and_clipboard_handling.render( ( SCREEN_WIDTH - gPromptTextTexture_text_input_and_clipboard_handling.getWidth() ) / 2, 0 );
				gInputTextTexture_text_input_and_clipboard_handling.render( ( SCREEN_WIDTH - gInputTextTexture_text_input_and_clipboard_handling.getWidth() ) / 2, gPromptTextTexture_text_input_and_clipboard_handling.getHeight() );

				//Update screen
				SDL_RenderPresent( gRenderer_text_input_and_clipboard_handling);
			}
			
			//Disable text input
			SDL_StopTextInput();
		}
	}

	//Free resources and close SDL
	close_text_input_and_clipboard_handling();

	return 0;
}