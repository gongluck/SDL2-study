/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, vectors, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <vector>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Texture wrapper class
class LTexture_per_pixel_collision_detection
{
	public:
		//Initializes variables
		LTexture_per_pixel_collision_detection();

		//Deallocates memory
		~LTexture_per_pixel_collision_detection();

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

//The dot that will move around on the screen
class Dot_per_pixel_collision_detection
{
    public:
		//The dimensions of the dot
		static const int DOT_WIDTH = 20;
		static const int DOT_HEIGHT = 20;

		//Maximum axis velocity of the dot
		static const int DOT_VEL = 1;

		//Initializes the variables
		Dot_per_pixel_collision_detection( int x, int y );

		//Takes key presses and adjusts the dot's velocity
		void handleEvent( SDL_Event& e );

		//Moves the dot and checks collision
		void move( std::vector<SDL_Rect>& otherColliders );

		//Shows the dot on the screen
		void render();

		//Gets the collision boxes
		std::vector<SDL_Rect>& getColliders();

    private:
		//The X and Y offsets of the dot
		int mPosX, mPosY;

		//The velocity of the dot
		int mVelX, mVelY;
		
		//Dot's collision boxes
	    std::vector<SDL_Rect> mColliders;

		//Moves the collision boxes relative to the dot's offset
		void shiftColliders();
};

//Starts up SDL and creates window
bool init_per_pixel_collision_detection();

//Loads media
bool loadMedia_per_pixel_collision_detection();

//Frees media and shuts down SDL
void close_per_pixel_collision_detection();

//Box set collision detector
bool checkCollision_per_pixel_collision_detection( std::vector<SDL_Rect>& a, std::vector<SDL_Rect>& b );

//The window we'll be rendering to
SDL_Window* gWindow_per_pixel_collision_detection = NULL;

//The window renderer
SDL_Renderer* gRenderer_per_pixel_collision_detection = NULL;

//Scene textures
LTexture_per_pixel_collision_detection gDotTexture_per_pixel_collision_detection;

LTexture_per_pixel_collision_detection::LTexture_per_pixel_collision_detection()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture_per_pixel_collision_detection::~LTexture_per_pixel_collision_detection()
{
	//Deallocate
	free();
}

bool LTexture_per_pixel_collision_detection::loadFromFile( std::string path )
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
        newTexture = SDL_CreateTextureFromSurface( gRenderer_per_pixel_collision_detection, loadedSurface );
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

void LTexture_per_pixel_collision_detection::free()
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

void LTexture_per_pixel_collision_detection::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture_per_pixel_collision_detection::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture_per_pixel_collision_detection::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture_per_pixel_collision_detection::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
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
	SDL_RenderCopyEx( gRenderer_per_pixel_collision_detection, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture_per_pixel_collision_detection::getWidth()
{
	return mWidth;
}

int LTexture_per_pixel_collision_detection::getHeight()
{
	return mHeight;
}

Dot_per_pixel_collision_detection::Dot_per_pixel_collision_detection( int x, int y )
{
    //Initialize the offsets
    mPosX = x;
    mPosY = y;

    //Create the necessary SDL_Rects
    mColliders.resize( 11 );

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;

    //Initialize the collision boxes' width and height
    mColliders[ 0 ].w = 6;
    mColliders[ 0 ].h = 1;

    mColliders[ 1 ].w = 10;
    mColliders[ 1 ].h = 1;

    mColliders[ 2 ].w = 14;
    mColliders[ 2 ].h = 1;

    mColliders[ 3 ].w = 16;
    mColliders[ 3 ].h = 2;

    mColliders[ 4 ].w = 18;
    mColliders[ 4 ].h = 2;

    mColliders[ 5 ].w = 20;
    mColliders[ 5 ].h = 6;

    mColliders[ 6 ].w = 18;
    mColliders[ 6 ].h = 2;

    mColliders[ 7 ].w = 16;
    mColliders[ 7 ].h = 2;

    mColliders[ 8 ].w = 14;
    mColliders[ 8 ].h = 1;

    mColliders[ 9 ].w = 10;
    mColliders[ 9 ].h = 1;

    mColliders[ 10 ].w = 6;
    mColliders[ 10 ].h = 1;

	//Initialize colliders relative to position
	shiftColliders();
}

void Dot_per_pixel_collision_detection::handleEvent( SDL_Event& e )
{
    //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY -= DOT_VEL; break;
            case SDLK_DOWN: mVelY += DOT_VEL; break;
            case SDLK_LEFT: mVelX -= DOT_VEL; break;
            case SDLK_RIGHT: mVelX += DOT_VEL; break;
        }
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY += DOT_VEL; break;
            case SDLK_DOWN: mVelY -= DOT_VEL; break;
            case SDLK_LEFT: mVelX += DOT_VEL; break;
            case SDLK_RIGHT: mVelX -= DOT_VEL; break;
        }
    }
}

void Dot_per_pixel_collision_detection::move( std::vector<SDL_Rect>& otherColliders )
{
    //Move the dot left or right
    mPosX += mVelX;
    shiftColliders();

    //If the dot collided or went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH > SCREEN_WIDTH ) || checkCollision_per_pixel_collision_detection( mColliders, otherColliders ) )
    {
        //Move back
        mPosX -= mVelX;
		shiftColliders();
    }

    //Move the dot up or down
    mPosY += mVelY;
	shiftColliders();

    //If the dot collided or went too far up or down
    if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > SCREEN_HEIGHT ) || checkCollision_per_pixel_collision_detection( mColliders, otherColliders ) )
    {
        //Move back
        mPosY -= mVelY;
		shiftColliders();
    }
}

void Dot_per_pixel_collision_detection::render()
{
    //Show the dot
	gDotTexture_per_pixel_collision_detection.render( mPosX, mPosY );
}

void Dot_per_pixel_collision_detection::shiftColliders()
{
    //The row offset
    int r = 0;

    //Go through the dot's collision boxes
    for( int set = 0; set < mColliders.size(); ++set )
    {
        //Center the collision box
        mColliders[ set ].x = mPosX + ( DOT_WIDTH - mColliders[ set ].w ) / 2;

        //Set the collision box at its row offset
        mColliders[ set ].y = mPosY + r;

        //Move the row offset down the height of the collision box
        r += mColliders[ set ].h;
    }
}

std::vector<SDL_Rect>& Dot_per_pixel_collision_detection::getColliders()
{
	return mColliders;
}

bool init_per_pixel_collision_detection()
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
		gWindow_per_pixel_collision_detection = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_per_pixel_collision_detection == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer_per_pixel_collision_detection = SDL_CreateRenderer( gWindow_per_pixel_collision_detection, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer_per_pixel_collision_detection == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer_per_pixel_collision_detection, 0xFF, 0xFF, 0xFF, 0xFF );

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

bool loadMedia_per_pixel_collision_detection()
{
	//Loading success flag
	bool success = true;

	//Load dot texture
	if( !gDotTexture_per_pixel_collision_detection.loadFromFile( "dot.bmp" ) )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}

	return success;
}

void close_per_pixel_collision_detection()
{
	//Free loaded images
	gDotTexture_per_pixel_collision_detection.free();

	//Destroy window	
	SDL_DestroyRenderer( gRenderer_per_pixel_collision_detection);
	SDL_DestroyWindow( gWindow_per_pixel_collision_detection);
	gWindow_per_pixel_collision_detection = NULL;
	gRenderer_per_pixel_collision_detection = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

bool checkCollision_per_pixel_collision_detection( std::vector<SDL_Rect>& a, std::vector<SDL_Rect>& b )
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Go through the A boxes
    for( int Abox = 0; Abox < a.size(); Abox++ )
    {
        //Calculate the sides of rect A
        leftA = a[ Abox ].x;
        rightA = a[ Abox ].x + a[ Abox ].w;
        topA = a[ Abox ].y;
        bottomA = a[ Abox ].y + a[ Abox ].h;

        //Go through the B boxes
        for( int Bbox = 0; Bbox < b.size(); Bbox++ )
        {
            //Calculate the sides of rect B
            leftB = b[ Bbox ].x;
            rightB = b[ Bbox ].x + b[ Bbox ].w;
            topB = b[ Bbox ].y;
            bottomB = b[ Bbox ].y + b[ Bbox ].h;

            //If no sides from A are outside of B
            if( ( ( bottomA <= topB ) || ( topA >= bottomB ) || ( rightA <= leftB ) || ( leftA >= rightB ) ) == false )
            {
                //A collision is detected
                return true;
            }
        }
    }

    //If neither set of collision boxes touched
    return false;
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init_per_pixel_collision_detection() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia_per_pixel_collision_detection() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//The dot that will be moving around on the screen
			Dot_per_pixel_collision_detection dot( 0, 0 );
			
			//The dot that will be collided against
			Dot_per_pixel_collision_detection otherDot( SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4 );
			
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

					//Handle input for the dot
					dot.handleEvent( e );
				}

				//Move the dot and check collision
				dot.move( otherDot.getColliders() );

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer_per_pixel_collision_detection, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer_per_pixel_collision_detection);
				
				//Render dots
				dot.render();
				otherDot.render();

				//Update screen
				SDL_RenderPresent( gRenderer_per_pixel_collision_detection);
			}
		}
	}

	//Free resources and close SDL
	close_per_pixel_collision_detection();

	return 0;
}