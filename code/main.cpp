
/*
	TPB-07 
	by The Pimp Brigade

	For Windows, but it should be relatively easy to port to Linux and OSX.

	I swear for once that I will:
	- Not overengineer a single bit
	- Don't give a fuck about how the code looks (you know you won't keep that promise 100%, right?)
	- Purely aim for result
	- We will use a small subset of OpenGL, so all should be fine
	- OpenGL handles don't seem to be have to released?

	So now it's clear why this is an ugly hack.
	I've tried doing it all right (TPB-06) and that cost me 3 months and got me a lousy 4th place.

	Shader source links (in my Shadertoy account):
	- Cracktro: https://www.shadertoy.com/view/Wl33WS
	- Caged balls: https://www.shadertoy.com/view/Wt3GWB
	- Trevor: https://www.shadertoy.com/view/Wtc3DB
	- Backup: https://www.shadertoy.com/view/tl3GWf
	- Always check for latest before using backups, esp. check different radial blur tweaks and assigned buffers

	Ravity's experiments:
	- Lens flare: https://www.shadertoy.com/view/3ttGDj?fbclid=IwAR0QaSRCmMIi9xW2XV8hkyXiK0-an-DL8oyl70VEeiTAfoXFc-P-MX09j2Q
	- Spikey thing: https://www.shadertoy.com/view/3td3WX?fbclid=IwAR0kq-CSe6BcKmqIA0cByyMNzLPqgREC1vDmWsnzdH32nT4zFB_6hZWndi4

	Links with potentially useful information:
	  + Cubemaps and OGL: https://learnopengl.com/Advanced-OpenGL/Cubemaps
	  + Author of tiny boilerplate I took: https://github.com/koute
*/

#include "main-header.h"
#include "text-loader.h"

typedef float t_mat4x4[16];

static inline void mat4x4_ortho(t_mat4x4 out, float left, float right, float bottom, float top, float znear, float zfar)
{
	#define T(a, b) (a * 4 + b)

	out[T(0,0)] = 2.0f / (right - left);
	out[T(0,1)] = 0.0f;
	out[T(0,2)] = 0.0f;
	out[T(0,3)] = 0.0f;

	out[T(1,1)] = 2.0f / (top - bottom);
	out[T(1,0)] = 0.0f;
	out[T(1,2)] = 0.0f;
	out[T(1,3)] = 0.0f;

	out[T(2,2)] = -2.0f / (zfar - znear);
	out[T(2,0)] = 0.0f;
	out[T(2,1)] = 0.0f;
	out[T(2,3)] = 0.0f;

	out[T(3,0)] = -(right + left) / (right - left);
	out[T(3,1)] = -(top + bottom) / (top - bottom);
	out[T(3,2)] = -(zfar + znear) / (zfar - znear);
	out[T(3,3)] = 1.0f;

	#undef T
}

static const char * vertex_shader =
	"#version 130\n"
	"in vec2 i_position;\n"
	"in vec4 i_color;\n"
	"out vec4 v_color;\n"
	"uniform mat4 u_projection_matrix;\n"
	"void main() {\n"
	"    v_color = i_color;\n"
	"    gl_Position = u_projection_matrix * vec4( i_position, 0.0, 1.0 );\n"
	"}\n";

static const char * fragment_shader =
	"#version 130\n"
	"in vec4 v_color;\n"
	"out vec4 o_color;\n"
	"void main() {\n"
	"    o_color = v_color;\n"
	"}\n";

typedef enum t_attrib_id
{
	attrib_position,
	attrib_color
} t_attrib_id;

// ---

#if 0

/*
	This was/is a little experiment initiated by Pieter v/d Meer, we've learned
	that labels are padded so that they, if so desired, fit within the block
	a branch predictor can use (16 bytes usually) to shove in the same bucket;
	the lower bits don't matter this way, so a loop address may very well be
	unaligned from that perspective.

	For more information: https://www.agner.org/optimize/microarchitecture.pdf, see section 3.8
*/

volatile uint64_t g_bullshit1;
volatile uint64_t g_bullshit2;

__declspec(noinline) void upbs2() { ++g_bullshit2; }

__declspec(noinline) void looptest()
{
	__debugbreak();
	// because Marco <3 UB
	for (int i = 0; i < 3; ++i)
	{
		++g_bullshit1;
		upbs2();
		__debugbreak();
		for (int j = 0; j < 4; ++j)
		{
			upbs2();			
		}
	}
}

#endif

// ---

int main(int argc, char * argv[])
{
//	looptest();
	
	printf("TPB-07, running off of a hacky OpenGL console app.\n");
	printf("Please see http://www.pouet.net to report bugs and such.\n");

#if defined(_DEBUG) && defined(WIN32_CRT_LEAK_CHECK)
	// Dump leak report at any possible exit.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | 
	_CRTDBG_LEAK_CHECK_DF);
	
	// Report all to debug pane.
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);

	if (-1 != WIN32_CRT_BREAK_ALLOC)
		_CrtSetBreakAlloc(WIN32_CRT_BREAK_ALLOC);
#endif

	int return_value = 0;

	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_Window *window = SDL_CreateWindow("TPB-07", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, kResX, kResY, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	// I have to declare these here so I get to use 'goto':

	GLint length;
	GLint status;
	bool demo_is_done = false;

	const GLfloat g_vertex_buffer_data[] = {
	/*  R, G, B, A, X, Y  */
		1, 0, 0, 1, 0, 0,
		0, 1, 0, 1, kResX, 0,
		0, 0, 1, 1, kResX, kResY,

		1, 0, 0, 1, 0, 0,
		0, 0, 1, 1, kResX, kResY,
		1, 1, 1, 1, 0, kResY
    };

	// OK -- with that out of the way, please continue :)

	GLenum err = glewInit();
	if (0 != err)
	{
		fprintf(stderr, "failed to initialize GLEW\n");	
		return_value = 1;
		goto quit;
	}

	GLuint vs, fs, program;

	vs = glCreateShader(GL_VERTEX_SHADER);
	fs = glCreateShader(GL_FRAGMENT_SHADER);

	length = (GLint) strlen(vertex_shader);
	glShaderSource(vs, 1, (const GLchar **)&vertex_shader, &length);
	glCompileShader(vs);

	status;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
	if(status == GL_FALSE)
	{
		fprintf(stderr, "vertex shader compilation failed\n");
		return 1;
	}

	length = (GLint) strlen(fragment_shader);
	glShaderSource(fs, 1, (const GLchar **)&fragment_shader, &length);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
	if(status == GL_FALSE)
	{
		fprintf(stderr, "fragment shader compilation failed\n");
		return 1;
	}

	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glBindAttribLocation(program, attrib_position, "i_position");
	glBindAttribLocation(program, attrib_color, "i_color");
	glLinkProgram(program);

	glUseProgram(program);

	// We won't be using the Z-buffer
	glDisable(GL_DEPTH_TEST);

	// ...
	glClearColor(0.5, 0.0, 0.0, 0.0);

	// Full viewport
	glViewport(0, 0, kResX, kResY);

	GLuint vao, vbo;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glEnableVertexAttribArray(attrib_position);
	glEnableVertexAttribArray(attrib_color);

	glVertexAttribPointer(attrib_color, 4, GL_FLOAT, GL_FALSE, sizeof( float )*6, 0);
	glVertexAttribPointer(attrib_position, 2, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void *)(4*sizeof(float)));

	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	t_mat4x4 projection_matrix;
	mat4x4_ortho(projection_matrix, 0.0f, (float) kResX, (float) kResY, 0.0f, 0.0f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(program, "u_projection_matrix" ), 1, GL_FALSE, projection_matrix);

	while (false == demo_is_done)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		SDL_Event event;
		while( SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_QUIT:
				goto quit;

			case SDL_KEYUP:
				if(event.key.keysym.sym == SDLK_ESCAPE)
					goto quit;

			default:
				break;
			}
		}

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		SDL_GL_SwapWindow( window );
		SDL_Delay(1);
	}

quit:
	// Release our own resources
	release_all_plain_text();

	// Release SDL
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return return_value;
}
