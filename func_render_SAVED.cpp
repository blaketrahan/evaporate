{
 
b4 PICK_COLOR_FROM_COLOR_CIRCLE = false;
b4 PICK_COLOR_FROM_COLOR_TRIANGLE = false;
b4 PICKUP_A_COLOR = false;
b4 COLLECT_MOUSE_PTS = false;
b4 DRAW_ON_PALETTE = false;
b4 DRAW_ON_CANVAS = false;

s4 mp_size = 0;
enum {
	CS_NULL = 0, CS_DRAWING, CS_PALETTE_PICKING, CS_PALETTE_EDITING,
	CS_BUTTON0, CS_BUTTON1, CS_BUTTON2, CS_BUTTON3,
	CS_COLORPICKER_CIRCLE, CS_COLORPICKER_TRIANGLE, CS_ADDBRUSH,
	CS_ADDBRUSHSLIDER, CS_FADER,
	CS_BUTTON_DELETE0, CS_BUTTON_DELETE1, CS_BUTTON_DELETE2, CS_BUTTON_DELETE3,
	CS_FADER_MIN_UP, CS_FADER_MIN_DOWN, CS_FADER_SEC_UP, CS_FADER_SEC_DOWN
};
s4 mouseover_state = CS_NULL;
s4 sdl_cursor_x, sdl_cursor_y;
SDL_GetMouseState(&sdl_cursor_x, &sdl_cursor_y);
f4pt msp;
{ 
	int window_pos_x, window_pos_y;
	SDL_GetWindowPosition(window, &window_pos_x, &window_pos_y);
	msp.x = (f4)(sdl_cursor_x) / (f4)user.screen_width * 2.0f;// - 1.0f;
	msp.y = (f4)(sdl_cursor_y) / (f4)user.screen_height * -2.0f + 2.0f;

	#include "func_collision.cpp"

	#ifdef IS_WINDOWS_BUILD
		if ( COLLECT_MOUSE_PTS )
		{
			// TODO: Change this from 64 to a reasonable number, 8 or 16?
			// Get previous mouse positions ------------------
			MOUSEMOVEPOINT win_mpin = { sdl_cursor_x + window_pos_x, sdl_cursor_y + window_pos_y, 0 };
			MOUSEMOVEPOINT win_mpout[64];
			int win_num_pts = GetMouseMovePointsEx(sizeof(win_mpin), &win_mpin, win_mpout, 64, GMMP_USE_DISPLAY_POINTS);
			if (win_num_pts == -1) // TODO: ERROR CHECKING (former assert)
			{
				std::cout << "ERROR: win_num_pts == -1" << std::endl;
			}
			b4 catch_two_more_for_catmull = false;	
			
			for (int i = 0; i < win_num_pts ; i++)
			{  
				// process and copy into mpts[]
				POINT aa = {win_mpout[i].x, win_mpout[i].y}; 
				
				aa.x -= window_pos_x;
				aa.y -= window_pos_y;

				mpts[mp_size].x = (f4)aa.x / (f4)user.screen_width * 2.0f;
				mpts[mp_size].y = (f4)aa.y / (f4)user.screen_height * -2.0f + 2.0f; 
				// mpts[mp_size].x = (f4)aa.x / (f4)user.screen_width * 2.0f - 1.0f;
				// mpts[mp_size].y = (f4)aa.y / (f4)user.screen_height * -2.0f + 1.0f; 
				mp_size++;
				// TODO: Figure out this2 win_num_pts+3 and etc
				if (win_mpout[i].time < last_mt && !catch_two_more_for_catmull)
				{
					win_num_pts = i + 3;
					catch_two_more_for_catmull = true;
				} 
			} 
			// assert(win_num_pts >= mp_size + 1); // TODO: assert		
		}
		DWORD message_time = GetMessageTime();
		last_mt = message_time;
	#endif
}

auto catmullRomSpline = []
( f4 x, f4 v0, f4 v1, f4 v2, f4 v3 ) -> f4
{
    f4 c1,c2,c3,c4;

    c1 =          1.0*v1;
    c2 = -0.5*v0          + 0.5*v2;
    c3 = 1.0*v0 + -2.5*v1 + 2.0*v2 + -0.5*v3;
    c4 = -0.5*v0 + 1.5*v1 + -1.5*v2 + 0.5*v3;

    return (((c4*x + c3)*x +c2)*x + c1);
};

// TODO: incorporate bezier lines like gimp into this
// for mpts closer than 6 pixels
// TODO: Figure out this -4 and etc

s4 num_smoothed_pt = 0;
for (s4 i = 0; i < mp_size-4; i++)
{ 
	for (f4 u = 1.0f; u >= 0.0f; u -= 0.2f )
	{ 
		f4 px = catmullRomSpline(u, mpts[i+3].x, mpts[i+2].x, mpts[i+1].x, mpts[i].x);
		f4 py = catmullRomSpline(u, mpts[i+3].y, mpts[i+2].y, mpts[i+1].y, mpts[i].y);
		msmoothpts[num_smoothed_pt].x = px;
		msmoothpts[num_smoothed_pt].y = py;
		num_smoothed_pt++;
	}
}

auto draw_call_polygon = [&] (
	f4 R, f4 G, f4 B, f4 A, // color
	f4 W, f4 H, f4 X, f4 Y, f4* RMAT, // size, position, rotation
	GLuint BUF, // array buffer
	GLenum DRAW_TYPE, // gl_triangles, gl_triangle_fan etc
	GLsizei NUM_VERT) // number of vertices
{
	glUseProgram(polygon_shader.id);
	glUniform4f(polygon_shader.color, R, G, B, A);
	glUniform2f(polygon_shader.scale, W, H); 
	glUniform2f(polygon_shader.offset, X, Y); 
	glUniformMatrix4fv(polygon_shader.rotation, 1, false, RMAT);
	glBindBuffer(GL_ARRAY_BUFFER, BUF);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(DRAW_TYPE, 0, NUM_VERT); 
};

auto draw_call_texture = [&] ( f4 W, f4 H, f4 X, f4 Y, GLuint Texture) 
{
	glBindTexture(GL_TEXTURE_2D, Texture); 
	glUseProgram(texture_shader.id);
	glUniform2f(texture_shader.scale,W,H);
	glUniform2f(texture_shader.offset,X,Y);
	glUniform1i(texture_shader.sampler2d, 0); // set texture unit 0 
	glBindBuffer(GL_ARRAY_BUFFER, array_buffer.polygon);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, array_buffer.polygon_uv);
	glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(void*)0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
};

auto draw_call_font = [&] (TEXTURE_TEXT &TT, f4 X, f4 Y, f4 SCALE = 0.5f)
{
	glPushMatrix();
	glScalef(SCALE,-SCALE,0.5);
	glDisable(GL_CULL_FACE);
	draw_call_texture(TT.w, TT.h, X, Y, TT.texture);
	glEnable(GL_CULL_FACE);	
	glPopMatrix();
};

auto set_viewport_projection = [&] (f4 vX, f4 vY, f4 vX2, f4 vY2, f4 pX, f4 pX2, f4 pY, f4 pY2)
{
	glViewport(vX,vY,vX2,vY2);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(pX,pX2,pY,pY2,-1.0f,1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
};

glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
glDrawBuffers(1, GLCA_CANVAS);

set_viewport_projection(0,0,user.screen_width,user.screen_height,-1.0f,1.0f,-1.0f,1.0f);

// TODO: Test if this is more accurate than using render_ms over long fades
if (!user.is_paused)
{
	f4 fadeamount = user.alpha * render_dt;	
	draw_call_polygon(
		user.bg_color[0], user.bg_color[1], user.bg_color[2], fadeamount,
		user.screen_width, user.screen_height, 1.0f, 1.0f, &identity[0],
		array_buffer.polygon, GL_TRIANGLES, 6
	); 
}
render_dt = 0.0f;

glUseProgram(polygon_shader.id);
glUniform4f(polygon_shader.color, selected_color[0], selected_color[1], selected_color[2], 1.0f);
glUniform2f(polygon_shader.scale, user.brush_radius, user.brush_radius); 
glUniformMatrix4fv(polygon_shader.rotation, 1, false, &identity[0]);
glBindBuffer(GL_ARRAY_BUFFER, array_buffer.circle);
glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

if (DRAW_ON_PALETTE)
{
	glDrawBuffers(1, GLCA_PALETTE);

	set_viewport_projection(0,0,PALETTE_WIDTH, PALETTE_HEIGHT,
		-PALETTE_WIDTH,PALETTE_WIDTH, -PALETTE_HEIGHT, PALETTE_HEIGHT);

	for (s4 i = 0; i < num_smoothed_pt; i++)
	{
		msmoothpts[i].x = (msmoothpts[i].x - ui_palette.x_gl)/ui_palette.w_gl;
		msmoothpts[i].y = (msmoothpts[i].y - ui_palette.y_gl)/ui_palette.h_gl;
		glUniform2f(polygon_shader.offset, msmoothpts[i].x, msmoothpts[i].y);
		glDrawArrays(GL_TRIANGLE_FAN, 0, num_data/4);
	}
}
else if (DRAW_ON_CANVAS)
{   
	set_viewport_projection(0,0,user.screen_width, user.screen_height,
		0,user.screen_width*2, 0, user.screen_height*2);
	
	for (s4 i = 0; i < num_smoothed_pt; i++)
	{  
		glUniform2f(polygon_shader.offset, msmoothpts[i].x, msmoothpts[i].y);
		glDrawArrays(GL_TRIANGLE_FAN, 0, num_data/4);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, array_buffer.polygon);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// Draw brush lines ----------------------------------  
	auto calculate_n_draw_line = [&] (f4pt prev_mp, f4pt next_mp)
	{
		f4 dx = (next_mp.x - prev_mp.x);
		f4 dy = (next_mp.y - prev_mp.y);
		f4 pos_x = dx / 2.0f;
		f4 pos_y = dy / 2.0f; 
		glUniform2f(polygon_shader.offset, prev_mp.x + pos_x, prev_mp.y + pos_y);
		
		// TODO: figure out how to do this without drawing a skewed line
		// Answer 1: Apply rotation matrix to a scale matrix
		// Answer 2: change viewport?
		dx *= user.screen_width;
		dy *= user.screen_height;
		f4 dx2 = dx * dx;
		f4 dy2 = dy * dy;
		f4 result = sqrt(dx2 + dy2)/2.0f;
		f4 line_scale_x = result;
		f4 line_scale_y = user.brush_radius;
		glUniform2f(polygon_shader.scale, line_scale_x, line_scale_y); 

		f4 arc_tan = atan2(dy, dx); 
		const f4 RotationMatrix[] =
		{
			(f4)cos( arc_tan ), (f4)-sin( arc_tan ), 0.0f, 0.0f,
			(f4)sin( arc_tan ), (f4) cos( arc_tan ), 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		glUniformMatrix4fv(polygon_shader.rotation, 1, false, &RotationMatrix[0]);
		glDrawArrays(GL_TRIANGLES, 0, 6); 
	}; 

	// Connect remaining lines caught in the current frame.
	for (s4 i = 0; i < num_smoothed_pt-1; i++)
	{
		// if (i % 2 == 1) { continue; } // for debug
		calculate_n_draw_line(msmoothpts[i], msmoothpts[i+1]);
	} 
}
// TODO: unbind palette buffer if CS_COLOR_EDITING
// glBindFramebuffer(GL_FRAMEBUFFER, fbo_list.canvas);

if (USER_SAVE_IMAGE_FILE)
{
	#include "func_save_image.cpp"
}

glBindFramebuffer(GL_FRAMEBUFFER,0);
set_viewport_projection(0,0,SCREEN_RESOLUTION.w,SCREEN_RESOLUTION.h, 0,SCREEN_RESOLUTION.w*2, 0,SCREEN_RESOLUTION.h*2);
draw_call_texture(SCREEN_RESOLUTION.w,SCREEN_RESOLUTION.h,1.0f,1.0f,texture_list.canvas);

// TODO: optimize? maybe
b4 ui_is_dirty = true;
if (ui_is_dirty)
{ 
	#include "func_draw_interface.cpp"
}

// Render text surface
glPushMatrix();
	glScalef(0.5,-0.5,0.5);
	glDisable(GL_CULL_FACE);
	draw_call_texture(text_list.help.w, text_list.help.h,
		0.0f + (f4)text_list.help.w/(f4)user.screen_width/2.0f,
		0.0f + (f4)text_list.help.h/(f4)user.screen_height/2.0f,text_list.help.texture);
	glEnable(GL_CULL_FACE);	
glPopMatrix();

if (PICKUP_A_COLOR)
{
	s4 pixel_x = sdl_cursor_x;
	s4 pixel_y = (sdl_cursor_y * -1) + user.screen_height; // flip y coordinate
	glReadPixels(pixel_x,pixel_y,1,1,GL_RGB,GL_FLOAT,&selected_color[0]);
}

draw_call_texture(user.brush_radius,user.brush_radius,msp.x, msp.y,texture_list.cursor);
draw_call_texture(10,10,msp.x, msp.y,texture_list.cursor);

//Update screen
SDL_GL_SwapWindow( window );

// auto add_bezier = []
// (
// 	f4pt* pts_out, s4 &counter,
// 	f4pt a, f4pt b,
// 	f4 c_x, f4 c_y
// )
// {
// 	auto get_point = [] (f4 n1 , f4 n2 , f4 perc) -> f4
// 	{
// 		f4 diff = n2 - n1;
// 	    return n1 + ( diff * perc );
// 	};

// 	for( f4 i = 0 ; i <= 1.0f ; i += 0.1f ) // TODO: i <= 1.0f or... just add final poiint of mp_size at the very end
// 	{
// 	    // The Green Line
// 	    f4 Ax = get_point( a.x , c_x , i );
// 	    f4 Ay = get_point( a.y , c_y , i );
// 	    f4 Bx = get_point( c_x , b.x , i );
// 	    f4 By = get_point( c_y , b.y , i );

// 	    // the processed point
// 	    pts_out[counter].x = get_point( Ax , Bx , i );
// 	    pts_out[counter].y = get_point( Ay , By , i );
// 	    counter++;
// 	}
// };
} // TODO: get rid of these silly scopes in all files