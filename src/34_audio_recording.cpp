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

//Maximum number of supported recording devices
const int MAX_RECORDING_DEVICES = 10;

//Maximum recording time
const int MAX_RECORDING_SECONDS = 5;

//Maximum recording time plus padding
const int RECORDING_BUFFER_SECONDS = MAX_RECORDING_SECONDS + 1;

//The various recording actions we can take
enum RecordingState_audio_recording
{
	SELECTING_DEVICE,
	STOPPED,
	RECORDING,
	RECORDED,
	PLAYBACK,
	ERROR
};

//Texture wrapper class
class LTexture_audio_recording
{
	public:
		//Initializes variables
		LTexture_audio_recording();

		//Deallocates memory
		~LTexture_audio_recording();

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
bool init_audio_recording();

//Loads media
bool loadMedia_audio_recording();

//Frees media and shuts down SDL
void close_audio_recording();

//Recording/playback callbacks
void audioRecordingCallback_audio_recording( void* userdata, Uint8* stream, int len );
void audioPlaybackCallback_audio_recording( void* userdata, Uint8* stream, int len );

//The window we'll be rendering to
SDL_Window* gWindow_audio_recording = NULL;

//The window renderer
SDL_Renderer* gRenderer_audio_recording = NULL;

//Globally used font
TTF_Font* gFont_audio_recording = NULL;
SDL_Color gTextColor_audio_recording = { 0, 0, 0, 0xFF };

//Prompt texture
LTexture_audio_recording gPromptTexture_audio_recording;

//The text textures that specify recording device names
LTexture_audio_recording gDeviceTextures_audio_recording[ MAX_RECORDING_DEVICES ];

//Number of available devices
int gRecordingDeviceCount_audio_recording = 0;

//Recieved audio spec
SDL_AudioSpec gReceivedRecordingSpec_audio_recording;
SDL_AudioSpec gReceivedPlaybackSpec_audio_recording;

//Recording data buffer
Uint8* gRecordingBuffer_audio_recording = NULL;

//Size of data buffer
Uint32 gBufferByteSize_audio_recording = 0;

//Position in data buffer
Uint32 gBufferBytePosition_audio_recording = 0;

//Maximum position in data buffer for recording
Uint32 gBufferByteMaxPosition_audio_recording = 0;

LTexture_audio_recording::LTexture_audio_recording()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture_audio_recording::~LTexture_audio_recording()
{
	//Deallocate
	free();
}

bool LTexture_audio_recording::loadFromFile( std::string path )
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
        newTexture = SDL_CreateTextureFromSurface( gRenderer_audio_recording, loadedSurface );
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
bool LTexture_audio_recording::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont_audio_recording, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer_audio_recording, textSurface );
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

void LTexture_audio_recording::free()
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

void LTexture_audio_recording::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture_audio_recording::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture_audio_recording::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture_audio_recording::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
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
	SDL_RenderCopyEx( gRenderer_audio_recording, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture_audio_recording::getWidth()
{
	return mWidth;
}

int LTexture_audio_recording::getHeight()
{
	return mHeight;
}

bool init_audio_recording()
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
		gWindow_audio_recording = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_audio_recording == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer_audio_recording = SDL_CreateRenderer( gWindow_audio_recording, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer_audio_recording == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer_audio_recording, 0xFF, 0xFF, 0xFF, 0xFF );

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
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia_audio_recording()
{
	//Loading success flag
	bool success = true;

	//Open the font
	gFont_audio_recording = TTF_OpenFont( "lazy.ttf", 28 );
	if( gFont_audio_recording == NULL )
	{
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}
	else
	{
		//Set starting prompt 
		gPromptTexture_audio_recording.loadFromRenderedText( "Select your recording device:", gTextColor_audio_recording);

		//Get capture device count
		gRecordingDeviceCount_audio_recording = SDL_GetNumAudioDevices( SDL_TRUE );

		//No recording devices
		if( gRecordingDeviceCount_audio_recording < 1 )
		{
			printf( "Unable to get audio capture device! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		//At least one device connected
		else
		{
			//Cap recording device count
			if( gRecordingDeviceCount_audio_recording > MAX_RECORDING_DEVICES )
			{
				gRecordingDeviceCount_audio_recording = MAX_RECORDING_DEVICES;
			}

			//Render device names
			std::stringstream promptText;
			for( int i = 0; i < gRecordingDeviceCount_audio_recording; ++i )
			{
				//Get capture device name
				promptText.str( "" );
				promptText << i << ": " << SDL_GetAudioDeviceName( i, SDL_TRUE );

				//Set texture from name
				gDeviceTextures_audio_recording[ i ].loadFromRenderedText( promptText.str().c_str(), gTextColor_audio_recording);
			}
		}
	}

	return success;
}

void close_audio_recording()
{
	//Free textures
	gPromptTexture_audio_recording.free();
	for( int i = 0; i < MAX_RECORDING_DEVICES; ++i )
	{
		gDeviceTextures_audio_recording[ i ].free();
	}

	//Free global font
	TTF_CloseFont( gFont_audio_recording);
	gFont_audio_recording = NULL;

	//Destroy window	
	SDL_DestroyRenderer( gRenderer_audio_recording);
	SDL_DestroyWindow( gWindow_audio_recording);
	gWindow_audio_recording = NULL;
	gRenderer_audio_recording = NULL;

	//Free playback audio
	if( gRecordingBuffer_audio_recording != NULL )
	{
		delete[] gRecordingBuffer_audio_recording;
		gRecordingBuffer_audio_recording = NULL;
	}

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void audioRecordingCallback_audio_recording( void* userdata, Uint8* stream, int len )
{
	//Copy audio from stream
	memcpy( &gRecordingBuffer_audio_recording[ gBufferBytePosition_audio_recording], stream, len );

	//Move along buffer
	gBufferBytePosition_audio_recording += len;
}

void audioPlaybackCallback_audio_recording( void* userdata, Uint8* stream, int len )
{
	//Copy audio to stream
	memcpy( stream, &gRecordingBuffer_audio_recording[ gBufferBytePosition_audio_recording], len );

	//Move along buffer
	gBufferBytePosition_audio_recording += len;
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init_audio_recording() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia_audio_recording() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//Set the default recording state
			RecordingState_audio_recording currentState = SELECTING_DEVICE;

			//Audio device IDs
			SDL_AudioDeviceID recordingDeviceId = 0;
			SDL_AudioDeviceID playbackDeviceId = 0;

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

					//Do current state event handling
					switch( currentState )
					{
						//User is selecting recording device
						case SELECTING_DEVICE:

							//On key press
							if( e.type == SDL_KEYDOWN )
							{
								//Handle key press from 0 to 9 
								if( e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9 )
								{
									//Get selection index
									int index = e.key.keysym.sym - SDLK_0;

									//Index is valid
									if( index < gRecordingDeviceCount_audio_recording)
									{
										//Default audio spec
										SDL_AudioSpec desiredRecordingSpec;
										SDL_zero(desiredRecordingSpec);
										desiredRecordingSpec.freq = 44100;
										desiredRecordingSpec.format = AUDIO_F32;
										desiredRecordingSpec.channels = 2;
										desiredRecordingSpec.samples = 4096;
										desiredRecordingSpec.callback = audioRecordingCallback_audio_recording;

										//Open recording device
										recordingDeviceId = SDL_OpenAudioDevice( SDL_GetAudioDeviceName( index, SDL_TRUE ), SDL_TRUE, &desiredRecordingSpec, &gReceivedRecordingSpec_audio_recording, SDL_AUDIO_ALLOW_FORMAT_CHANGE );
										
										//Device failed to open
										if( recordingDeviceId == 0 )
										{
											//Report error
											printf( "Failed to open recording device! SDL Error: %s", SDL_GetError() );
											gPromptTexture_audio_recording.loadFromRenderedText( "Failed to open recording device!", gTextColor_audio_recording);
											currentState = ERROR;
										}
										//Device opened successfully
										else
										{
											//Default audio spec
											SDL_AudioSpec desiredPlaybackSpec;
											SDL_zero(desiredPlaybackSpec);
											desiredPlaybackSpec.freq = 44100;
											desiredPlaybackSpec.format = AUDIO_F32;
											desiredPlaybackSpec.channels = 2;
											desiredPlaybackSpec.samples = 4096;
											desiredPlaybackSpec.callback = audioPlaybackCallback_audio_recording;

											//Open playback device
											playbackDeviceId = SDL_OpenAudioDevice( NULL, SDL_FALSE, &desiredPlaybackSpec, &gReceivedPlaybackSpec_audio_recording, SDL_AUDIO_ALLOW_FORMAT_CHANGE );

											//Device failed to open
											if( playbackDeviceId == 0 )
											{
												//Report error
												printf( "Failed to open playback device! SDL Error: %s", SDL_GetError() );
												gPromptTexture_audio_recording.loadFromRenderedText( "Failed to open playback device!", gTextColor_audio_recording);
												currentState = ERROR;
											}
											//Device opened successfully
											else
											{
												//Calculate per sample bytes
												int bytesPerSample = gReceivedRecordingSpec_audio_recording.channels * ( SDL_AUDIO_BITSIZE( gReceivedRecordingSpec_audio_recording.format ) / 8 );

												//Calculate bytes per second
												int bytesPerSecond = gReceivedRecordingSpec_audio_recording.freq * bytesPerSample;

												//Calculate buffer size
												gBufferByteSize_audio_recording = RECORDING_BUFFER_SECONDS * bytesPerSecond;

												//Calculate max buffer use
												gBufferByteMaxPosition_audio_recording = MAX_RECORDING_SECONDS * bytesPerSecond;

												//Allocate and initialize byte buffer
												gRecordingBuffer_audio_recording = new Uint8[ gBufferByteSize_audio_recording];
												memset( gRecordingBuffer_audio_recording, 0, gBufferByteSize_audio_recording);

												//Go on to next state
												gPromptTexture_audio_recording.loadFromRenderedText("Press 1 to record for 5 seconds.", gTextColor_audio_recording);
												currentState = STOPPED;
											}
										}
									}
								}
							}
							break;	

						//User getting ready to record
						case STOPPED:

							//On key press
							if( e.type == SDL_KEYDOWN )
							{
								//Start recording
								if( e.key.keysym.sym == SDLK_1 )
								{
									//Go back to beginning of buffer
									gBufferBytePosition_audio_recording = 0;

									//Start recording
									SDL_PauseAudioDevice( recordingDeviceId, SDL_FALSE );

									//Go on to next state
									gPromptTexture_audio_recording.loadFromRenderedText( "Recording...", gTextColor_audio_recording);
									currentState = RECORDING;
								}
							}
							break;	

						//User has finished recording
						case RECORDED:

							//On key press
							if( e.type == SDL_KEYDOWN )
							{
								//Start playback
								if( e.key.keysym.sym == SDLK_1 )
								{
									//Go back to beginning of buffer
									gBufferBytePosition_audio_recording = 0;

									//Start playback
									SDL_PauseAudioDevice( playbackDeviceId, SDL_FALSE );

									//Go on to next state
									gPromptTexture_audio_recording.loadFromRenderedText( "Playing...", gTextColor_audio_recording);
									currentState = PLAYBACK;
								}
								//Record again
								if( e.key.keysym.sym == SDLK_2 )
								{
									//Reset the buffer
									gBufferBytePosition_audio_recording = 0;
									memset( gRecordingBuffer_audio_recording, 0, gBufferByteSize_audio_recording);

									//Start recording
									SDL_PauseAudioDevice( recordingDeviceId, SDL_FALSE );

									//Go on to next state
									gPromptTexture_audio_recording.loadFromRenderedText( "Recording...", gTextColor_audio_recording);
									currentState = RECORDING;
								}
							}
							break;
					}
				}

				//Updating recording
				if( currentState == RECORDING )
				{
					//Lock callback
					SDL_LockAudioDevice( recordingDeviceId );

					//Finished recording
					if( gBufferBytePosition_audio_recording > gBufferByteMaxPosition_audio_recording)
					{
						//Stop recording audio
						SDL_PauseAudioDevice( recordingDeviceId, SDL_TRUE );

						//Go on to next state
						gPromptTexture_audio_recording.loadFromRenderedText( "Press 1 to play back. Press 2 to record again.", gTextColor_audio_recording);
						currentState = RECORDED;
					}

					//Unlock callback
					SDL_UnlockAudioDevice( recordingDeviceId );
				}
				//Updating playback
				else if( currentState == PLAYBACK )
				{
					//Lock callback
					SDL_LockAudioDevice( playbackDeviceId );

					//Finished playback
					if( gBufferBytePosition_audio_recording > gBufferByteMaxPosition_audio_recording)
					{
						//Stop playing audio
						SDL_PauseAudioDevice( playbackDeviceId, SDL_TRUE );

						//Go on to next state
						gPromptTexture_audio_recording.loadFromRenderedText( "Press 1 to play back. Press 2 to record again.", gTextColor_audio_recording);
						currentState = RECORDED;
					}

					//Unlock callback
					SDL_UnlockAudioDevice( playbackDeviceId );
				}

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer_audio_recording, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer_audio_recording);

				//Render prompt centered at the top of the screen
				gPromptTexture_audio_recording.render( ( SCREEN_WIDTH - gPromptTexture_audio_recording.getWidth() ) / 2, 0 );

				//User is selecting 
				if( currentState == SELECTING_DEVICE )
				{
					//Render device names
					int yOffset = gPromptTexture_audio_recording.getHeight() * 2;
					for( int i = 0; i < gRecordingDeviceCount_audio_recording; ++i )
					{
						gDeviceTextures_audio_recording[ i ].render( 0, yOffset );
						yOffset += gDeviceTextures_audio_recording[ i ].getHeight() + 1;
					}
				}

				//Update screen
				SDL_RenderPresent( gRenderer_audio_recording);
			}
		}
	}

	//Free resources and close SDL
	close_audio_recording();

	return 0;
}