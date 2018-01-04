/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL, SDL OpenGL, GLEW, standard IO, and strings
#include <SDL.h>

#define GLEW_STATIC
#include <GL/glew.h> 
#include "../glew-1.13.0/src/glew.c" 
#include <SDL_opengl.h>

#include <stdio.h>
#include <iostream>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
  
int main( int argc, char* args[] )
{
	//The window we'll be rendering to
	SDL_Window* window = NULL;

	//OpenGL context
	SDL_GLContext gl_context;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
		// TODO: write an safely_crash_program("text to display", optional error text sdl_geterror()); function
		return 0;
	} 

	//Use OpenGL 3.1 core TODO: check these
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	//Create window 
	window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
	if( window == NULL )
	{
		std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
		return 0;
	}

	//Create context
	gl_context = SDL_GL_CreateContext( window );
	if( gl_context == NULL )
	{
		std::cout << "OpenGL context could not be created! SDL Error: %s\n" << SDL_GetError() << std::endl;
		return 0;
	} 

	//Initialize GLEW
	glewExperimental = GL_TRUE; // TODO: research this
	GLenum glewError = glewInit();
	if( glewError != GLEW_OK )
	{
		printf( "Error initializing GLEW! %s\n", glewGetErrorString( glewError ) );
	}

	// Use Vsync
	// TODO: this
	if( SDL_GL_SetSwapInterval( 1 ) < 0 )
	{
		printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
	}

	// //Initialize OpenGL
	// if( !initGL() )
	// {
	// 	printf( "Unable to initialize OpenGL!\n" );
	// 	return 0;
	// } 

	 
	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;
	
	//Enable text input
	SDL_StartTextInput();

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
			//Handle keypress with current mouse position
			else if( e.type == SDL_TEXTINPUT )
			{
				int x = 0, y = 0;
				SDL_GetMouseState( &x, &y );
				// handleKeys( e.text.text[ 0 ], x, y );
			}
		}  

		POINT curr_cursor_pos;
		DWORD message_time = GetMessageTime();
		GetCursorPos(&curr_cursor_pos);  
		
		//Update screen
		SDL_GL_SwapWindow( window );
	}
	
	//Disable text input
	SDL_StopTextInput(); 

	//Free resources and close SDL
	//Deallocate program
	// glDeleteProgram( gProgramID );

	//Destroy window	
	SDL_DestroyWindow( window );
	window = NULL;

	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}