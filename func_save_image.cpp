// TODO: Do this in small batches rather than all during one render call.
// Because it halts the program.
// First pass: glreadpixels into array
// Second pass: flip image
// Third pass: stbi_write_png
// on each save, alloc memory for it and have it take care of itself
// in case the player tries to save again while it's still between passes

USER_SAVE_IMAGE_FILE = false;

// TODO: research

glBindFramebuffer(GL_FRAMEBUFFER,FBO);
glReadBuffer(GL_COLOR_ATTACHMENT0);

// std::cout << "Screen Dimensions: " << user.screen_width << ", " << user.screen_height << std::endl;

s4 file_w = user.screen_width;
s4 file_h = user.screen_height;

glPixelStorei(GL_PACK_ALIGNMENT,1);
glReadPixels( 0, 0, file_w, file_h, GL_RGB, GL_UNSIGNED_BYTE, &saved_png[0]);

// std::cout << "BEGIN PRINT:" << std::endl;

// s4 el = 0;
// for (s4 i = 0; i < file_w * file_h * 3; i+=3)
// {
// 	std::cout << "{" << (s4)saved_png[i] << "," << (s4)saved_png[i+1] << "," << (s4)saved_png[i+2] << "} ";
// 	el++;
	
// 	if (el % file_w == 0) std::cout << std::endl;
// }
// std::cout << "END PRINT:" << std::endl;

s4 NC = 3; // number components (R,G,B)
s4 stride_in_bytes = sizeof(u1) * (file_w * NC);
s4 stride = (file_w * NC);
s4 data_size = NC * (file_w * file_h);
s4 k = data_size;

for (s4 i = 0; i < data_size; i += stride)
{
	k -= stride;
	memcpy(&flipped_png[k], &saved_png[i], stride_in_bytes);
}

// el = 0;
// for (s4 i = 0; i < file_w * file_h * 3; i+=3)
// {
// 	std::cout << "{" << (s4)flipped_png[i] << "," << (s4)flipped_png[i+1] << "," << (s4)flipped_png[i+2] << "} ";
// 	el++;
	
// 	if (el % file_w == 0) std::cout << std::endl;
// }
// std::cout << "END PRINT:" << std::endl;

// get time
time_t rawtime;
struct tm* timeinfo;
time(&rawtime);
timeinfo = localtime (&rawtime);

// create filename
std::ostringstream stream;
stream 	<< "./images/image_" << timeinfo->tm_mon << "_" 
		<< timeinfo->tm_mday << "_" << timeinfo->tm_hour
		<< timeinfo->tm_min << timeinfo->tm_sec << ".png";
std::string file_name = stream.str();

// write
int error_check = stbi_write_png(file_name.c_str(), file_w, file_h, NC, &flipped_png[0], stride_in_bytes);

#ifdef DEBUG_BUILD
	std::cout << "stbi_write_png: error check: " << error_check << std::endl;
#endif