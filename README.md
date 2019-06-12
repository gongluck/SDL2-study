# SDL2-study
##SDL2的学习笔记,主要参考http://lazyfoo.net/tutorials/SDL/index.php
Lesson 01
Hello SDL
	In this tutorial we will be setting up the SDL library and creating our first window.
Lesson 02
Getting an Image on the Screen
	Now that we can get a window to appear, lets blit an image onto it.
Lesson 03
Event Driven Programming
	Here we'll start handling user input by allow the user to X out the window.
Lesson 04
Key Presses
	Here we'll learn to handle keyboard input.
Lesson 05
Optimized Surface Loading and Soft Stretching
	Now that we know how to load and blit surfaces, it's time to make our blits faster. We'll also take a smaller image and stretch it to fit the screen.
Lesson 06
Extension Libraries and Loading Other Image Formats
	Here we'll be using the SDL_image extension library to load png images.
Lesson 07
Texture Loading and Rendering
	A big new feature in SDL 2.0 is hardware accelerated texture based 2D rendering. Here we'll be loading an image to render it using textures.
Lesson 08
Geometry Rendering
	Another new feature in SDL 2.0 is hardware accelerated primitive rendering. Here we'll be using it to render some common shapes.
Lesson 09
The Viewport
	SDL 2.0 also lets you control where you render on the screen using the viewport. We'll be using the viewport to create subscreens.
Lesson 10
Color Keying
	Here we'll use color keying to give texture transparent backgrounds.
Lesson 11
Clip Rendering and Sprite Sheets
	Using clip rendering, you can keep multiple images on one texture and render the part you need. We'll be using this to render individual sprites from a sprite sheet.
Lesson 12
Color Modulation
	We'll be altering the color of rendered textures using color modulation.
Lesson 13
Alpha Blending
	Here we'll be using SDL 2.0 new hardware accelerated alpha blending.
Lesson 14
Animated Sprites and Vsync
	Here we'll be using a sequence of sprites to animate them.
Lesson 15
Rotation and Flipping
	Here we'll be using SDL 2.0's new texture rotation and flipping.
Lesson 16
True Type Fonts
	Here we'll be rendering text from true type fonts using SDL_ttf.
Lesson 17
Mouse Events
	Here we'll learn to read mouse input using mouse events.
Lesson 18
Key States
	There's other ways to read the keys besides event polling. Here will get the current states of the keyboard using get states.
Lesson 19
Gamepads and Joysticks
	Here we'll learn to read input from a game controller.
Lesson 20
Force Feedback
	Another new feature for SDL 2.0 is rumble support using the SDL haptics. We'll make our controller rumble when a button is pressed.
Lesson 21
Sound Effects and Music
	Here we'll be using SDL_mixer to add music and sound to our SDL App.
Lesson 22
Timing
	Here we'll be using SDL's time capabilites.
Lesson 23
Advanced Timers
	Here we'll extend SDL time capabilities to make our own custom timer.
Lesson 24
Calculating Frame Rate
	Here we'll use the timers we built to measure frame rate.
Lesson 25
Capping Frame Rate
	If you need a constant frame rate when vsync isn't available, frame rate capping can be used as a fall back.
Lesson 26
Motion
	Here we'll be taking what we learned about render and handling input to make a dot move around the screen.
Lesson 27
Collision Detection
	Here we'll have two objects interact with each other using bounding box collision detection.
Lesson 28
Per-pixel Collision Detection
	Here we'll have two object collide using per-pixel collision detection.
Lesson 29
Circular Collision Detection
	Here we'll learn to detect collisions with circles and boxes.
Lesson 30
Scrolling
	Here we'll be implement a camera to scroll levels larger than the screen.
Lesson 31
Scrolling Backgrounds
	Here we'll using a scrolling background to give the illusion of an infinite level.
Lesson 32
Text Input and Clipboard Handling
	Here we'll using SDL 2.0's new way of handling text input and its new clip board handling feature.
Lesson 33
File Reading and Writing
	Here we'll using SDL's RWOps API to do binary file IO.
Lesson 34
Audio Recording
	SDL 2.0.8 supports audio recording. Here we'll be copying from the audio device to record and copying to the audio device to play back.
Lesson 35
Window Events
	Here we'll be handling events from a resizable window.
Lesson 36
Multiple Windows
	A new feature in SDL is the ability to support more than one window. Here we'll make an application that has 3 windows.
Lesson 37
Multiple Displays
	Another new feature of SDL 2.0 is the ability to handle more than one physical display. Here we'll make our window jump from display to display.
Lesson 38
Particle Engines
	Here we'll use a simple particle effect to create a simple trail effect.
Lesson 39
Tiling
	Here we'll make a simple level using a tiling engine.
Lesson 40
Texture Manipulation
	Here we'll be directly accessing and manipulating a texture's pixels.
Lesson 41
Bitmap Fonts
	Here we'll be using a texture as a font using bitmap font techniques.
Lesson 42
Texture Streaming
	Here we'll be rendering from a streaming data source using texture streaming.
Lesson 43
Render to Texture
	Here we'll be taking a scene and rendering it to a texture.
Lesson 44
Frame Independent Movement
	Here we'll be making the dot move independent of the current frame rate.
Lesson 45
Timer Callbacks
	SDL has another timing mechanism called timer callbacks. Here we'll be setting a function to be called back after a certain amount of time.
Lesson 46
Multithreading
	Multithreading allows your program to do things simultaneously. Here we'll make things print to the console from outside our main thread.
Lesson 47
Semaphores
	A major issue in multithreaded applications is that you need to make sure that they don't try to access the same data at the same time. Semaphores are a way to make sure only a certain amount of threads are performing an action at the same time.
Lesson 48
Atomic Operations
	Atomic operations are another way to synchronize threads. Here we'll be redoing the previous tutorial with atomic counters.
Lesson 49
Mutexes and Conditions
	Mutexes and conditions are yet another way to synchronize threads. Here we'll be using the added benefit that they allow threads to communicate with each other.
Lesson 50
SDL and OpenGL 2
	SDL is a powerful tool when combined with OpenGL. If you're just starting out with OpenGL or want to maximize compatibility, you can use SDL with OpenGL 2.1. In this tutorial we will make a minimalist OpenGL 2.1 program.
Lesson 51
SDL and Modern OpenGL
	SDL 2.0 now has support for OpenGL 3.0+ with context controls. Here we'll be making a minimalist OpenGL 3+ core program.
Lesson 52
Hello Mobile
	Here we'll be loading and displaying an image in our first mobile app!
Lesson 53
Extensions and Changing Orientation
	Here we'll be using SDL extension libraries and handling changing orientation.
Lesson 54
Touches
	Here we'll be handling single touch input.
Lesson 55
Multitouch
	Here we'll be handling multitouch events like pinches and rotation.