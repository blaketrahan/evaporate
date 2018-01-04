/*
	Project: Evaporate
	Author: Blake Trahan www.blaketrahan.com
	Date: December 2015 - 2016

	TODO: Immediate list  
	- fix brush and palette relative sizes

	TODO list
	- figure out resolution independent rendering
	- add color picking from image
	- can't draw dots yet bc they're not lines
	- something funny MAY be happening with line drawing, see picture
	- my drawing window is 0 to 2.0 in x,y coordinates. maybe i need to change this?
	- investigate using glpushmatrix glpopmatrix instead of glortho
	- color wheel
		- color wheel shader: http://stackoverflow.com/questions/13210998/opengl-colour-interpolation
		- make color wheel circles triangles instead of triangle fan, so it doesnt lerp all the values at the center
		- overall fine tuning
		- add an eye dropper icon
	- UI
		- scale itself to fit window dimensions
	- clean up render code
		- use glpush and glpop matrix
		- group together draw calls and program flow in a sane way
	- what should happen when holding resize window?
	- when leaving menu, keep pause on if user set it before opening menu
	- do i need to release opengl assets before exit?

	features todo:
	- grab and pull image around when it's smaller than full screen
	- easytab.h
	- basic color blending
	- anti-alias lines
	- add "layers"? 
			- save current drawing to a layer. it stops fading.
			- begins drawing on a top layer
			- edit layer image file to add an image into program
			- draw on top of say, section and plan, or wip painting
	- user options:
		- lock when mouse is off screen
		- save location
		- background color
	- if not pressing a hotkey, display hotkey list at bottom of screen
	- saving all settings and palette to a settings file
*/

#include <iostream>
#include <iomanip>

// TODO: research these
#define IS_WINDOWS_BUILD
#ifdef IS_WINDOWS_BUILD
	#define NTDDI_VERSION 0x05010200 
	#define _WIN32_WINNT 0x0502  
	#define WINVER 0x0502 
#endif

#include <SDL.h>
#include "SDL_syswm.h"
#include <SDL_ttf.h>

#define GLEW_STATIC
#include <GL/glew.h> 
#include "../glew-1.13.0/src/glew.c" 
#include <SDL_opengl.h>

#include "../bullet/Bullet/LinearMath/btQuickprof.h"
 
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// For filenames
#include <sstream>
#include <string>
#include <time.h>

#include <vector>
#include <algorithm>

#include "memory.h" 

#define EASYTAB_IMPLEMENTATION
#include "easytab.h"

#define DEBUG_BUILD

#ifdef DEBUG_BUILD
	#include "hexdump.c"
#endif

#define M_PI32 3.14159265359f
#define M_DEGTORAD32 M_PI32 / 180.0f
#define M_RADTODEG32 180.0f / M_PI32

// TODO: properly place these
#define MAX_NUM_BRUSH 4

struct EvaporateUserSettings {
	// paint settings --- 
	f4 alpha;
	f4 bg_color[3];
	f4 brush_radius;

	// tool settings ---
	s4 num_brush;

	// interface settings ---
	s4 ui_button_size;
	f4 ui_color[3];

	// display settings ---
	s4 screen_width;
	s4 screen_height;
	u4 SDL_flags;

	// input ---
	b4 dragging;
	b4 is_paused; 
	b4 is_pause_locked; 
	b4 initial_click;
	s4 initial_click_mouseover_state;
	b4 has_clicked;
	b4 ctrl_is_pressed;
} user; 

#include "func_shaders.cpp"
#include "func_shader_creation.cpp"

b4 USER_SAVE_IMAGE_FILE = false;

int main( int argc, char* args[] )
{
	SDL_Window* window = NULL;//The window we'll be rendering to
	SDL_GLContext gl_context; //OpenGL context
	initialize_memory(memory, 10); // 10 megabytes

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
		// TODO: write a safely_crash_program("text to display", optional error text sdl_geterror()); function
		return 0;
	} 
	// TODO: things arent scaling correctly when changing aspect ratio
	// TODO: because i was not using power of 2 texture FBO 
	// TODO: I think this was fixed by using SDL?
	#define TESTING_DESKTOP_WINDOWED
	// #define TESTING_DESKTOP_SMALL
	// #define TESTING_DESKTOP_PHONE
	// #define TESTING_DESKTOP_FULLSCREEN

	SDL_DisplayMode SCREEN_RESOLUTION;
	if (SDL_GetDesktopDisplayMode(0, &SCREEN_RESOLUTION) != 0)
	{
		std::cout << "ERROR: could not get SDL_GetDesktopDisplayMode" << SDL_GetError() << std::endl;
	}

	s4 PALETTE_WIDTH = 128;
	s4 PALETTE_HEIGHT = 512;

	#ifdef TESTING_DESKTOP_WINDOWED
		user.screen_width = 1280;
		user.screen_height = 800;
		user.SDL_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
	#endif

	#ifdef TESTING_DESKTOP_SMALL
		PALETTE_HEIGHT = 256;
		user.screen_width = 800;
		user.screen_height = 600;
		user.SDL_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
	#endif

	#ifdef TESTING_DESKTOP_PHONE
		user.screen_width = 1024;
		user.screen_height = 800;
		user.SDL_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
	#endif

	#ifdef TESTING_DESKTOP_FULLSCREEN
		user.screen_width = SCREEN_RESOLUTION.w;
		user.screen_height = SCREEN_RESOLUTION.h;
		user.SDL_flags = SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
	#endif

	// TODO: check these
	//Use OpenGL 3.1 core
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	//Create window 
	window = SDL_CreateWindow(
		"Evaporate Draw",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		user.screen_width,
		user.screen_height,
		user.SDL_flags);
	if( window == NULL )
	{
		std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
		return 0;
	}

	//Create context
	gl_context = SDL_GL_CreateContext( window );
	if( gl_context == NULL )
	{
		std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
		return 0;
	} 

	//Initialize GLEW
	glewExperimental = GL_TRUE; // TODO: research this
	GLenum glewError = glewInit();
	if( glewError != GLEW_OK )
	{
		std::cout << "Error initializing GLEW! " << glewGetErrorString( glewError ) << std::endl;
	}

	// NOT DOING THIS. Causes mouse drawing lag.
	// Use Vsync
	// TODO: this
	// if( SDL_GL_SetSwapInterval( 1 ) < 0 )
	// {
	// 	printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
	// }

	SDL_ShowCursor(0);
	btClock* btclock = (btClock*)alloc(memory,sizeof(btClock)); 
	new (btclock) btClock(); 

	GLenum err = glewInit();
	if (GLEW_OK != err) { std::cout << "ERROR: Failed glewInit()." << std::endl; }
	std::cout << "Using GLEW Version: " << glewGetString(GLEW_VERSION) << std::endl;
	if (GLEW_VERSION_1_5) { std::cout << "Core extensions of OpenGL 1.1 to 1.5 are available!" << std::endl; }

	#ifdef IS_WINDOWS_BUILD
		SDL_EventState(SDL_SYSWMEVENT,SDL_ENABLE);

		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		if (!SDL_GetWindowWMInfo(window,&info))
		{
			std::cout << "ERROR: Failed to SDL_GetWindowWMInfo: " << SDL_GetError() << std::endl;
			return 0;
		} 
		int easy_tab_success = EasyTab_Load(info.info.win.window);
		if (easy_tab_success != EASYTAB_OK)
		{
			std::cout << "ERROR: Failed to load easy tab. " << std::endl;
		}
	#endif

	//Initialize SDL_ttf --------------------------
    if( TTF_Init() == -1 )
    {
        std::cout << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    TTF_Font* font = TTF_OpenFont( "cousine-regular-latin.ttf", 28 ); // TODO: file location
    if( font == NULL ) { std::cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl; }    
    
    // SDL_ttf ------------------------------------
 	
    #define ONE_SECOND_FADE 6.0f
    #define DEFAULT_FADE 0.2f // 30 seconds
    // 6.0 = 1 seconds
	user.alpha = DEFAULT_FADE;
	user.bg_color[0] = 0.85f;//0.2f;//1.0f;//0.8f;
	user.bg_color[1] = 0.82f;//0.2f;//1.0f;//0.8f;
	user.bg_color[2] = 0.80f;//0.2f;//1.0f;//0.8f;
	user.brush_radius = 10.0f;
	user.ui_button_size = 24; // pixels
	user.is_paused = false;
	user.is_pause_locked = false;
	user.has_clicked = false;
	user.initial_click = false;
	user.initial_click_mouseover_state = 0;
	user.dragging = false;
	user.ctrl_is_pressed = false;
	user.num_brush = 1;
	user.ui_color[0] = 0.5f;
	user.ui_color[1] = 0.5f;
	user.ui_color[2] = 0.5f;

	#include "func_opengl_objects.cpp"

	f4 aspect_x = 1.0f;
	f4 aspect_y = 1.0f; 
	auto calculate_aspect_ratio = [&] ()
	{
		aspect_x = 1.0f;
		aspect_y = 1.0f;
		
		if (user.screen_width > user.screen_height)
		{ 
			aspect_y = (f4)user.screen_width/(f4)user.screen_height; 
		} else // TODO: test this
		{ 
			aspect_x = (f4)user.screen_height/(f4)user.screen_width; 
		}
	};
	calculate_aspect_ratio();

	auto remove_aspect_ratio = [=] (f4 &x, f4 &y)
	{
		x/=aspect_x;
		y/=aspect_y;
	};

	auto reapply_aspect_ratio = [=] (f4 &x, f4 &y)
	{
		x*=aspect_x;
		y*=aspect_y;
	};
	
	s4 max_data_size = 3/*NUM_COMPONENTS*/ * SCREEN_RESOLUTION.w * SCREEN_RESOLUTION.h;
	GLubyte* saved_png = (GLubyte*)alloc(memory,sizeof(GLubyte) * max_data_size);
	GLubyte* flipped_png = (GLubyte*)alloc(memory,sizeof(GLubyte) * max_data_size);
	
	// TODO: What's a good number so that this will never reach its cap
	const s4 SAFE_ALLOTMENT_MOUSE = 64; 
	struct f4pt {  f4 x; f4 y; }; 
	f4pt* mpts = (f4pt*)alloc(memory,sizeof(f4pt) * SAFE_ALLOTMENT_MOUSE);

	struct MY_EASYTAB_HISTORY
	{
		struct PT
		{
			s4 x,y;
			u4 time;
		};
		PT pts[64];
		s4 last_pt;
		u4 last_query;
	} easytab_history;
	easytab_history.last_pt = 0;
	easytab_history.last_query = 0;
	for (s4 i = 0; i < 64; i++)
	{
		easytab_history.pts[i].x = 0;
		easytab_history.pts[i].y = 0;
		easytab_history.pts[i].time = 0;
	}
	
	// TODO: replace safe_allotment * 10 with number of points generated for each catmull/bezier curve
	f4pt* msmoothpts = (f4pt*)alloc(memory,sizeof(f4pt) * SAFE_ALLOTMENT_MOUSE * 10); 
	
	// TODO: move this into user
	f4 selected_color[3] = {0.0f,0.0f,0.0f};
	
	// TODO: surround this in ifdef windows build
	DWORD last_mt = 0; 

	//Enable text input
	SDL_StartTextInput();

	#include "func_interface.cpp"
	
	f4         render_dt = 0.0f;
	const f4   render_ms = 1.0f/120.0f;
	u4         time_physics_prev = btclock->getTimeMicroseconds(); 

	b4 QUIT_APP = false;

	while(!QUIT_APP)
	{
		const u4 time_physics_curr = btclock->getTimeMicroseconds();
		const f4 frameTime = ((f4)(time_physics_curr - time_physics_prev)) / 1000000.0f; // todo: is this truncated correctly?
		time_physics_prev = time_physics_curr;
		
		{
			#include "func_input.cpp"
		}
		// stringw str =L"Enter: save | Tab: hide color | Left mouse: draw | Right mouse: eyedropper | Space: pause (";

		render_dt += frameTime;
		if (render_dt >= render_ms )
		{
			#include "func_render.cpp"
		}
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1); 

	EasyTab_Unload();

	free(memory.TransientStorage);
	free(memory.PermanentStorage);
	
	TTF_CloseFont( font );
	font = NULL;

	//Destroy window
	SDL_DestroyWindow( window );
	window = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	SDL_Quit();
	return 0;
}
