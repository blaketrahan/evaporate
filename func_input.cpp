//Event handler
SDL_Event e;
b4 space_is_pressed = false;

auto toggle_pause = [] (EvaporateUserSettings* user, b4 color_brush_menu, b4 ctrl_state = false, b4 prev_pal_state = false)
{
	if (!ctrl_state && user->is_pause_locked)
	{
		// stay pause-locked  
		return;
	}

	if (ctrl_state && !user->is_pause_locked)
	{
		// turn on pause-lock
		user->is_paused = true;
		user->is_pause_locked = true; 
		return;
	}

	if (ctrl_state && user->is_pause_locked && !color_brush_menu)
	{
		// turn off pause-lock and pause
		user->is_paused = false;
		user->is_pause_locked = false; 
		return;
	}

	if (ctrl_state && user->is_pause_locked && color_brush_menu)
	{
		// turn off pause-lock but keep pause
		user->is_paused = true;
		user->is_pause_locked = false; 
		return;
	}

	if (color_brush_menu && user->is_paused)
	{ 
		return;
	}

	// TODO: turn is_paused into an enum
	// Show a different current-state-button for each state
	// 0 - unpaused				no button
	// 1 - paused by player		opaque paused button (so that palette doesnt turn it off)
	// 2 - paused by palette 	pulsing paused button
	// 3 - pause locked 		locked icon
	if (user->is_paused && !user->is_pause_locked && !color_brush_menu)
	{
		user->is_paused = false;
		user->is_pause_locked = false; 
		return;
	}

	user->is_paused = !user->is_paused; 
};

//Handle events on queue
while( SDL_PollEvent( &e ) != 0 )
{
	#ifdef IS_WINDOWS_BUILD
	if (e.type == SDL_SYSWMEVENT)
	{
		SDL_SysWMEvent system_event = e.syswm;
		if (system_event.msg->subsystem == SDL_SYSWM_WINDOWS)
		{
            EasyTabResult er = EasyTab_HandleEvent(system_event.msg->msg.win.hwnd,
				system_event.msg->msg.win.msg,
				system_event.msg->msg.win.lParam,
				system_event.msg->msg.win.wParam);
            if (er == EASYTAB_OK)
            {
            	
        		easytab_history.pts[easytab_history.last_pt].x = EasyTab->PosX;
        		easytab_history.pts[easytab_history.last_pt].y = EasyTab->PosY;
        		easytab_history.pts[easytab_history.last_pt].time = time_physics_curr;
        		easytab_history.last_pt++;
        		if (easytab_history.last_pt >= 64)
        		{
        			easytab_history.last_pt = 0;
        		}
            	
            	continue;
            }
        } 
	}
	#endif
	
	if ( e.type == SDL_MOUSEBUTTONDOWN )
	{
		user.dragging = true;
		user.initial_click = true;
	}
	else if ( e.type == SDL_MOUSEBUTTONUP )
	{
		// TODO: could possibly click twice before render loop
		// causing a click to be missed
		// but i don't think this will ever be a problem
		user.dragging = false; 
		user.has_clicked = true; 
	}
	else if( e.type == SDL_KEYDOWN )
    {
        switch( e.key.keysym.sym )
        {
        	case SDLK_TAB:
        	{
        		f4 prev_pal_state = ui_menu.winfo.visible;
        		ui_menu.winfo.visible = !ui_menu.winfo.visible;
        		toggle_pause(&user, ui_menu.winfo.visible, false, prev_pal_state);
        	} break;
        	case SDLK_RCTRL:
        	case SDLK_LCTRL:
        	{
        		user.ctrl_is_pressed = true;
        	} break;
        	case SDLK_SPACE:
        	{
        		// TODO: if (!user.menu_active)
        		space_is_pressed = true;
        	} break;
        	case SDLK_RETURN:
        	{
        		USER_SAVE_IMAGE_FILE = true;
        	} break;
        	case SDLK_ESCAPE:
        	{
        		QUIT_APP = true;
        	} break;
        	default: {} break;
        }
    }
    else if( e.type == SDL_KEYUP )
    {
        switch( e.key.keysym.sym )
        { 
        	case SDLK_RCTRL:
        	case SDLK_LCTRL:
        	{
        		user.ctrl_is_pressed = false;
        	} break; 
        	default: {} break;
        }
    } 
    else if ( e.type == SDL_WINDOWEVENT )
    {
    	switch (e.window.event)
    	{
    		case SDL_WINDOWEVENT_SIZE_CHANGED:
    		{ 
    			user.screen_width = e.window.data1;
            	user.screen_height = e.window.data2; 
				calculate_aspect_ratio();
            	update_entire_interface(); 
    		} break;
    	}
    }
    else if( e.type == SDL_QUIT )
	{
		QUIT_APP = true;
	}
}  

if (space_is_pressed)
{
	toggle_pause(&user, ui_menu.winfo.visible, user.ctrl_is_pressed);
}