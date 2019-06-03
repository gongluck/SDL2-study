/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, vectors, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//A circle stucture
struct Circle_circular_collision_detection
{
	int x, y;
	int r;
};

//Texture wrapper class
class LTexture_circular_collision_detection
{
	public:
		//Initializes variables
		LTexture_circular_collision_detection();

		//Deallocates memory
		~LTexture_circular_collision_detection();

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
class Dot_circular_collision_detection
{
    public:
		//The dimensions of the dot
		static const int DOT_WIDTH = 20;
		static const int DOT_HEIGHT = 20;

		//Maximum axis velocity of the dot
		static const int DOT_VEL = 1;

		//Initializes the variables
		Dot_circular_collision_detection( int x, int y );

		//Takes key presses and adjusts the dot's velocity
		void handleEvent( SDL_Event& e );

		//Moves the dot and checks collision
		void move( SDL_Rect& square, Circle_circular_collision_detection& circle );

		//Shows the dot on the screen
		void render();

		//Gets collision circle
		Circle_circular_collision_detection& getCollider();

    private:
		//The X and Y offsets of the dot
		int mPosX, mPosY;

		//The velocity of the dot
		int mVelX, mVelY;
		
		//Dot's collision circle
		Circle_circular_collision_detection mCollider;

		//Moves the collision circle relative to the dot's offset
		void shiftColliders();
};

//Starts up SDL and creates window
bool init_circular_collision_detection();

//Loads media
bool loadMedia_circular_collision_detection();

//Frees media and shuts down SDL
void close_circular_collision_detection();

//Circle/Circle collision detector
bool checkCollision_circular_collision_detection( Circle_circular_collision_detection& a, Circle_circular_collision_detection& b );

//Circle/Box collision detector
bool checkCollision_circular_collision_detection( Circle_circular_collision_detection& a, SDL_Rect& b );

//Calculates distance squared between two points
double distanceSquared_circular_collision_detection( int x1, int y1, int x2, int y2 );

//The window we'll be rendering to
SDL_Window* gWindow_circular_collision_detection = NULL;

//The window renderer
SDL_Renderer* gRenderer_circular_collision_detection = NULL;

//Scene textures
LTexture_circular_collision_detection gDotTexture_circular_collision_detection;

LTexture_circular_collision_detection::LTexture_circular_collision_detection()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture_circular_collision_detection::~LTexture_circular_collision_detection()
{
	//Deallocate
	free();
}

bool LTexture_circular_collision_detection::loadFromFile( std::string path )
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
        newTexture = SDL_CreateTextureFromSurface( gRenderer_circular_collision_detection, loadedSurface );
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

void LTexture_circular_collision_detection::free()
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

void LTexture_circular_collision_detection::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture_circular_collision_detection::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture_circular_collision_detection::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture_circular_collision_detection::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
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
	SDL_RenderCopyEx( gRenderer_circular_collision_detection, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture_circular_collision_detection::getWidth()
{
	return mWidth;
}

int LTexture_circular_collision_detection::getHeight()
{
	return mHeight;
}

Dot_circular_collision_detection::Dot_circular_collision_detection( int x, int y )
{
    //Initialize the offsets
    mPosX = x;
    mPosY = y;

	//Set collision circle size
	mCollider.r = DOT_WIDTH / 2;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;

	//Move collider relative to the circle
	shiftColliders();
}

void Dot_circular_collision_detection::handleEvent( SDL_Event& e )
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

void Dot_circular_collision_detection::move( SDL_Rect& square, Circle_circular_collision_detection& circle )
{
    //Move the dot left or right
    mPosX += mVelX;
	shiftColliders();

    //If the dot collided or went too far to the left or right
	if( ( mPosX - mCollider.r < 0 ) || ( mPosX + mCollider.r > SCREEN_WIDTH ) || checkCollision_circular_collision_detection( mCollider, square ) || checkCollision_circular_collision_detection( mCollider, circle ) )
    {
        //Move back
        mPosX -= mVelX;
		shiftColliders();
    }

    //Move the dot up or down
    mPosY += mVelY;
	shiftColliders();

    //If the dot collided or went too far up or down
    if( ( mPosY - mCollider.r < 0 ) || ( mPosY + mCollider.r > SCREEN_HEIGHT ) || checkCollision_circular_collision_detection( mCollider, square ) || checkCollision_circular_collision_detection( mCollider, circle ) )
    {
        //Move back
        mPosY -= mVelY;
		shiftColliders();
    }
}

void Dot_circular_collision_detection::render()
{
    //Show the dot
	gDotTexture_circular_collision_detection.render( mPosX - mCollider.r, mPosY - mCollider.r );
}

Circle_circular_collision_detection& Dot_circular_collision_detection::getCollider()
{
	return mCollider;
}

void Dot_circular_collision_detection::shiftColliders()
{
	//Align collider to center of dot
	mCollider.x = mPosX;
	mCollider.y = mPosY;
}

bool init_circular_collision_detection()
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
		gWindow_circular_collision_detection = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_circular_collision_detection == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer_circular_collision_detection = SDL_CreateRenderer( gWindow_circular_collision_detection, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer_circular_collision_detection == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer_circular_collision_detection, 0xFF, 0xFF, 0xFF, 0xFF );

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

bool loadMedia_circular_collision_detection()
{
	//Loading success flag
	bool success = true;

	//Load dot texture
	if( !gDotTexture_circular_collision_detection.loadFromFile( "dot.bmp" ) )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}

	return success;
}

void close_circular_collision_detection()
{
	//Free loaded images
	gDotTexture_circular_collision_detection.free();

	//Destroy window	
	SDL_DestroyRenderer( gRenderer_circular_collision_detection);
	SDL_DestroyWindow( gWindow_circular_collision_detection);
	gWindow_circular_collision_detection = NULL;
	gRenderer_circular_collision_detection = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

bool checkCollision_circular_collision_detection( Circle_circular_collision_detection& a, Circle_circular_collision_detection& b )
{
	//Calculate total radius squared
	int totalRadiusSquared = a.r + b.r;
	totalRadiusSquared = totalRadiusSquared * totalRadiusSquared;

    //If the distance between the centers of the circles is less than the sum of their radii
    if( distanceSquared_circular_collision_detection( a.x, a.y, b.x, b.y ) < ( totalRadiusSquared ) )
    {
        //The circles have collided
        return true;
    }

    //If not
    return false;
}

bool checkCollision_circular_collision_detection( Circle_circular_collision_detection& a, SDL_Rect& b )
{
    //Closest point on collision box
    int cX, cY;

    //Find closest x offset
    if( a.x < b.x )
    {
        cX = b.x;
    }
    else if( a.x > b.x + b.w )
    {
        cX = b.x + b.w;
    }
    else
    {
        cX = a.x;
    }

    //Find closest y offset
    if( a.y < b.y )
    {
        cY = b.y;
    }
    else if( a.y > b.y + b.h )
    {
        cY = b.y + b.h;
    }
    else
    {
        cY = a.y;
    }

    //If the closest point is inside the circle
    if( distanceSquared_circular_collision_detection( a.x, a.y, cX, cY ) < a.r * a.r )
    {
        //This box and the circle have collided
        return true;
    }

    //If the shapes have not collided
    return false;
}

double distanceSquared_circular_collision_detection( int x1, int y1, int x2, int y2 )
{
	int deltaX = x2 - x1;
	int deltaY = y2 - y1;
	return deltaX*deltaX + deltaY*deltaY;
}

int main_circular_collision_detection( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init_circular_collision_detection() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia_circular_collision_detection() )
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
			Dot_circular_collision_detection dot( Dot_circular_collision_detection::DOT_WIDTH / 2, Dot_circular_collision_detection::DOT_HEIGHT / 2 );
			Dot_circular_collision_detection otherDot( SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4 );

			//Set the wall
			SDL_Rect wall;
			wall.x = 300;
			wall.y = 40;
			wall.w = 40;
			wall.h = 400;
			
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
				dot.move( wall, otherDot.getCollider() );

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer_circular_collision_detection, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer_circular_collision_detection);

				//Render wall
				SDL_SetRenderDrawColor( gRenderer_circular_collision_detection, 0x00, 0x00, 0x00, 0xFF );
				SDL_RenderDrawRect( gRenderer_circular_collision_detection, &wall );
				
				//Render dots
				dot.render();
				otherDot.render();

				//Update screen
				SDL_RenderPresent( gRenderer_circular_collision_detection);
			}
		}
	}

	//Free resources and close SDL
	close_circular_collision_detection();

	return 0;
}