/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, strings, and file streams
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <fstream>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//The dimensions of the level
const int LEVEL_WIDTH = 1280;
const int LEVEL_HEIGHT = 960;

//Tile constants
const int TILE_WIDTH = 80;
const int TILE_HEIGHT = 80;
const int TOTAL_TILES = 192;
const int TOTAL_TILE_SPRITES = 12;

//The different tile sprites
const int TILE_RED = 0;
const int TILE_GREEN = 1;
const int TILE_BLUE = 2;
const int TILE_CENTER = 3;
const int TILE_TOP = 4;
const int TILE_TOPRIGHT = 5;
const int TILE_RIGHT = 6;
const int TILE_BOTTOMRIGHT = 7;
const int TILE_BOTTOM = 8;
const int TILE_BOTTOMLEFT = 9;
const int TILE_LEFT = 10;
const int TILE_TOPLEFT = 11;

//Texture wrapper class
class LTexture_tiling
{
	public:
		//Initializes variables
		LTexture_tiling();

		//Deallocates memory
		~LTexture_tiling();

		//Loads image at specified path
		bool loadFromFile( std::string path );
		
		#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
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

//The tile
class Tile_tiling
{
    public:
		//Initializes position and type
		Tile_tiling( int x, int y, int tileType );

		//Shows the tile
		void render( SDL_Rect& camera );

		//Get the tile type
		int getType();

		//Get the collision box
		SDL_Rect getBox();

    private:
		//The attributes of the tile
		SDL_Rect mBox;

		//The tile type
		int mType;
};

//The dot that will move around on the screen
class Dot_tiling
{
    public:
		//The dimensions of the dot
		static const int DOT_WIDTH = 20;
		static const int DOT_HEIGHT = 20;

		//Maximum axis velocity of the dot
		static const int DOT_VEL = 10;

		//Initializes the variables
		Dot_tiling();

		//Takes key presses and adjusts the dot's velocity
		void handleEvent( SDL_Event& e );

		//Moves the dot and check collision against tiles
		void move( Tile_tiling*tiles[] );

		//Centers the camera over the dot
		void setCamera( SDL_Rect& camera );

		//Shows the dot on the screen
		void render( SDL_Rect& camera );

    private:
		//Collision box of the dot
		SDL_Rect mBox;

		//The velocity of the dot
		int mVelX, mVelY;
};

//Starts up SDL and creates window
bool init_tiling();

//Loads media
bool loadMedia_tiling( Tile_tiling* tiles[] );

//Frees media and shuts down SDL
void close_tiling( Tile_tiling* tiles[] );

//Box collision detector
bool checkCollision_tiling( SDL_Rect a, SDL_Rect b );

//Checks collision box against set of tiles
bool touchesWall_tiling( SDL_Rect box, Tile_tiling* tiles[] );

//Sets tiles from tile map
bool setTiles_tiling( Tile_tiling*tiles[] );

//The window we'll be rendering to
SDL_Window* gWindow_tiling = NULL;

//The window renderer
SDL_Renderer* gRenderer_tiling = NULL;

//Scene textures
LTexture_tiling gDotTexture_tiling;
LTexture_tiling gTileTexture_tiling;
SDL_Rect gTileClips_tiling[ TOTAL_TILE_SPRITES ];

LTexture_tiling::LTexture_tiling()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture_tiling::~LTexture_tiling()
{
	//Deallocate
	free();
}

bool LTexture_tiling::loadFromFile( std::string path )
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
        newTexture = SDL_CreateTextureFromSurface( gRenderer_tiling, loadedSurface );
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

void LTexture_tiling::free()
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

void LTexture_tiling::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture_tiling::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture_tiling::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture_tiling::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
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
	SDL_RenderCopyEx( gRenderer_tiling, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture_tiling::getWidth()
{
	return mWidth;
}

int LTexture_tiling::getHeight()
{
	return mHeight;
}

Tile_tiling::Tile_tiling( int x, int y, int tileType )
{
    //Get the offsets
    mBox.x = x;
    mBox.y = y;

    //Set the collision box
    mBox.w = TILE_WIDTH;
    mBox.h = TILE_HEIGHT;

    //Get the tile type
    mType = tileType;
}

void Tile_tiling::render( SDL_Rect& camera )
{
    //If the tile is on screen
    if( checkCollision_tiling( camera, mBox ) )
    {
        //Show the tile
        gTileTexture_tiling.render( mBox.x - camera.x, mBox.y - camera.y, &gTileClips_tiling[ mType ] );
    }
}

int Tile_tiling::getType()
{
    return mType;
}

SDL_Rect Tile_tiling::getBox()
{
    return mBox;
}

Dot_tiling::Dot_tiling()
{
    //Initialize the collision box
    mBox.x = 0;
    mBox.y = 0;
	mBox.w = DOT_WIDTH;
	mBox.h = DOT_HEIGHT;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;
}

void Dot_tiling::handleEvent( SDL_Event& e )
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

void Dot_tiling::move( Tile_tiling*tiles[] )
{
    //Move the dot left or right
    mBox.x += mVelX;

    //If the dot went too far to the left or right or touched a wall
    if( ( mBox.x < 0 ) || ( mBox.x + DOT_WIDTH > LEVEL_WIDTH ) || touchesWall_tiling( mBox, tiles ) )
    {
        //move back
        mBox.x -= mVelX;
    }

    //Move the dot up or down
    mBox.y += mVelY;

    //If the dot went too far up or down or touched a wall
    if( ( mBox.y < 0 ) || ( mBox.y + DOT_HEIGHT > LEVEL_HEIGHT ) || touchesWall_tiling( mBox, tiles ) )
    {
        //move back
        mBox.y -= mVelY;
    }
}

void Dot_tiling::setCamera( SDL_Rect& camera )
{
	//Center the camera over the dot
	camera.x = ( mBox.x + DOT_WIDTH / 2 ) - SCREEN_WIDTH / 2;
	camera.y = ( mBox.y + DOT_HEIGHT / 2 ) - SCREEN_HEIGHT / 2;

	//Keep the camera in bounds
	if( camera.x < 0 )
	{ 
		camera.x = 0;
	}
	if( camera.y < 0 )
	{
		camera.y = 0;
	}
	if( camera.x > LEVEL_WIDTH - camera.w )
	{
		camera.x = LEVEL_WIDTH - camera.w;
	}
	if( camera.y > LEVEL_HEIGHT - camera.h )
	{
		camera.y = LEVEL_HEIGHT - camera.h;
	}
}

void Dot_tiling::render( SDL_Rect& camera )
{
    //Show the dot
	gDotTexture_tiling.render( mBox.x - camera.x, mBox.y - camera.y );
}

bool init_tiling()
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
		gWindow_tiling = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow_tiling == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer_tiling = SDL_CreateRenderer( gWindow_tiling, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer_tiling == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer_tiling, 0xFF, 0xFF, 0xFF, 0xFF );

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

bool loadMedia_tiling( Tile_tiling* tiles[] )
{
	//Loading success flag
	bool success = true;

	//Load dot texture
	if( !gDotTexture_tiling.loadFromFile( "dot.bmp" ) )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}

	//Load tile texture
	if( !gTileTexture_tiling.loadFromFile( "tiles.png" ) )
	{
		printf( "Failed to load tile set texture!\n" );
		success = false;
	}

	//Load tile map
	if( !setTiles_tiling( tiles ) )
	{
		printf( "Failed to load tile set!\n" );
		success = false;
	}

	return success;
}

void close_tiling( Tile_tiling* tiles[] )
{
	//Deallocate tiles
	for( int i = 0; i < TOTAL_TILES; ++i )
	{
		 if( tiles[ i ] == NULL )
		 {
			delete tiles[ i ];
			tiles[ i ] = NULL;
		 }
	}

	//Free loaded images
	gDotTexture_tiling.free();
	gTileTexture_tiling.free();

	//Destroy window	
	SDL_DestroyRenderer( gRenderer_tiling);
	SDL_DestroyWindow( gWindow_tiling);
	gWindow_tiling = NULL;
	gRenderer_tiling = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

bool checkCollision_tiling( SDL_Rect a, SDL_Rect b )
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    //Calculate the sides of rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    //If any of the sides from A are outside of B
    if( bottomA <= topB )
    {
        return false;
    }

    if( topA >= bottomB )
    {
        return false;
    }

    if( rightA <= leftB )
    {
        return false;
    }

    if( leftA >= rightB )
    {
        return false;
    }

    //If none of the sides from A are outside B
    return true;
}

bool setTiles_tiling( Tile_tiling* tiles[] )
{
	//Success flag
	bool tilesLoaded = true;

    //The tile offsets
    int x = 0, y = 0;

    //Open the map
    std::ifstream map( "lazy.map" );

    //If the map couldn't be loaded
    if( !map.is_open())
    {
		printf( "Unable to load map file!\n" );
		tilesLoaded = false;
    }
	else
	{
		//Initialize the tiles
		for( int i = 0; i < TOTAL_TILES; ++i )
		{
			//Determines what kind of tile will be made
			int tileType = -1;

			//Read tile from map file
			map >> tileType;

			//If the was a problem in reading the map
			if( map.fail() )
			{
				//Stop loading map
				printf( "Error loading map: Unexpected end of file!\n" );
				tilesLoaded = false;
				break;
			}

			//If the number is a valid tile number
			if( ( tileType >= 0 ) && ( tileType < TOTAL_TILE_SPRITES ) )
			{
				tiles[ i ] = new Tile_tiling( x, y, tileType );
			}
			//If we don't recognize the tile type
			else
			{
				//Stop loading map
				printf( "Error loading map: Invalid tile type at %d!\n", i );
				tilesLoaded = false;
				break;
			}

			//Move to next tile spot
			x += TILE_WIDTH;

			//If we've gone too far
			if( x >= LEVEL_WIDTH )
			{
				//Move back
				x = 0;

				//Move to the next row
				y += TILE_HEIGHT;
			}
		}
		
		//Clip the sprite sheet
		if( tilesLoaded )
		{
			gTileClips_tiling[ TILE_RED ].x = 0;
			gTileClips_tiling[ TILE_RED ].y = 0;
			gTileClips_tiling[ TILE_RED ].w = TILE_WIDTH;
			gTileClips_tiling[ TILE_RED ].h = TILE_HEIGHT;

			gTileClips_tiling[ TILE_GREEN ].x = 0;
			gTileClips_tiling[ TILE_GREEN ].y = 80;
			gTileClips_tiling[ TILE_GREEN ].w = TILE_WIDTH;
			gTileClips_tiling[ TILE_GREEN ].h = TILE_HEIGHT;

			gTileClips_tiling[ TILE_BLUE ].x = 0;
			gTileClips_tiling[ TILE_BLUE ].y = 160;
			gTileClips_tiling[ TILE_BLUE ].w = TILE_WIDTH;
			gTileClips_tiling[ TILE_BLUE ].h = TILE_HEIGHT;

			gTileClips_tiling[ TILE_TOPLEFT ].x = 80;
			gTileClips_tiling[ TILE_TOPLEFT ].y = 0;
			gTileClips_tiling[ TILE_TOPLEFT ].w = TILE_WIDTH;
			gTileClips_tiling[ TILE_TOPLEFT ].h = TILE_HEIGHT;

			gTileClips_tiling[ TILE_LEFT ].x = 80;
			gTileClips_tiling[ TILE_LEFT ].y = 80;
			gTileClips_tiling[ TILE_LEFT ].w = TILE_WIDTH;
			gTileClips_tiling[ TILE_LEFT ].h = TILE_HEIGHT;

			gTileClips_tiling[ TILE_BOTTOMLEFT ].x = 80;
			gTileClips_tiling[ TILE_BOTTOMLEFT ].y = 160;
			gTileClips_tiling[ TILE_BOTTOMLEFT ].w = TILE_WIDTH;
			gTileClips_tiling[ TILE_BOTTOMLEFT ].h = TILE_HEIGHT;

			gTileClips_tiling[ TILE_TOP ].x = 160;
			gTileClips_tiling[ TILE_TOP ].y = 0;
			gTileClips_tiling[ TILE_TOP ].w = TILE_WIDTH;
			gTileClips_tiling[ TILE_TOP ].h = TILE_HEIGHT;

			gTileClips_tiling[ TILE_CENTER ].x = 160;
			gTileClips_tiling[ TILE_CENTER ].y = 80;
			gTileClips_tiling[ TILE_CENTER ].w = TILE_WIDTH;
			gTileClips_tiling[ TILE_CENTER ].h = TILE_HEIGHT;

			gTileClips_tiling[ TILE_BOTTOM ].x = 160;
			gTileClips_tiling[ TILE_BOTTOM ].y = 160;
			gTileClips_tiling[ TILE_BOTTOM ].w = TILE_WIDTH;
			gTileClips_tiling[ TILE_BOTTOM ].h = TILE_HEIGHT;

			gTileClips_tiling[ TILE_TOPRIGHT ].x = 240;
			gTileClips_tiling[ TILE_TOPRIGHT ].y = 0;
			gTileClips_tiling[ TILE_TOPRIGHT ].w = TILE_WIDTH;
			gTileClips_tiling[ TILE_TOPRIGHT ].h = TILE_HEIGHT;

			gTileClips_tiling[ TILE_RIGHT ].x = 240;
			gTileClips_tiling[ TILE_RIGHT ].y = 80;
			gTileClips_tiling[ TILE_RIGHT ].w = TILE_WIDTH;
			gTileClips_tiling[ TILE_RIGHT ].h = TILE_HEIGHT;

			gTileClips_tiling[ TILE_BOTTOMRIGHT ].x = 240;
			gTileClips_tiling[ TILE_BOTTOMRIGHT ].y = 160;
			gTileClips_tiling[ TILE_BOTTOMRIGHT ].w = TILE_WIDTH;
			gTileClips_tiling[ TILE_BOTTOMRIGHT ].h = TILE_HEIGHT;
		}
	}

    //Close the file
    map.close();

    //If the map was loaded fine
    return tilesLoaded;
}

bool touchesWall_tiling( SDL_Rect box, Tile_tiling* tiles[] )
{
    //Go through the tiles
    for( int i = 0; i < TOTAL_TILES; ++i )
    {
        //If the tile is a wall type tile
        if( ( tiles[ i ]->getType() >= TILE_CENTER ) && ( tiles[ i ]->getType() <= TILE_TOPLEFT ) )
        {
            //If the collision box touches the wall tile
            if( checkCollision_tiling( box, tiles[ i ]->getBox() ) )
            {
                return true;
            }
        }
    }

    //If no wall tiles were touched
    return false;
}

int main_tiling( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init_tiling() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//The level tiles
		Tile_tiling* tileSet[ TOTAL_TILES ];

		//Load media
		if( !loadMedia_tiling( tileSet ) )
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
			Dot_tiling dot;

			//Level camera
			SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

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

				//Move the dot
				dot.move( tileSet );
				dot.setCamera( camera );

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer_tiling, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer_tiling);

				//Render level
				for( int i = 0; i < TOTAL_TILES; ++i )
				{
					tileSet[ i ]->render( camera );
				}

				//Render dot
				dot.render( camera );

				//Update screen
				SDL_RenderPresent( gRenderer_tiling);
			}
		}
		
		//Free resources and close SDL
		close_tiling( tileSet );
	}

	return 0;
}