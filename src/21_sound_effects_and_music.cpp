/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, SDL_ttf, SDL_mixer, standard IO, math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Texture wrapper class
class LTexture_sound_effects_and_music
{
	public:
		//Initializes variables
		LTexture_sound_effects_and_music();

		//Deallocates memory
		~LTexture_sound_effects_and_music();

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
bool init_sound_effects_and_music();

//Loads media
bool loadMedia_sound_effects_and_music();

//Frees media and shuts down SDL
void close_sound_effects_and_music();

//The window we'll be rendering to
SDL_Window* gWindow_sound_effects_and_music = NULL;

//The window renderer
SDL_Renderer* gRenderer_sound_effects_and_music = NULL;

//Scene texture
LTexture_sound_effects_and_music gPromptTexture_sound_effects_and_music;

//The music that will be played
Mix_Music *gMusic_sound_effects_and_music = NULL;

//The sound effects that will be used
Mix_Chunk *gScratch_sound_effects_and_music = NULL;
Mix_Chunk *gHigh_sound_effects_and_music = NULL;
Mix_Chunk *gMedium_sound_effects_and_music = NULL;
Mix_Chunk *gLow_sound_effects_and_music = NULL;


LTexture_sound_effects_and_music::LTexture_sound_effects_and_music()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture_sound_effects_and_music::~LTexture_sound_effects_and_music()
{
	//Deallocate
	free();
}

bool LTexture_sound_effects_and_music::loadFromFile( std::string path )
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
        newTexture = SDL_CreateTextureFromSurface( gRenderer_sound_effects_and_music, loadedSurface );
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

void LTexture_sound_effects_and_music::free()
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

void LTexture_sound_effects_and_music::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture_sound_effects_and_music::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture_sound_effects_and_music::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture_sound_effects_and_music::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
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
	SDL_RenderCopyEx( gRenderer_sound_effects_and_music, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture_sound_effects_and_music::getWidth()
{
	return mWidth;
}

int LTexture_sound_effects_and_music::getHeight()
{
	return mHeight;
}

bool init_sound_effects_and_music()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
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
		gWindow_sound_effects_and_music = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_sound_effects_and_music == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer_sound_effects_and_music = SDL_CreateRenderer( gWindow_sound_effects_and_music, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer_sound_effects_and_music == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer_sound_effects_and_music, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}

				 //Initialize SDL_mixer
				if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
				{
					printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia_sound_effects_and_music()
{
	//Loading success flag
	bool success = true;

	//Load prompt texture
	if( !gPromptTexture_sound_effects_and_music.loadFromFile( "prompt.png" ) )
	{
		printf( "Failed to load prompt texture!\n" );
		success = false;
	}

	//Load music
	gMusic_sound_effects_and_music = Mix_LoadMUS( "beat.wav" );
	if( gMusic_sound_effects_and_music == NULL )
	{
		printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	
	//Load sound effects
	gScratch_sound_effects_and_music = Mix_LoadWAV( "scratch.wav" );
	if( gScratch_sound_effects_and_music == NULL )
	{
		printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	
	gHigh_sound_effects_and_music = Mix_LoadWAV( "high.wav" );
	if( gHigh_sound_effects_and_music == NULL )
	{
		printf( "Failed to load high sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	gMedium_sound_effects_and_music = Mix_LoadWAV( "medium.wav" );
	if( gMedium_sound_effects_and_music == NULL )
	{
		printf( "Failed to load medium sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	gLow_sound_effects_and_music = Mix_LoadWAV( "low.wav" );
	if( gLow_sound_effects_and_music == NULL )
	{
		printf( "Failed to load low sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	return success;
}

void close_sound_effects_and_music()
{
	//Free loaded images
	gPromptTexture_sound_effects_and_music.free();

	//Free the sound effects
	Mix_FreeChunk( gScratch_sound_effects_and_music);
	Mix_FreeChunk( gHigh_sound_effects_and_music);
	Mix_FreeChunk( gMedium_sound_effects_and_music);
	Mix_FreeChunk( gLow_sound_effects_and_music);
	gScratch_sound_effects_and_music = NULL;
	gHigh_sound_effects_and_music = NULL;
	gMedium_sound_effects_and_music = NULL;
	gLow_sound_effects_and_music = NULL;
	
	//Free the music
	Mix_FreeMusic( gMusic_sound_effects_and_music);
	gMusic_sound_effects_and_music = NULL;

	//Destroy window	
	SDL_DestroyRenderer( gRenderer_sound_effects_and_music);
	SDL_DestroyWindow( gWindow_sound_effects_and_music);
	gWindow_sound_effects_and_music = NULL;
	gRenderer_sound_effects_and_music = NULL;

	//Quit SDL subsystems
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init_sound_effects_and_music() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia_sound_effects_and_music() )
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
					//Handle key press
					else if( e.type == SDL_KEYDOWN )
					{
						switch( e.key.keysym.sym )
						{
							//Play high sound effect
							case SDLK_1:
							Mix_PlayChannel( -1, gHigh_sound_effects_and_music, 0 );
							break;
							
							//Play medium sound effect
							case SDLK_2:
							Mix_PlayChannel( -1, gMedium_sound_effects_and_music, 0 );
							break;
							
							//Play low sound effect
							case SDLK_3:
							Mix_PlayChannel( -1, gLow_sound_effects_and_music, 0 );
							break;
							
							//Play scratch sound effect
							case SDLK_4:
							Mix_PlayChannel( -1, gScratch_sound_effects_and_music, 0 );
							break;
							
							case SDLK_9:
							//If there is no music playing
							if( Mix_PlayingMusic() == 0 )
							{
								//Play the music
								Mix_PlayMusic( gMusic_sound_effects_and_music, -1 );
							}
							//If music is being played
							else
							{
								//If the music is paused
								if( Mix_PausedMusic() == 1 )
								{
									//Resume the music
									Mix_ResumeMusic();
								}
								//If the music is playing
								else
								{
									//Pause the music
									Mix_PauseMusic();
								}
							}
							break;
							
							case SDLK_0:
							//Stop the music
							Mix_HaltMusic();
							break;
						}
					}
				}

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer_sound_effects_and_music, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer_sound_effects_and_music);

				//Render prompt
				gPromptTexture_sound_effects_and_music.render( 0, 0 );

				//Update screen
				SDL_RenderPresent( gRenderer_sound_effects_and_music);
			}
		}
	}

	//Free resources and close SDL
	close_sound_effects_and_music();

	return 0;
}