struct EUIWINDOW
{ 
	s4 w_pixels;
	s4 h_pixels;
	f4 x_gl;
	f4 y_gl;
	f4 w_gl;
	f4 h_gl;
	f4 colors[4];
	f4 colors_ON[4];
	f4 visible;
};
struct EUIBRUSHES
{
	b4 is_set[4];
	f4 brush_sizes[4];
	EUIWINDOW winfo_delete;
	EUIWINDOW winfo;
};
struct EUIPAUSE
{
	EUIWINDOW winfo;
	//char text[6];
};

struct EUIFADER
{  
	s4 sec[2];
	s4 min[2];
	f4pt plus[2]; 
	f4pt minus[2];
	EUIWINDOW winfo;
	EUIWINDOW winfo_digit;
};

struct EUICOLORWHEEL
{
	u8 hue; // 0 - 255
	u8 value; // 0 - 255
	u8 saturation; // 0 - 255 
	f4 thickness_pixels;
	f4 rotation_matrix[16];
	f4 radians;
	f4pt selected_pos;
	EUIWINDOW winfo;
};

struct EUIADDBRUSH
{ 
	s4 brush_size;
	f4 brush_size_gl;
	EUIWINDOW winfo_slider;
	EUIWINDOW winfo;
};

struct EUIMENU
{
	f4pt anchors[4];
	f4pt midpoint;
	s4 desired_width;
	s4 desired_height;
	b4 too_small;
	EUIWINDOW winfo;
};

auto update_palette_position = [=] (EUIWINDOW &ui)
{
	ui.w_gl = (f4)ui.w_pixels/(f4)user.screen_width;
	ui.h_gl = (f4)ui.h_pixels/(f4)user.screen_height;
	ui.x_gl = 2.0f - ui.w_gl;
	ui.y_gl = 2.0f - ui.h_gl;
};

auto update_color_wheel_position = [=] (EUICOLORWHEEL &ui, EUIMENU &menu)
{
	ui.winfo.w_gl = (f4)ui.winfo.w_pixels/(f4)user.screen_width;
	ui.winfo.h_gl = (f4)ui.winfo.h_pixels/(f4)user.screen_height;
	// origin is at center of triangle
	ui.winfo.x_gl = menu.anchors[0].x;//1.0f;
	ui.winfo.y_gl = menu.anchors[0].y;//1.0f;  
};

auto update_brushes_position = [=] (EUIBRUSHES &ui)
{		
	ui.winfo.w_gl = (f4)ui.winfo.w_pixels/(f4)user.screen_width;
	ui.winfo.h_gl = (f4)ui.winfo.h_pixels/(f4)user.screen_height;
	ui.winfo.x_gl = 2.0f - ui.winfo.w_gl;
	ui.winfo.y_gl = 0.0f + ui.winfo.h_gl;

	ui.winfo_delete.w_gl = (f4)ui.winfo_delete.w_pixels/(f4)user.screen_width;
	ui.winfo_delete.h_gl = (f4)ui.winfo_delete.h_pixels/(f4)user.screen_height;
	ui.winfo_delete.x_gl = ui.winfo.x_gl - (ui.winfo.w_gl) + ui.winfo_delete.w_gl;
	ui.winfo_delete.y_gl = ui.winfo.y_gl;
};

auto update_paused_position = [=] (EUIWINDOW &ui, EUIWINDOW &palette)
{	
	ui.w_pixels = palette.w_pixels;	
	ui.w_gl = palette.w_gl;
	ui.h_gl = (f4)ui.h_pixels/(f4)user.screen_height;
	ui.x_gl = palette.x_gl;
	ui.y_gl = palette.y_gl - palette.h_gl - ui.h_gl - 0.01f;
};

auto update_add_brush_position = [=] (EUIADDBRUSH &ui, EUIMENU &menu)
{
	ui.winfo.w_gl = (f4)ui.winfo.w_pixels/(f4)user.screen_width;
	ui.winfo.h_gl = (f4)ui.winfo.h_pixels/(f4)user.screen_height;

	ui.winfo.x_gl = menu.anchors[2].x;
	ui.winfo.y_gl = menu.anchors[2].y - 200.0f/(f4)user.screen_height;

	ui.winfo_slider.w_gl = (f4)ui.winfo_slider.w_pixels/(f4)user.screen_width;
	ui.winfo_slider.h_gl = (f4)ui.winfo_slider.h_pixels/(f4)user.screen_height; 

	ui.winfo_slider.x_gl = menu.anchors[2].x;
	ui.winfo_slider.y_gl = menu.anchors[2].y;

	ui.brush_size_gl = (f4)ui.brush_size/(f4)user.screen_width;
};

auto update_menu_position = [=] (EUIMENU &ui, EUIWINDOW &palette)
{
	// TODO: fix this when window is too small
	ui.desired_width = 500;
	ui.desired_height = 800;

	s4 padding = 60;//pixels

	s4 space_w = user.screen_width - padding - palette.w_pixels;

	if (space_w < ui.desired_width)
	{
		ui.winfo.w_pixels = space_w;
		ui.winfo.w_gl = (f4)ui.winfo.w_pixels/(f4)user.screen_width;
	}
	else 
	{
		ui.winfo.w_pixels = ui.desired_width;
		ui.winfo.w_gl = (f4)ui.winfo.w_pixels/(f4)user.screen_width;	
	}

	s4 space_h = user.screen_height - padding;
	if (space_h < ui.desired_height)
	{
		ui.winfo.h_pixels = space_h;
		ui.winfo.h_gl = (f4)ui.winfo.h_pixels/(f4)user.screen_height;
	}
	else
	{
		ui.winfo.h_pixels = ui.desired_height;
		ui.winfo.h_gl = (f4)ui.winfo.h_pixels/(f4)user.screen_height;	
	}

	// TODO: hardcoded values
	if (space_w < 340 || space_h < 570)
	{
		ui.too_small = true;
	} else
	{
		ui.too_small = false;
	}

	ui.winfo.x_gl = palette.x_gl - palette.w_gl - ui.winfo.w_gl - (f4)padding/(f4)user.screen_width;
	ui.winfo.y_gl = 1.0f;

	ui.anchors[0].x = ui.winfo.x_gl;
	ui.anchors[0].y = ui.winfo.y_gl + ui.winfo.h_gl * 0.4;

	ui.midpoint.x = ui.winfo.x_gl;
	ui.midpoint.y = ui.winfo.y_gl - ui.winfo.h_gl * 0.2;

	ui.anchors[1].x = ui.winfo.x_gl + ui.winfo.w_gl * 0.5;
	ui.anchors[1].y = ui.winfo.y_gl - ui.winfo.h_gl * 0.6;

	ui.anchors[2].x = ui.winfo.x_gl - ui.winfo.w_gl * 0.5;
	ui.anchors[2].y = ui.winfo.y_gl - ui.winfo.h_gl * 0.5;

	// TODO: unused currently
	ui.anchors[3].x = ui.winfo.x_gl - ui.winfo.w_gl;
	ui.anchors[3].y = ui.winfo.y_gl - ui.winfo.h_gl * 0.5;
};

auto update_fader_position = [=] (EUIFADER &ui, EUIMENU &menu)
{
	ui.winfo.w_gl = (f4)ui.winfo.w_pixels/(f4)user.screen_width;
	ui.winfo.h_gl = (f4)ui.winfo.h_pixels/(f4)user.screen_height;
	ui.winfo.x_gl = menu.anchors[1].x;//1.0f;
	ui.winfo.y_gl = menu.anchors[1].y;//1.0f;  

	ui.winfo_digit.w_gl = (f4)ui.winfo_digit.w_pixels/(f4)user.screen_width;
	ui.winfo_digit.h_gl = (f4)ui.winfo_digit.h_pixels/(f4)user.screen_height;
	ui.winfo_digit.x_gl = menu.anchors[1].x;//1.0f;
	ui.winfo_digit.y_gl = menu.anchors[1].y;//1.0f;

	ui.plus[0].x = ui.winfo.x_gl + ui.winfo_digit.w_gl * -2.0f + (ui.winfo.w_gl * -1.0f) + ui.winfo.w_gl + ui.winfo_digit.w_gl;
	ui.plus[0].y = ui.winfo.y_gl + ui.winfo.h_gl;

	ui.plus[1].x = ui.winfo.x_gl + (ui.winfo_digit.w_gl * 2.0f) + (ui.winfo.w_gl * 2.0f) + ui.winfo.w_gl + ui.winfo_digit.w_gl;
	ui.plus[1].y = ui.winfo.y_gl + ui.winfo.h_gl;

	ui.minus[0].x = ui.plus[0].x;
	ui.minus[0].y = ui.winfo.y_gl - ui.winfo.h_gl;

	ui.minus[1].x = ui.plus[1].x;
	ui.minus[1].y = ui.winfo.y_gl - ui.winfo.h_gl;
};

EUIWINDOW ui_palette =  {PALETTE_WIDTH,PALETTE_HEIGHT,0,0,0,0, {1.0,1.0,1.0,1.0}, {1.0,1.0,1.0,1.0}, true };

const s4 EUIW = 128;

EUIBRUSHES ui_brush_list =
{
	{1,0,0,0},
	{8,12,4,2},
	{48,48,0,0,0,0, {0.8,0.5,0.45,1.0}, {0.95,0.4,0.35,1.0}, true },
	{EUIW,48,0,0,0,0, {0.5,0.5,0.5,1.0}, {0.6,0.6,0.6,1.0}, true }
}; 

EUIPAUSE ui_pause = {EUIW,32,0,0,0,0, {0.8,0.2,0.2,1.0}, {0.2,0.2,0.8,1.0}, false };

EUIMENU ui_menu =
{
	{{0,0}, {0,0}, {0,0}, {0,0}},
	{0,0},
	0,0,
	false,
	{500,500, 0,0,0,0, {user.ui_color[0], user.ui_color[1], user.ui_color[2], 1.0}, {1.0f, user.ui_color[1], user.ui_color[1], 1.0}, false } // winfo
};

EUIADDBRUSH ui_add_brush =
{ 
	10, // brush size
	0,	// brush size gl
	{12,80,0,0,0,0, {0.2,0.2,0.2,1.0}, {0.3,0.3,0.3,1.0}, true },
	{120,48,0,0,0,0, {0.2,0.2,0.2,1.0}, {0.3,0.3,0.3,1.0}, true }
};

EUICOLORWHEEL ui_colorwheel = 
{
	0, 	// hue
	0, 	// value
	0, 	// saturation
	80,	// color band thickness
	{1,0,0,0, // rotation matrix
	 0,1,0,0,
	 0,0,1,0,
	 0,0,0,1},
 	0.0f, 	// radians
 	{0,0}, 	// selected pos
	{256,256, 0,0,0,0, {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, true } // winfo
};

EUIFADER ui_fader =
{
	{3,0},
	{0,0},
	{{0,0},{0,0}},
	{{0,0},{0,0}},
	{20,20, 0,0,0,0, {0.2,0.2,0.2,1.0}, {0.3,0.3,0.3,1.0}, true }, // winfo
	{20,40, 0,0,0,0, {1.0,1.0,1.0,1.0}, {1.0,1.0,1.0,1.0}, true } // winfo digit
};

auto update_entire_interface = [&] ()
{ 
	update_palette_position(ui_palette);
	update_brushes_position(ui_brush_list);
	update_paused_position(ui_pause.winfo, ui_palette);
	update_menu_position(ui_menu, ui_palette);
	update_add_brush_position(ui_add_brush, ui_menu);
	update_color_wheel_position(ui_colorwheel,ui_menu);
	update_fader_position(ui_fader, ui_menu);
};

update_entire_interface();