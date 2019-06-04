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

//Number of data integers
const int TOTAL_DATA = 10;

//Texture wrapper class
class LTexture_file_reading_and_writing
{
	public:
		//Initializes variables
		LTexture_file_reading_and_writing();

		//Deallocates memory
		~LTexture_file_reading_and_writing();

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
bool init_file_reading_and_writing();

//Loads media
bool loadMedia_file_reading_and_writing();

//Frees media and shuts down SDL
void close_file_reading_and_writing();

//The window we'll be rendering to
SDL_Window* gWindow_file_reading_and_writing = NULL;

//The window renderer
SDL_Renderer* gRenderer_file_reading_and_writing = NULL;

//Globally used font
TTF_Font *gFont_file_reading_and_writing = NULL;

//Scene textures
LTexture_file_reading_and_writing gPromptTextTexture_file_reading_and_writing;
LTexture_file_reading_and_writing gDataTextures_file_reading_and_writing[ TOTAL_DATA ];

//Data points
Sint32 gData_file_reading_and_writing[ TOTAL_DATA ];

LTexture_file_reading_and_writing::LTexture_file_reading_and_writing()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture_file_reading_and_writing::~LTexture_file_reading_and_writing()
{
	//Deallocate
	free();
}

bool LTexture_file_reading_and_writing::loadFromFile( std::string path )
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
        newTexture = SDL_CreateTextureFromSurface( gRenderer_file_reading_and_writing, loadedSurface );
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
bool LTexture_file_reading_and_writing::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont_file_reading_and_writing, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer_file_reading_and_writing, textSurface );
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

void LTexture_file_reading_and_writing::free()
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

void LTexture_file_reading_and_writing::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture_file_reading_and_writing::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture_file_reading_and_writing::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture_file_reading_and_writing::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
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
	SDL_RenderCopyEx( gRenderer_file_reading_and_writing, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture_file_reading_and_writing::getWidth()
{
	return mWidth;
}

int LTexture_file_reading_and_writing::getHeight()
{
	return mHeight;
}

bool init_file_reading_and_writing()
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
		gWindow_file_reading_and_writing = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_file_reading_and_writing == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer_file_reading_and_writing = SDL_CreateRenderer( gWindow_file_reading_and_writing, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer_file_reading_and_writing == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer_file_reading_and_writing, 0xFF, 0xFF, 0xFF, 0xFF );

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

bool loadMedia_file_reading_and_writing()
{
	//Text rendering color
	SDL_Color textColor = { 0, 0, 0, 0xFF };
	SDL_Color highlightColor = { 0xFF, 0, 0, 0xFF };
	
	//Loading success flag
	bool success = true;

	//Open the font
	gFont_file_reading_and_writing = TTF_OpenFont( "lazy.ttf", 28 );
	if( gFont_file_reading_and_writing == NULL )
	{
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}
	else
	{
		//Render the prompt
		if( !gPromptTextTexture_file_reading_and_writing.loadFromRenderedText( "Enter Data:", textColor ) )
		{
			printf( "Failed to render prompt text!\n" );
			success = false;
		}
	}

	//Open file for reading in binary
	SDL_RWops* file = SDL_RWFromFile( "nums.bin", "r+b" );

	//File does not exist
	if( file == NULL )
	{
		printf( "Warning: Unable to open file! SDL Error: %s\n", SDL_GetError() );
		
		//Create file for writing
		file = SDL_RWFromFile( "nums.bin", "w+b" );
		if( file != NULL )
		{
			printf( "New file created!\n" );

			//Initialize data
			for( int i = 0; i < TOTAL_DATA; ++i )
			{
				gData_file_reading_and_writing[ i ] = 0;
				SDL_RWwrite( file, &gData_file_reading_and_writing[ i ], sizeof(Sint32), 1 );
			}
			
			//Close file handler
			SDL_RWclose( file );
		}
		else
		{
			printf( "Error: Unable to create file! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
	}
	//File exists
	else
	{
		//Load data
		printf( "Reading file...!\n" );
		for( int i = 0; i < TOTAL_DATA; ++i )
		{
			SDL_RWread( file, &gData_file_reading_and_writing[ i ], sizeof(Sint32), 1 );
		}

		//Close file handler
		SDL_RWclose( file );
	}

	//Initialize data textures
	gDataTextures_file_reading_and_writing[ 0 ].loadFromRenderedText( std::to_string( gData_file_reading_and_writing[ 0 ] ), highlightColor );
	for( int i = 1; i < TOTAL_DATA; ++i )
	{
		gDataTextures_file_reading_and_writing[ i ].loadFromRenderedText( std::to_string( gData_file_reading_and_writing[ i ] ), textColor );
	}

	return success;
}

void close_file_reading_and_writing()
{
	//Open data for writing
	SDL_RWops* file = SDL_RWFromFile( "nums.bin", "w+b" );
	if( file != NULL )
	{
		//Save data
		for( int i = 0; i < TOTAL_DATA; ++i )
		{
			SDL_RWwrite( file, &gData_file_reading_and_writing[ i ], sizeof(Sint32), 1 );
		}

		//Close file handler
		SDL_RWclose( file );
	}
	else
	{
		printf( "Error: Unable to save file! %s\n", SDL_GetError() );
	}

	//Free loaded images
	gPromptTextTexture_file_reading_and_writing.free();
	for( int i = 0; i < TOTAL_DATA; ++i )
	{
		gDataTextures_file_reading_and_writing[ i ].free();
	}

	//Free global font
	TTF_CloseFont( gFont_file_reading_and_writing);
	gFont_file_reading_and_writing = NULL;

	//Destroy window	
	SDL_DestroyRenderer( gRenderer_file_reading_and_writing);
	SDL_DestroyWindow( gWindow_file_reading_and_writing);
	gWindow_file_reading_and_writing = NULL;
	gRenderer_file_reading_and_writing = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init_file_reading_and_writing() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia_file_reading_and_writing() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//Text rendering color
			SDL_Color textColor = { 0, 0, 0, 0xFF };
			SDL_Color highlightColor = { 0xFF, 0, 0, 0xFF };

			//Current input point
			int currentData = 0;

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
							//Previous data entry
							case SDLK_UP:
							//Rerender previous entry input point
							gDataTextures_file_reading_and_writing[ currentData ].loadFromRenderedText( std::to_string( gData_file_reading_and_writing[ currentData ] ), textColor );
							--currentData;
							if( currentData < 0 )
							{
								currentData = TOTAL_DATA - 1;
							}
							
							//Rerender current entry input point
							gDataTextures_file_reading_and_writing[ currentData ].loadFromRenderedText( std::to_string( gData_file_reading_and_writing[ currentData ] ), highlightColor );
							break;
							
							//Next data entry
							case SDLK_DOWN:
							//Rerender previous entry input point
							gDataTextures_file_reading_and_writing[ currentData ].loadFromRenderedText( std::to_string( gData_file_reading_and_writing[ currentData ] ), textColor );
							++currentData;
							if( currentData == TOTAL_DATA )
							{
								currentData = 0;
							}
							
							//Rerender current entry input point
							gDataTextures_file_reading_and_writing[ currentData ].loadFromRenderedText( std::to_string( gData_file_reading_and_writing[ currentData ] ), highlightColor );
							break;

							//Decrement input point
							case SDLK_LEFT:
							--gData_file_reading_and_writing[ currentData ];
							gDataTextures_file_reading_and_writing[ currentData ].loadFromRenderedText( std::to_string( gData_file_reading_and_writing[ currentData ] ), highlightColor );
							break;
							
							//Increment input point
							case SDLK_RIGHT:
							++gData_file_reading_and_writing[ currentData ];
							gDataTextures_file_reading_and_writing[ currentData ].loadFromRenderedText( std::to_string( gData_file_reading_and_writing[ currentData ] ), highlightColor );
							break;
						}
					}
				}

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer_file_reading_and_writing, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer_file_reading_and_writing);

				//Render text textures
				gPromptTextTexture_file_reading_and_writing.render( ( SCREEN_WIDTH - gPromptTextTexture_file_reading_and_writing.getWidth() ) / 2, 0 );
				for( int i = 0; i < TOTAL_DATA; ++i )
				{
					gDataTextures_file_reading_and_writing[ i ].render( ( SCREEN_WIDTH - gDataTextures_file_reading_and_writing[ i ].getWidth() ) / 2, gPromptTextTexture_file_reading_and_writing.getHeight() + gDataTextures_file_reading_and_writing[ 0 ].getHeight() * i );
				}

				//Update screen
				SDL_RenderPresent( gRenderer_file_reading_and_writing);
			}
		}
	}

	//Free resources and close SDL
	close_file_reading_and_writing();

	return 0;
}