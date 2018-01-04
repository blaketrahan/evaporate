GLuint my_create_texture(s4 sw, s4 sh, b4 alpha)
{
	// TODO: figure texture size
	unsigned int width = sw;
	unsigned int height = sh;

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);
 
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
 	
	// TODO: are these misplaced?  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Give the image to OpenGL
	if (alpha)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);

	return textureID;
}

GLuint create_font_texture(SDL_Surface* sdl_surface)
{
	Uint8 colors = sdl_surface->format->BytesPerPixel;
    GLenum texture_format;
	if (colors == 4) {   // alpha
	    if (sdl_surface->format->Rmask == 0x000000ff)
	        texture_format = GL_RGBA;
	    else
	        texture_format = GL_BGRA;
	} else {             // no alpha
	    if (sdl_surface->format->Rmask == 0x000000ff)
	        texture_format = GL_RGB;
	    else
	        texture_format = GL_BGR;
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, colors, sdl_surface->w, sdl_surface->h, 0, texture_format, GL_UNSIGNED_BYTE, sdl_surface->pixels);

	return textureID;
}

GLuint CreateProgram(const std::vector<GLuint> &shaderList)
{
	GLuint program = glCreateProgram();
	
	for(size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glAttachShader(program, shaderList[iLoop]);
	
	glLinkProgram(program);
	
	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}
	
	for(size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glDetachShader(program, shaderList[iLoop]);

	return program;
}

GLuint CreateShader(GLenum eShaderType, const std::string &strShaderFile)
{
	GLuint shader = glCreateShader(eShaderType);
	const char *strFileData = strShaderFile.c_str();
	glShaderSource(shader, 1, &strFileData, NULL);
	
	glCompileShader(shader);
	
	// Error checking ---
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
		
		const char *strShaderType = NULL;
		switch(eShaderType)
		{
			case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
			case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
			case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}
		
		fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
		delete[] strInfoLog;
	}

	return shader;
}

void create_buffer(GLuint &array_buffer, const f4* verts, s4 size)
{
	glGenBuffers(1, &array_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
	glBufferData(GL_ARRAY_BUFFER, size, verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
}

GLuint create_shader_program(const GLchar *vs_source, const GLchar *fs_source)
{
	std::vector<GLuint> shaderList;
	shaderList.push_back(CreateShader(GL_VERTEX_SHADER, vs_source));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, fs_source));
	GLuint id = CreateProgram(shaderList);
	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
	return id;
}