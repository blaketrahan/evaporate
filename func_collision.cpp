// collision detection -------------------------------------------
// TODO: make these more in tune with the cursor shape
f4pt useful_temporary_vector = {0.0f, 0.0f};

if (user.dragging && user.initial_click == false)
{
	// keep initial mouse state when dragging
	mouseover_state = user.initial_click_mouseover_state;
}
else if (ui_menu.winfo.visible) // menu is open
{
	if (msp.x >= ui_palette.x_gl - ui_palette.w_gl) // On palette side
	{
		if (msp.y >= ui_palette.y_gl - ui_palette.h_gl)
		{
			mouseover_state = CS_PALETTE_EDITING;
		}
		else
		{
			b4 is_deleting_brush = false;
			for (s4 i = 0; i < MAX_NUM_BRUSH; i++)
			{
				if (!ui_brush_list.is_set[i]) break;

				if (msp.y < ui_brush_list.winfo_delete.y_gl + ui_brush_list.winfo_delete.h_gl + (i * ui_brush_list.winfo_delete.h_gl * 2.0f) &&
					msp.x > ui_brush_list.winfo_delete.x_gl - ui_brush_list.winfo_delete.w_gl &&
					msp.x <= ui_brush_list.winfo_delete.x_gl + ui_brush_list.winfo_delete.w_gl )
				{
					mouseover_state = CS_BUTTON_DELETE0 + i;
					is_deleting_brush = true;
					break;
				}
			}
			if (!is_deleting_brush)
			{
				for (s4 i = 0; i < MAX_NUM_BRUSH; i++)
				{
					if (!ui_brush_list.is_set[i]) break;

					if (msp.y < ui_brush_list.winfo.y_gl + ui_brush_list.winfo.h_gl + (i * ui_brush_list.winfo.h_gl * 2.0f))
					{
						mouseover_state = CS_BUTTON0 + i;
						break;
					}
				}
			}
		}
	} else // on menu window side
	{
		if (msp.y < ui_menu.midpoint.y) // LOWER
		{	
			if (msp.x < ui_menu.winfo.x_gl ) // LOWER LEFT SIDE
			{
				if (msp.x > ui_add_brush.winfo.x_gl - ui_add_brush.winfo.w_gl && msp.x <= ui_add_brush.winfo.x_gl + ui_add_brush.winfo.w_gl &&
					msp.y > ui_add_brush.winfo.y_gl - ui_add_brush.winfo.h_gl && msp.y <= ui_add_brush.winfo.y_gl + ui_add_brush.winfo.h_gl)
				{
					mouseover_state = CS_ADDBRUSH;
				}
				else if (msp.x > ui_add_brush.winfo_slider.x_gl - (ui_add_brush.winfo_slider.w_gl*2.0f + ui_add_brush.brush_size_gl) &&
						 msp.x <= ui_add_brush.winfo_slider.x_gl + (ui_add_brush.winfo_slider.w_gl*2.0f + ui_add_brush.brush_size_gl) &&
						 msp.y > ui_add_brush.winfo_slider.y_gl - ui_add_brush.winfo_slider.h_gl && msp.y <= ui_add_brush.winfo_slider.y_gl + ui_add_brush.winfo_slider.h_gl)
				{
					mouseover_state = CS_ADDBRUSHSLIDER;
				}
			}
			else // LOWER RIGHT SIDE
			{
				if (msp.x > ui_fader.plus[0].x - ui_fader.winfo.w_gl &&
					msp.x <= ui_fader.plus[0].x + ui_fader.winfo.w_gl &&
					msp.y <= ui_fader.plus[0].y + ui_fader.winfo.h_gl &&
					msp.y > ui_fader.plus[0].y - ui_fader.winfo.h_gl)
				{
					mouseover_state = CS_FADER_MIN_UP;
				}
				else if (msp.x > ui_fader.plus[1].x - ui_fader.winfo.w_gl &&
					msp.x <= ui_fader.plus[1].x + ui_fader.winfo.w_gl &&
					msp.y <= ui_fader.plus[1].y + ui_fader.winfo.h_gl &&
					msp.y > ui_fader.plus[1].y - ui_fader.winfo.h_gl)
				{
					mouseover_state = CS_FADER_SEC_UP;
				}
				else if (msp.x > ui_fader.minus[0].x - ui_fader.winfo.w_gl &&
					msp.x <= ui_fader.minus[0].x + ui_fader.winfo.w_gl &&
					msp.y <= ui_fader.minus[0].y + ui_fader.winfo.h_gl &&
					msp.y > ui_fader.minus[0].y - ui_fader.winfo.h_gl)
				{
					mouseover_state = CS_FADER_MIN_DOWN;
				}
				else if (msp.x > ui_fader.minus[1].x - ui_fader.winfo.w_gl &&
					msp.x <= ui_fader.minus[1].x + ui_fader.winfo.w_gl &&
					msp.y <= ui_fader.minus[1].y + ui_fader.winfo.h_gl &&
					msp.y > ui_fader.minus[1].y - ui_fader.winfo.h_gl)
				{
					mouseover_state = CS_FADER_SEC_DOWN;
				}
			}
		}
		else // UPPER SIDE
		{
			f4 rel_mx = fabs(msp.x - ui_colorwheel.winfo.x_gl);
			f4 rel_my = fabs(msp.y - ui_colorwheel.winfo.y_gl);
			f4 outerx = ui_colorwheel.winfo.w_gl + (ui_colorwheel.thickness_pixels/user.screen_width);
			f4 innerx = ui_colorwheel.winfo.w_gl;
			f4 outery = ui_colorwheel.winfo.h_gl + (ui_colorwheel.thickness_pixels/user.screen_height);
			f4 innery = ui_colorwheel.winfo.h_gl;

			if (rel_mx < outerx && rel_my < outery) 
			{
				// if within larger circle
				// but not within inner circle 
				f4 dx = rel_mx/aspect_x;
				f4 dy = rel_my/aspect_y;
				f4 dist = dx * dx + dy * dy;
				dist = sqrt(dist);
				
				if (dist < outerx && dist >= innerx) // if inside circle band
				{
					mouseover_state = CS_COLORPICKER_CIRCLE; 
				}
				else if (dist <= innerx) // if inside triangle
				{  
					mouseover_state = CS_COLORPICKER_TRIANGLE; 
				} 
			}
			
		}
	}
}
// normal drawing on canvas
else if (msp.x < ui_palette.x_gl - ui_palette.w_gl)
{
	mouseover_state = CS_DRAWING;
}
else // palette and brush picking
{
	if (msp.y >= ui_palette.y_gl - ui_palette.h_gl)
	{
		mouseover_state = CS_PALETTE_PICKING; 
	}
	else
	{
		for (s4 i = 0; i < MAX_NUM_BRUSH; i++)
		{
			if (!ui_brush_list.is_set[i]) break;

			if (msp.y < ui_brush_list.winfo.y_gl + ui_brush_list.winfo.h_gl + (i * ui_brush_list.winfo.h_gl * 2.0f))
			{
				mouseover_state = CS_BUTTON0 + i;
				break;
			}
		}
	}
}
// collision detection -------------------------------------------

// collision response ON MOUSEOVER
if (mouseover_state == CS_COLORPICKER_CIRCLE)
{
	useful_temporary_vector.x = (msp.x - ui_colorwheel.winfo.x_gl) / aspect_x;
	useful_temporary_vector.y = (msp.y - ui_colorwheel.winfo.y_gl) / aspect_y;
}
else if (mouseover_state == CS_COLORPICKER_TRIANGLE)
{
	f4 x1 = (ui_colorwheel.rotation_matrix[0] * (tri_verts[0] * 0.99)) + (ui_colorwheel.rotation_matrix[1] * (tri_verts[1] * 0.99));
	f4 y1 = (ui_colorwheel.rotation_matrix[4] * (tri_verts[0] * 0.99)) + (ui_colorwheel.rotation_matrix[5] * (tri_verts[1] * 0.99));
	f4 x2 = (ui_colorwheel.rotation_matrix[0] * (tri_verts[4] * 0.99)) + (ui_colorwheel.rotation_matrix[1] * (tri_verts[5] * 0.99));
	f4 y2 = (ui_colorwheel.rotation_matrix[4] * (tri_verts[4] * 0.99)) + (ui_colorwheel.rotation_matrix[5] * (tri_verts[5] * 0.99));
	f4 x3 = (ui_colorwheel.rotation_matrix[0] * (tri_verts[8] * 0.99)) + (ui_colorwheel.rotation_matrix[1] * (tri_verts[9] * 0.99));
	f4 y3 = (ui_colorwheel.rotation_matrix[4] * (tri_verts[8] * 0.99)) + (ui_colorwheel.rotation_matrix[5] * (tri_verts[9] * 0.99));

	f4pt v1 = {ui_colorwheel.winfo.x_gl + x1 * ui_colorwheel.winfo.w_gl, 
			   ui_colorwheel.winfo.y_gl + y1 * ui_colorwheel.winfo.h_gl};
	f4pt v2 = {ui_colorwheel.winfo.x_gl + x2 * ui_colorwheel.winfo.w_gl,
			   ui_colorwheel.winfo.y_gl + y2 * ui_colorwheel.winfo.h_gl};
	f4pt v3 = {ui_colorwheel.winfo.x_gl + x3 * ui_colorwheel.winfo.w_gl,
			   ui_colorwheel.winfo.y_gl + y3 * ui_colorwheel.winfo.h_gl};

	// Test point is inside triangle
	auto sign = [] (f4pt p1, f4pt p2, f4pt p3) -> float
	{
	    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
	};

    bool b1, b2, b3;

    b1 = sign(msp, v1, v2) < 0.0f;
    b2 = sign(msp, v2, v3) < 0.0f;
    b3 = sign(msp, v3, v1) < 0.0f;

    if ((b1 == b2) && (b2 == b3))
	{
		// normalize it with the width of ui_colorwheel's size
		useful_temporary_vector.x = (msp.x - ui_colorwheel.winfo.x_gl)/ ui_colorwheel.winfo.w_gl;// / aspect_x; 
		useful_temporary_vector.y = (msp.y - ui_colorwheel.winfo.y_gl)/ ui_colorwheel.winfo.h_gl;// / aspect_y; 
	}
	else
	{
		useful_temporary_vector.x = ui_colorwheel.selected_pos.x;
		useful_temporary_vector.y = ui_colorwheel.selected_pos.y;
	}
}
 
// collision response ON USER CLICK/DRAGGING  -------------------------------------------
if (user.dragging || user.has_clicked)
{
	auto remove_brush = [&] (s4 k)
	{
		if (ui_brush_list.is_set[k])
		{
			ui_brush_list.is_set[k] = false;
			for (s4 i = k + 1; i < MAX_NUM_BRUSH; i++)
			{
				if (ui_brush_list.is_set[i])
				{
					ui_brush_list.is_set[i] = false;
					ui_brush_list.is_set[i-1] = true;
					ui_brush_list.brush_sizes[i-1] = ui_brush_list.brush_sizes[i];
				}
				else
				{
					break;
				}
			}
			user.num_brush -= 1;
		}
	};

	auto recalculate_fader = [&] ()
	{
		f4 number_sec = (ui_fader.sec[0] * 10) + ui_fader.sec[1];
		number_sec += ((ui_fader.min[0] * 10) + ui_fader.min[1]) * 60.0f;
		if (number_sec == 0.0f)
		{
			number_sec = 1.0f;
		} 
		user.alpha = (1.0f/number_sec) * ONE_SECOND_FADE;
	};

	if (user.initial_click)
	{
		user.initial_click_mouseover_state = mouseover_state;
		user.initial_click = false; 
	}

	if (mouseover_state == user.initial_click_mouseover_state)
	{ 
	  	switch(mouseover_state)
	  	{
			case CS_BUTTON0:
			case CS_BUTTON1:
			case CS_BUTTON2:
			case CS_BUTTON3:
			{
				if (user.has_clicked) {
					user.brush_radius = ui_brush_list.brush_sizes[mouseover_state - CS_BUTTON0];
				}
			} break; 
			case CS_BUTTON_DELETE0:
			case CS_BUTTON_DELETE1:
			case CS_BUTTON_DELETE2:
			case CS_BUTTON_DELETE3:
			{
				if (user.has_clicked)
				{
					remove_brush(mouseover_state - CS_BUTTON_DELETE0);
				}
			} break;
			case CS_COLORPICKER_CIRCLE:
			{
				PICK_COLOR_FROM_COLOR_CIRCLE = true;
				f4 cat = atan2(useful_temporary_vector.y, useful_temporary_vector.x);
				cat -= 90.0f * M_DEGTORAD32;
				f4 difference = cat - ui_colorwheel.radians;
				ui_colorwheel.radians = cat;
				// std::cout << cat * M_RADTODEG32 << std::endl;
				ui_colorwheel.rotation_matrix[0] = (f4) cos( cat );
				ui_colorwheel.rotation_matrix[1] = (f4)-sin( cat );
				ui_colorwheel.rotation_matrix[4] = (f4) sin( cat );
				ui_colorwheel.rotation_matrix[5] = (f4) cos( cat );

				f4 diff_matrix[4];
				diff_matrix[0] = (f4) cos( difference );
				diff_matrix[1] = (f4)-sin( difference );
				diff_matrix[2] = (f4) sin( difference );
				diff_matrix[3] = (f4) cos( difference );

				// rotate cw_cursor pos 
				f4 dx = ui_colorwheel.selected_pos.x;
				f4 dy = ui_colorwheel.selected_pos.y;
				// std::cout << aspect_x << " " << aspect_y << std::endl; 
				f4 dxb = (diff_matrix[0] * dx) + (diff_matrix[1] * dy);
				f4 dyb = (diff_matrix[2] * dx) + (diff_matrix[3] * dy); 

				ui_colorwheel.selected_pos.x = dxb;
				ui_colorwheel.selected_pos.y = dyb;
			} break;
			case CS_COLORPICKER_TRIANGLE:
			{
				ui_colorwheel.selected_pos.x = useful_temporary_vector.x;
				ui_colorwheel.selected_pos.y = useful_temporary_vector.y;
				PICK_COLOR_FROM_COLOR_TRIANGLE = true;
			} break;
			case CS_ADDBRUSH:
			{
				if (user.has_clicked && ui_add_brush.brush_size != 0.0f)
				{
					for (s4 i = 0; i < MAX_NUM_BRUSH; i++)
					{
						if (!ui_brush_list.is_set[i])
						{
							ui_brush_list.is_set[i] = true;
							ui_brush_list.brush_sizes[i] = ui_add_brush.brush_size;
							user.num_brush = i + 1;
							break;
						}
					}
				}
			} break;
			case CS_ADDBRUSHSLIDER:
			{
				if (msp.x >= ui_add_brush.winfo_slider.x_gl) {
					ui_add_brush.brush_size_gl = (msp.x - ui_add_brush.winfo_slider.x_gl) - ui_add_brush.winfo_slider.w_gl;
				} else {
					ui_add_brush.brush_size_gl = (ui_add_brush.winfo_slider.x_gl - msp.x) - ui_add_brush.winfo_slider.w_gl;
				}
				// std::cout << ui_add_brush.brush_size_gl << std::endl;
				if (ui_add_brush.brush_size_gl < 0.0f)
				{
					ui_add_brush.brush_size_gl = 0.0f; 
				}
				ui_add_brush.brush_size = (s4)(ui_add_brush.brush_size_gl * (f4)user.screen_width);
				if (ui_add_brush.brush_size > 148)
				{
					ui_add_brush.brush_size = 148;
					ui_add_brush.brush_size_gl = (f4)ui_add_brush.brush_size/(f4)user.screen_width;
				}
			} break;
			case CS_PALETTE_PICKING:
			{
				PICKUP_A_COLOR = true;
			} break;
			case CS_DRAWING:
			{ 
				COLLECT_MOUSE_PTS = true;
				DRAW_ON_CANVAS = true;
			} break;
			case CS_PALETTE_EDITING:
			{
				COLLECT_MOUSE_PTS = true;
				DRAW_ON_PALETTE = true;
			} break;
			case CS_FADER_SEC_UP:
			{
				if (user.has_clicked)
				{
					ui_fader.sec[1]++;
					if (ui_fader.sec[1] >= 10)
					{
						ui_fader.sec[0]++;
						ui_fader.sec[1] -= 10;
						if (ui_fader.sec[0] >= 6)
						{ 
							ui_fader.sec[0] -= 6; 
						}
					}
					recalculate_fader();
				}
			} break;
			case CS_FADER_SEC_DOWN:
			{
				if (user.has_clicked)
				{
					ui_fader.sec[1]--;
					if (ui_fader.sec[1] < 0)
					{
						ui_fader.sec[0]--;
						ui_fader.sec[1] += 10;
						if (ui_fader.sec[0] < 0)
						{ 
							ui_fader.sec[0] += 6; 
						}
					} 
					recalculate_fader();
				}
			} break;
			case CS_FADER_MIN_DOWN:
			{
				if (user.has_clicked)
				{
					ui_fader.min[1]--;
					if (ui_fader.min[1] < 0)
					{
						ui_fader.min[0]--;
						ui_fader.min[1] += 10;
						if (ui_fader.min[0] < 0)
						{ 
							ui_fader.min[0] += 6; 
						}
					}
					recalculate_fader();
				}
			} break;
			case CS_FADER_MIN_UP:
			{
				if (user.has_clicked)
				{
					ui_fader.min[1]++;
					if (ui_fader.min[1] >= 10)
					{
						ui_fader.min[0]++;
						ui_fader.min[1] -= 10;
						if (ui_fader.min[0] >= 6)
						{ 
							ui_fader.min[0] -= 6; 
						}
					}
					recalculate_fader();
				}
			} break;
	  	}
	}
	user.has_clicked = false; // reset
}
// collision response  -------------------------------------------