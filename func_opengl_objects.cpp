struct TEXTURE_LIST
{
	GLuint canvas;
	GLuint palette;
	GLuint cursor;
	GLuint cursor_white;
} texture_list;

struct ARRAY_BUFFER
{ 
	GLuint circle;
	GLuint circle_spectrum;
	GLuint circle_bg_color;  
	GLuint polygon;
	GLuint polygon_uv;
	GLuint triangle;
	GLuint triangle_colors;
} array_buffer;

struct POLYGON_SHADER
{
	GLuint id;
	GLuint offset;
	GLuint color;
	GLuint scale;
	GLuint rotation;
} polygon_shader;

struct COLORWHEEL_SHADER
{
	GLuint id;
	GLuint offset;
	GLuint scale;
	GLuint rotation;
} colorwheel_shader_FLAT, colorwheel_shader_SMOOTH;

struct TEXTURE_SHADER
{
	GLuint id;
	GLuint offset;
	GLuint scale;
	GLuint sampler2d;
} texture_shader; 

// A square polygon made of two triangles
const float polygon_vertices[] =
{
	 1.0f,  1.0f,  0.0f,  1.0f,
	 1.0f, -1.0f,  0.0f,  1.0f,
	-1.0f, -1.0f,  0.0f,  1.0f, 

	 1.0f,  1.0f,  0.0f,  1.0f,
	-1.0f, -1.0f,  0.0f,  1.0f,
	-1.0f,  1.0f,  0.0f,  1.0f, 
};

const float polygon_uv_coords[] =
{
	 1.0f,  1.0f, 
	 1.0f,  0.0f, 
	 0.0f,  0.0f,

	 1.0f,  1.0f, 
	 0.0f,  0.0f,  
	 0.0f,  1.0f,  
};  

const float tri_verts[] =
{
	0.0f,  1.0f,  0.0f,  1.0f,
	-sin( 240.0f * M_DEGTORAD32 ), cos( 240.0f * M_DEGTORAD32 ),  0.0f,  1.0f,
	-sin( 120.0f * M_DEGTORAD32), cos( 120.0f * M_DEGTORAD32 ),  0.0f,  1.0f,
};

auto HSVtoRGB = [] (f4& R, f4& G, f4& B,
					f4  H, f4  S, f4  V)
{
		f4 C = V * S; // Chroma
		f4 HPrime = fmod(H / 60.0, 6);
		f4 X = C * (1 - fabs(fmod(HPrime, 2) - 1));
		f4 M = V - C;

		if(0 <= HPrime && HPrime < 1) {
		R = C; G = X; B = 0;
		} else if(1 <= HPrime && HPrime < 2) {
	    R = X; G = C; B = 0;
  	} else if(2 <= HPrime && HPrime < 3) {
	    R = 0; G = C; B = X;
	} else if(3 <= HPrime && HPrime < 4) {
	    R = 0; G = X; B = C;
	} else if(4 <= HPrime && HPrime < 5) {
	    R = X; G = 0; B = C;
	} else if(5 <= HPrime && HPrime < 6) {
	    R = C; G = 0; B = X;
	} else {
	    R = 0; G = 0; B = 0;
	}

  	R += M;
  	G += M;
  	B += M;
};

float tri_colors[12] =
{
	 0.0f, 0.0f, 0.0f, 1.0f,
	 1.0f, 1.0f, 1.0f, 1.0f,
	 0.0f, 0.0f, 0.0f, 1.0f,
};
HSVtoRGB(tri_colors[0],tri_colors[1],tri_colors[2],270.0f,1.0,1.0);   

// TODO: is 32 too many?
// TODO: a lot of things are pushed onto the stack that i don't need after using them
s4 num_pts = 32;
s4 num_data = (num_pts * 4) + 4 + 4;
f4 circle_vertices[num_data];
f4 circle_spectrum[num_data];
f4 circle_bg_color[num_data];
f4 angle = 0.0f;
f4 angle_turn = (f4)(360.0f/(f4)num_pts) * M_DEGTORAD32;
circle_vertices[0] = 0.0f;
circle_vertices[1] = 0.0f;
circle_vertices[2] = 0.0f;
circle_vertices[3] = 1.0f;
HSVtoRGB(circle_spectrum[0], circle_spectrum[1], circle_spectrum[2], 0, 1.0f, 1.0f); 
circle_spectrum[3] = 1.0f; // A
circle_bg_color[0] = user.ui_color[0];
circle_bg_color[1] = user.ui_color[1];
circle_bg_color[2] = user.ui_color[2];
circle_bg_color[3] = 1.0f; 

// TODO: rbg formula
for (s4 i = 4; i < num_data-4; i+=4)
{
	circle_vertices[i]   = 1.0f * cosf( angle );
	circle_vertices[i+1] = 1.0f * sinf( angle );
	circle_vertices[i+2] = 0.0f; // Z
	circle_vertices[i+3] = 1.0f; // A
	// f4 degrees = ((11.25f * (i/4))/ 360.0f * 255.0f)/255.0f;
	f4 degrees = (360.0f/32.0f) * (f4)(i/4);
	HSVtoRGB(circle_spectrum[i], circle_spectrum[i+1], circle_spectrum[i+2],
			 degrees, 1.0f, 1.0f); 
	circle_spectrum[i+3] = 1.0f; 	// A

	circle_bg_color[i] = user.ui_color[0];
	circle_bg_color[i+1] = user.ui_color[1];
	circle_bg_color[i+2] = user.ui_color[2];
	circle_bg_color[i+3] = 1.0f; 	// A
	angle -= angle_turn;
}
// complete circle
circle_vertices[num_data-4] = circle_vertices[4];
circle_vertices[num_data-3] = circle_vertices[5];
circle_vertices[num_data-2] = 0.0f; // Z
circle_vertices[num_data-1] = 1.0f; // A
// TODO: * 33.0f may be cutting out some red hues
HSVtoRGB(circle_spectrum[num_data-4], circle_spectrum[num_data-3], circle_spectrum[num_data-2], (360.0f/32.0f) * 33.0f, 1.0f, 1.0f); 
circle_spectrum[num_data-4] = 1.0f;	
circle_spectrum[num_data-1] = 1.0f; 	// A
circle_bg_color[num_data-4] = user.ui_color[0];	
circle_bg_color[num_data-3] = user.ui_color[1];
circle_bg_color[num_data-2] = user.ui_color[2];
circle_bg_color[num_data-1] = 1.0f; 	// A

create_buffer(array_buffer.circle, circle_vertices, sizeof(circle_vertices));
create_buffer(array_buffer.circle_spectrum, circle_spectrum, sizeof(circle_spectrum)); 
create_buffer(array_buffer.circle_bg_color, circle_bg_color, sizeof(circle_bg_color)); 

create_buffer(array_buffer.triangle, tri_verts, sizeof(tri_verts)); 
create_buffer(array_buffer.triangle_colors, tri_colors, sizeof(tri_colors));
create_buffer(array_buffer.polygon, polygon_vertices, sizeof(polygon_vertices)); 
create_buffer(array_buffer.polygon_uv, polygon_uv_coords, sizeof(polygon_uv_coords)); 

colorwheel_shader_FLAT.id = create_shader_program(colorwheel_FLAT_vert, colorwheel_FLAT_frag);
colorwheel_shader_FLAT.offset = glGetUniformLocation(colorwheel_shader_FLAT.id, "offset");
colorwheel_shader_FLAT.scale = glGetUniformLocation(colorwheel_shader_FLAT.id, "scale");
colorwheel_shader_FLAT.rotation = glGetUniformLocation(colorwheel_shader_FLAT.id, "rotation");

colorwheel_shader_SMOOTH.id = create_shader_program(colorwheel_SMOOTH_vert, colorwheel_SMOOTH_frag);
colorwheel_shader_SMOOTH.offset = glGetUniformLocation(colorwheel_shader_SMOOTH.id, "offset");
colorwheel_shader_SMOOTH.scale = glGetUniformLocation(colorwheel_shader_SMOOTH.id, "scale");
colorwheel_shader_SMOOTH.rotation = glGetUniformLocation(colorwheel_shader_SMOOTH.id, "rotation");

polygon_shader.id = create_shader_program(polygon_shader_vert, polygon_shader_frag);
polygon_shader.offset = glGetUniformLocation(polygon_shader.id, "offset");
polygon_shader.color = glGetUniformLocation(polygon_shader.id, "color");  
polygon_shader.scale = glGetUniformLocation(polygon_shader.id, "scale"); 
polygon_shader.rotation = glGetUniformLocation(polygon_shader.id, "rotation");

texture_shader.id = create_shader_program(texture_shader_vert, texture_shader_frag);
texture_shader.sampler2d = glGetUniformLocation(texture_shader.id, "myTextureSampler");  
texture_shader.scale = glGetUniformLocation(texture_shader.id, "scale");  
texture_shader.offset = glGetUniformLocation(texture_shader.id, "offset");  

// Load Textures ---------------------------
const f4 color_cursor_pixels = 10.0f; // TODO: default to this max size
texture_list.cursor = my_create_texture(color_cursor_pixels, color_cursor_pixels, true);
texture_list.cursor_white = my_create_texture(color_cursor_pixels, color_cursor_pixels, true);
texture_list.canvas = my_create_texture(SCREEN_RESOLUTION.w, SCREEN_RESOLUTION.h, false);
texture_list.palette = my_create_texture(PALETTE_WIDTH, PALETTE_HEIGHT, false);
if (texture_list.cursor == 0) { std::cout << "ERROR: Could not load color_cursor texture." << std::endl; } 
if (texture_list.cursor_white == 0) { std::cout << "ERROR: Could not load color_cursor white texture." << std::endl; } 
if (texture_list.canvas == 0)  { std::cout << "ERROR: Could not load canvas texture." << std::endl; } 
if (texture_list.palette == 0) { std::cout << "ERROR: Could not load palette texture." << std::endl; } 

// Create frame buffer ----------------------------------------
/* http://www.lighthouse3d.com/tutorials/opengl_framebuffer_objects/ */
GLuint FBO = 0;
glGenFramebuffers(1,&FBO);
glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_list.canvas, 0);
glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, texture_list.palette, 0);
glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, texture_list.cursor, 0);
glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, texture_list.cursor_white, 0);

if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
	std::cout << "ERROR: Could not create frame buffer: canvas." << std::endl;
}

// bind the framebuffer as the output framebuffer
glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
 
// Init color attachments
const GLenum GLCA_CANVAS[] = {GL_COLOR_ATTACHMENT0};
const GLenum GLCA_PALETTE[] = {GL_COLOR_ATTACHMENT1};
const GLenum GLCA_CW_CURSOR[] = {GL_COLOR_ATTACHMENT2};
const GLenum GLCA_CW_CURSOR_WHITE[] = {GL_COLOR_ATTACHMENT3};
glDrawBuffers(1, GLCA_CANVAS);
glClearColor(user.bg_color[0],user.bg_color[1],user.bg_color[2],1.0f);
glClear(GL_COLOR_BUFFER_BIT);  

glDrawBuffers(1, GLCA_PALETTE);
if (user.bg_color[0] < 0.5)
	glClearColor(user.bg_color[0]+0.2,user.bg_color[1]+0.2,user.bg_color[2]+0.2,1.0f);
else
	glClearColor(user.bg_color[0]-0.2,user.bg_color[1]-0.2,user.bg_color[2]-0.2,1.0f);
glClear(GL_COLOR_BUFFER_BIT);

glDrawBuffers(1, GLCA_CW_CURSOR);
glClearColor(0.0f,0.0f,0.0f,0.0f);
glClear(GL_COLOR_BUFFER_BIT);

glDrawBuffers(1, GLCA_CW_CURSOR_WHITE);
glClearColor(0.0f,0.0f,0.0f,0.0f);
glClear(GL_COLOR_BUFFER_BIT);

glBindFramebuffer(GL_FRAMEBUFFER,0);

// Init some OpenGL states ---------------------------------
glEnable(GL_CULL_FACE);
glCullFace(GL_BACK);
glFrontFace(GL_CW);
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
glEnableVertexAttribArray(0);
glEnableVertexAttribArray(1);
glActiveTexture(GL_TEXTURE0); 
glDepthMask(GL_FALSE);
glDepthFunc(GL_ALWAYS); 

f4 identity[] =
{
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};

// create cursor texture
glBindFramebuffer(GL_FRAMEBUFFER,FBO);
glDrawBuffers(1,GLCA_CW_CURSOR);
glViewport(0,0,color_cursor_pixels,color_cursor_pixels);
glOrtho(-color_cursor_pixels,color_cursor_pixels,-color_cursor_pixels,color_cursor_pixels,1,-1);
glUseProgram(polygon_shader.id); 
	glUniform2f(polygon_shader.scale, color_cursor_pixels,color_cursor_pixels); 
	glUniform4f(polygon_shader.color, 0.1f, 0.1f, 0.1f, 1.0f); 
	glUniform2f(polygon_shader.offset, 0, 0);
	glUniformMatrix4fv(polygon_shader.rotation, 1, false, &identity[0]);
	glBindBuffer(GL_ARRAY_BUFFER, array_buffer.circle);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)(4*sizeof(f4)));
	glDrawArrays(GL_LINE_LOOP, 0, num_data/4);

glDrawBuffers(1,GLCA_CW_CURSOR_WHITE); 
	glUniform2f(polygon_shader.scale, color_cursor_pixels,color_cursor_pixels); 
	glUniform4f(polygon_shader.color, 1.0f, 1.0f, 1.0f, 1.0f);
	glUniform2f(polygon_shader.offset, 0, 0);
	glUniformMatrix4fv(polygon_shader.rotation, 1, false, &identity[0]);
	glBindBuffer(GL_ARRAY_BUFFER, array_buffer.circle);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)(4*sizeof(f4)));
	glDrawArrays(GL_LINE_LOOP, 0, num_data/4);

glBindFramebuffer(GL_FRAMEBUFFER,0);

struct TEXTURE_TEXT
{
	GLuint texture;
	f4 w;
	f4 h;
};

struct TEXTURE_TEXT_LIST
{
	TEXTURE_TEXT help;
	TEXTURE_TEXT add_brush;
	TEXTURE_TEXT pause;
	TEXTURE_TEXT delete_brush;
	TEXTURE_TEXT edit_mode;
	TEXTURE_TEXT number[10];
	TEXTURE_TEXT fade_text;
	TEXTURE_TEXT minsec;
	TEXTURE_TEXT plus;
	TEXTURE_TEXT minus;
	TEXTURE_TEXT small_menu_1;
	TEXTURE_TEXT small_menu_2;
} text_list;

auto create_writing = [&] (TEXTURE_TEXT &TT, const char* stuff, u1 R, u1 G, u1 B)
{
	SDL_Color text_color = { R,G,B, 255 };
	std::ostringstream stream;
	stream << stuff;
	std::string display_text = stream.str();
	SDL_Surface* sdl_surface = TTF_RenderUTF8_Blended(font, display_text.c_str(), text_color);
	if( sdl_surface == NULL ) { std::cout << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl; }
	TT.texture = create_font_texture(sdl_surface);
	TT.w = (f4)sdl_surface->w;
	TT.h = (f4)sdl_surface->h;
	SDL_FreeSurface( sdl_surface );
};

create_writing(text_list.help,"[TAB menu] [SPACE pause] [CTRL+SPACE lock/unlock pause] [ENTER save] [ESC quit]",0,0,0);
create_writing(text_list.add_brush,"CREATE",255,255,255);
create_writing(text_list.pause,"PAUSE",255,255,255);
create_writing(text_list.delete_brush,"X",255,255,255);
create_writing(text_list.edit_mode,"EDIT MODE",255,255,255);
create_writing(text_list.number[0],"0",0,0,0);
create_writing(text_list.number[1],"1",0,0,0);
create_writing(text_list.number[2],"2",0,0,0);
create_writing(text_list.number[3],"3",0,0,0);
create_writing(text_list.number[4],"4",0,0,0);
create_writing(text_list.number[5],"5",0,0,0);
create_writing(text_list.number[6],"6",0,0,0);
create_writing(text_list.number[7],"7",0,0,0);
create_writing(text_list.number[8],"8",0,0,0);
create_writing(text_list.number[9],"9",0,0,0);
create_writing(text_list.fade_text,"FADES IN",0,0,0);
create_writing(text_list.minsec,"[MIN] [SEC]",0,0,0);
create_writing(text_list.plus,"+",0,0,0);
create_writing(text_list.minus,"-",0,0,0);
create_writing(text_list.small_menu_1,"Window too small.",0,0,0);
create_writing(text_list.small_menu_2,"Resize to access.",0,0,0);