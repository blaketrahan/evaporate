{
	auto set_RGBA = [] (f4 &R, f4 &G, f4 &B, f4 &A, f4* c) { R = c[0]; G = c[1]; B = c[2]; A = c[3]; };
	
	auto draw_winfo = [&] (EUIWINDOW &winfo, b4 highlighted = false, f4 x_offset = 0.0f, f4 y_offset = 0.0f)
	{
		f4 R,G,B,A;
		if (highlighted)
			set_RGBA(R,G,B,A,winfo.colors_ON);
		else
			set_RGBA(R,G,B,A,winfo.colors);

		draw_call_polygon( R,G,B,A, winfo.w_pixels, winfo.h_pixels, winfo.x_gl + x_offset, winfo.y_gl + y_offset,
			identity, array_buffer.polygon, GL_TRIANGLES, 6);
	};

	set_viewport_projection(0,0,user.screen_width,user.screen_height,0,user.screen_width*2, 0, user.screen_height*2);

	draw_call_texture(ui_palette.w_pixels,ui_palette.h_pixels,ui_palette.x_gl,ui_palette.y_gl,texture_list.palette);

	// Draw brush buttons ------
	f4 button_offset = 0;
	for (s4 i = 0; i < 4; i++)
	{
		if (!ui_brush_list.is_set[i]) continue; 
		draw_winfo(ui_brush_list.winfo, (mouseover_state - CS_BUTTON0) == i, 0.0f, ui_brush_list.winfo.h_gl * 2.0f * (f4)i);
	}
	for (s4 i = 0; i < user.num_brush; i++)
	{
		if (!ui_brush_list.is_set[i]) continue;
		draw_call_polygon( 0.0f,0.0f,0.0f,1.0f, ui_brush_list.brush_sizes[i], ui_brush_list.brush_sizes[i],
			ui_brush_list.winfo.x_gl, ui_brush_list.winfo.y_gl + (ui_brush_list.winfo.h_gl * 2.0f * (f4)i),
			&identity[0], array_buffer.circle, GL_TRIANGLE_FAN, num_data/4); 		
	}
	// -------------------------

	if (user.is_paused)
	{
		draw_winfo(ui_pause.winfo, user.is_pause_locked);  
		draw_call_font(text_list.pause,ui_pause.winfo.x_gl, ui_pause.winfo.y_gl);
	}

	if (ui_menu.winfo.visible)
	{
		// palette edit mode text
		draw_call_font(text_list.edit_mode,ui_palette.x_gl, ui_palette.y_gl);
		
		// Draw DELETE brush buttons ------
		button_offset = 0;
		for (s4 i = 0; i < 4; i++)
		{
			if (!ui_brush_list.is_set[i]) continue; 
			draw_winfo(ui_brush_list.winfo_delete, (mouseover_state - CS_BUTTON_DELETE0) == i, 0.0f, ui_brush_list.winfo_delete.h_gl * 2.0f * (f4)i);
			draw_call_font(text_list.delete_brush,ui_brush_list.winfo_delete.x_gl, ui_brush_list.winfo_delete.y_gl + ui_brush_list.winfo_delete.h_gl * 2.0f * (f4)i,1.0f);
		} 

		// the window 		-/-/-/-/-/-/-/-/
		if (ui_menu.too_small)
		{
			draw_winfo(ui_menu.winfo, true); 
			draw_call_font(text_list.small_menu_1,ui_menu.winfo.x_gl,ui_menu.winfo.y_gl + (f4)text_list.small_menu_1.h/(f4)user.screen_height);
			draw_call_font(text_list.small_menu_2,ui_menu.winfo.x_gl,ui_menu.winfo.y_gl - (f4)text_list.small_menu_1.h/(f4)user.screen_height);
		}
		else
		{
			draw_winfo(ui_menu.winfo);
			// fader ui 		-/-/-/-/-/-/-/-/
			f4 x_offset[4];
			x_offset[0] = ui_fader.winfo_digit.w_gl * -4.0f + (ui_fader.winfo.w_gl * -1.0f);
			x_offset[1] = ui_fader.winfo_digit.w_gl * -2.0f + (ui_fader.winfo.w_gl * -1.0f);
			x_offset[2] = ui_fader.winfo.w_gl * 2.0f;
			x_offset[3] = (ui_fader.winfo_digit.w_gl * 2.0f) + (ui_fader.winfo.w_gl * 2.0f);
			draw_winfo(ui_fader.winfo_digit, false, x_offset[0]);
			draw_winfo(ui_fader.winfo_digit, false, x_offset[1]);
			draw_winfo(ui_fader.winfo_digit, false, x_offset[2]);
			draw_winfo(ui_fader.winfo_digit, false, x_offset[3]);

			draw_call_font(text_list.number[ ui_fader.min[0] ],ui_fader.winfo.x_gl + x_offset[0], ui_fader.winfo.y_gl, 1.0f);
			draw_call_font(text_list.number[ ui_fader.min[1] ],ui_fader.winfo.x_gl + x_offset[1], ui_fader.winfo.y_gl, 1.0f);
			draw_call_font(text_list.number[ ui_fader.sec[0] ],ui_fader.winfo.x_gl + x_offset[2], ui_fader.winfo.y_gl, 1.0f);
			draw_call_font(text_list.number[ ui_fader.sec[1] ],ui_fader.winfo.x_gl + x_offset[3], ui_fader.winfo.y_gl, 1.0f);

			draw_call_font(text_list.fade_text,ui_fader.winfo.x_gl,
				ui_fader.winfo.y_gl + (text_list.minsec.h/(f4)user.screen_height) * 2.0f + (text_list.fade_text.h/(f4)user.screen_height));
			draw_call_font(text_list.minsec,ui_fader.winfo.x_gl,
				ui_fader.winfo.y_gl + (text_list.minsec.h/(f4)user.screen_height) * 2.0f);

			draw_winfo(ui_fader.winfo, mouseover_state == CS_FADER_MIN_UP, 	x_offset[1] + ui_fader.winfo.w_gl + ui_fader.winfo_digit.w_gl, ui_fader.winfo.h_gl);
			draw_winfo(ui_fader.winfo, mouseover_state == CS_FADER_MIN_DOWN,x_offset[1] + ui_fader.winfo.w_gl + ui_fader.winfo_digit.w_gl, -ui_fader.winfo.h_gl);
			draw_winfo(ui_fader.winfo, mouseover_state == CS_FADER_SEC_UP, 	x_offset[3] + ui_fader.winfo.w_gl + ui_fader.winfo_digit.w_gl, ui_fader.winfo.h_gl);
			draw_winfo(ui_fader.winfo, mouseover_state == CS_FADER_SEC_DOWN,x_offset[3] + ui_fader.winfo.w_gl + ui_fader.winfo_digit.w_gl, -ui_fader.winfo.h_gl);

			draw_call_font(text_list.plus,ui_fader.winfo.x_gl + x_offset[1] + ui_fader.winfo.w_gl + ui_fader.winfo_digit.w_gl, ui_fader.winfo.y_gl + ui_fader.winfo.h_gl);
			draw_call_font(text_list.plus,ui_fader.winfo.x_gl + x_offset[3] + ui_fader.winfo.w_gl + ui_fader.winfo_digit.w_gl, ui_fader.winfo.y_gl + ui_fader.winfo.h_gl);
			draw_call_font(text_list.minus,ui_fader.winfo.x_gl + x_offset[1] + ui_fader.winfo.w_gl + ui_fader.winfo_digit.w_gl, ui_fader.winfo.y_gl - ui_fader.winfo.h_gl);
			draw_call_font(text_list.minus,ui_fader.winfo.x_gl + x_offset[3] + ui_fader.winfo.w_gl + ui_fader.winfo_digit.w_gl, ui_fader.winfo.y_gl - ui_fader.winfo.h_gl);
			
			// add brush ui 	-/-/-/-/-/-/-/-/
			draw_winfo(ui_add_brush.winfo, mouseover_state == CS_ADDBRUSH);
			draw_winfo(ui_add_brush.winfo_slider, mouseover_state == CS_ADDBRUSHSLIDER, ui_add_brush.brush_size_gl + ui_add_brush.winfo_slider.w_gl);
			draw_winfo(ui_add_brush.winfo_slider, mouseover_state == CS_ADDBRUSHSLIDER, (ui_add_brush.brush_size_gl + ui_add_brush.winfo_slider.w_gl) * -1.0f);
			draw_call_polygon( 0.0f,0.0f,0.0f,1.0f, ui_add_brush.brush_size, ui_add_brush.brush_size,
				ui_add_brush.winfo_slider.x_gl, ui_add_brush.winfo_slider.y_gl, &identity[0], array_buffer.circle, GL_TRIANGLE_FAN, num_data/4); 
			draw_call_font(text_list.add_brush,ui_add_brush.winfo.x_gl, ui_add_brush.winfo.y_gl);

			// color wheel ui 	-/-/-/-/-/-/-/-/
			// bind stuff
			glUseProgram(colorwheel_shader_SMOOTH.id);
			glBindBuffer(GL_ARRAY_BUFFER, array_buffer.circle);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, array_buffer.circle_spectrum);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

			// draw colored circle first
			glUniform2f(colorwheel_shader_SMOOTH.offset, ui_colorwheel.winfo.x_gl, ui_colorwheel.winfo.y_gl);
			glUniform2f(colorwheel_shader_SMOOTH.scale, ui_colorwheel.winfo.w_pixels + ui_colorwheel.thickness_pixels /*pixels*/,ui_colorwheel.winfo.h_pixels + ui_colorwheel.thickness_pixels /*pixels*/);
			glUniformMatrix4fv(colorwheel_shader_SMOOTH.rotation, 1, false, &identity[0]);
	        glDrawArrays(GL_TRIANGLE_FAN, 0, num_data/4);

			// change color
	        glBindBuffer(GL_ARRAY_BUFFER, array_buffer.circle_bg_color);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

			// draw inner white circle
	    	glUniform2f(colorwheel_shader_SMOOTH.scale, ui_colorwheel.winfo.w_pixels, ui_colorwheel.winfo.h_pixels);
			glDrawArrays(GL_TRIANGLE_FAN, 0, num_data/4);

	    	// draw inner triangle
	    	// change to smooth shader 
	    	// bind stuff
		    glBindBuffer(GL_ARRAY_BUFFER, array_buffer.triangle);
		    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
		    glBindBuffer(GL_ARRAY_BUFFER, array_buffer.triangle_colors);
		    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		    if (PICK_COLOR_FROM_COLOR_CIRCLE)
			{
				f4 color_from_color_circle[3] = {0.0f,0.0f,0.0f};
				s4 pixel_x = sdl_cursor_x;
			    s4 pixel_y = (sdl_cursor_y * -1) + user.screen_height; // flip y coordinate
			    f4 dx = msp.x - ui_colorwheel.winfo.x_gl;
			    f4 dy = msp.y - ui_colorwheel.winfo.y_gl;
			    remove_aspect_ratio(dx,dy);

			    f4 cat = atan2(dy,dx);
			    cat *= -M_RADTODEG32; 
			    cat += 360.0f; 
			    HSVtoRGB(color_from_color_circle[0],color_from_color_circle[1],color_from_color_circle[2],cat,1.0,1.0); 
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(f4)*3, &color_from_color_circle[0]);
			}

	        glUniform2f(colorwheel_shader_SMOOTH.offset, ui_colorwheel.winfo.x_gl, ui_colorwheel.winfo.y_gl);
	        glUniform2f(colorwheel_shader_SMOOTH.scale, ui_colorwheel.winfo.w_pixels, ui_colorwheel.winfo.h_pixels);
	        glUniformMatrix4fv(colorwheel_shader_SMOOTH.rotation, 1, false, &ui_colorwheel.rotation_matrix[0]);
	        glDrawArrays(GL_TRIANGLES, 0, 3);
	        // COLOR WHEEL RENDER -----------------------------------------------------------

	        // Color picker
			if (PICK_COLOR_FROM_COLOR_CIRCLE || PICK_COLOR_FROM_COLOR_TRIANGLE)
			{
				// TODO: check the position accuracy of this
			    s4 pixel_x = (ui_colorwheel.winfo.x_gl + ui_colorwheel.selected_pos.x * ui_colorwheel.winfo.w_gl) * user.screen_width/2.0f;
			    s4 pixel_y = (ui_colorwheel.winfo.y_gl + ui_colorwheel.selected_pos.y * ui_colorwheel.winfo.h_gl) * user.screen_height/2.0f;
				glReadPixels(pixel_x, pixel_y, 1, 1, GL_RGB, GL_FLOAT, &selected_color[0]);
			}

			// draw cursor texture on color wheel
			glUseProgram(texture_shader.id); 
			glBindTexture(GL_TEXTURE_2D, texture_list.cursor_white);
			glBindBuffer(GL_ARRAY_BUFFER, array_buffer.polygon);
				glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glBindBuffer(GL_ARRAY_BUFFER, array_buffer.polygon_uv);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glUniform1i(texture_shader.sampler2d, 0); // set texture unit 0
			glUniform2f(texture_shader.scale, color_cursor_pixels, color_cursor_pixels);
			glUniform2f(texture_shader.offset,
						ui_colorwheel.winfo.x_gl + ui_colorwheel.selected_pos.x * ui_colorwheel.winfo.w_gl,
						ui_colorwheel.winfo.y_gl + ui_colorwheel.selected_pos.y * ui_colorwheel.winfo.h_gl);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
}