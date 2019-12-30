
/*
	TPB-07

	Yup, OpenGL.

	Shader source links (Shadertoy):
	- Cracktro: https://www.shadertoy.com/view/Wl33WS
	- Caged balls: https://www.shadertoy.com/view/Wt3GWB
	- Trevor: https://www.shadertoy.com/view/Wtc3DB
	- Backup: https://www.shadertoy.com/view/tl3GWf
	- Always check for latest before using backups, esp. check different radial blur tweaks and assigned buffers

	Ravity's flare shader:
	- https://www.shadertoy.com/view/3ttGDj?fbclid=IwAR0QaSRCmMIi9xW2XV8hkyXiK0-an-DL8oyl70VEeiTAfoXFc-P-MX09j2Q

	I swear that I will:
	- Not overengineer a single bit
	- Don't give a fuck about how the code looks
	- Purely aim for result
	- We will use a small subset of OpenGL, so all should be fine

	To do:
	- Ask Nytrik for some typography graphics
	- Get 1 single shader to work as GLSL, this will require:
	  + Loading shader from text, *or*, integrating it as a long string! :)
	- Implement render targets
	- Take some time to split the common functionality out of most shaders and stash them in includes
	- Try to get every shader *but* Trevor's to work
	- Implement cubemaps and implement Trevor's
	- Implement basic 2D rendering system
	- Play some music (BASS)
	- GNU Rocket integration

	Links with information:
	  + Cubemaps: https://learnopengl.com/Advanced-OpenGL/Cubemaps

	Author of tiny boilerplate I took: https://github.com/koute
*/

#include "main_header.h"
#include "glsl_loader.h"

typedef float t_mat4x4[16];

static inline void mat4x4_ortho( t_mat4x4 out, float left, float right, float bottom, float top, float znear, float zfar )
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

// this was/is a little experiment initiated by one Pieter van der Meer
// what we've learned that labels are padded so that they, if so desired, fit within
// the block a branch predictor can use (it used to be 16 bits at a time, generated code suggests as much)
// the lower bits don't matter then, so a loop address can very well be "unaligned" in that manner
// here's more information: https://www.agner.org/optimize/microarchitecture.pdf see section 3.8

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

//

int main( int argc, char * argv[] )
{
//	looptest();

	int retVal = 0;

	SDL_Init( SDL_INIT_VIDEO );

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	static const int width = 1920;
	static const int height = 1080;

	SDL_Window * window = SDL_CreateWindow( "TPB-07", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
	SDL_GLContext context = SDL_GL_CreateContext( window );

	GLenum err = glewInit();
	if (0 != err)
	{
		fprintf( stderr, "initializion of glew failed\n" );	
		return 1;
	}

	GLuint vs, fs, program;

	vs = glCreateShader( GL_VERTEX_SHADER );
	fs = glCreateShader( GL_FRAGMENT_SHADER );

	int length = strlen( vertex_shader );
	glShaderSource( vs, 1, ( const GLchar ** )&vertex_shader, &length );
	glCompileShader( vs );

	GLint status;
	glGetShaderiv( vs, GL_COMPILE_STATUS, &status );
	if( status == GL_FALSE )
	{
		fprintf( stderr, "vertex shader compilation failed\n" );
		return 1;
	}

	length = strlen( fragment_shader );
	glShaderSource( fs, 1, ( const GLchar ** )&fragment_shader, &length );
	glCompileShader( fs );

	glGetShaderiv( fs, GL_COMPILE_STATUS, &status );
	if( status == GL_FALSE )
	{
		fprintf( stderr, "fragment shader compilation failed\n" );
		return 1;
	}

	program = glCreateProgram();
	glAttachShader( program, vs );
	glAttachShader( program, fs );

	glBindAttribLocation( program, attrib_position, "i_position" );
	glBindAttribLocation( program, attrib_color, "i_color" );
	glLinkProgram( program );

	glUseProgram( program );

	glDisable( GL_DEPTH_TEST );
	glClearColor( 0.5, 0.0, 0.0, 0.0 );
	glViewport( 0, 0, width, height );

	GLuint vao, vbo;

	glGenVertexArrays( 1, &vao );
	glGenBuffers( 1, &vbo );
	glBindVertexArray( vao );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );

	glEnableVertexAttribArray( attrib_position );
	glEnableVertexAttribArray( attrib_color );

	glVertexAttribPointer( attrib_color, 4, GL_FLOAT, GL_FALSE, sizeof( float ) * 6, 0 );
	glVertexAttribPointer( attrib_position, 2, GL_FLOAT, GL_FALSE, sizeof( float ) * 6, ( void * )(4 * sizeof(float)) );

	const GLfloat g_vertex_buffer_data[] = {
	/*  R, G, B, A, X, Y  */
		1, 0, 0, 1, 0, 0,
		0, 1, 0, 1, width, 0,
		0, 0, 1, 1, width, height,

		1, 0, 0, 1, 0, 0,
		0, 0, 1, 1, width, height,
		1, 1, 1, 1, 0, height
    };

	glBufferData( GL_ARRAY_BUFFER, sizeof( g_vertex_buffer_data ), g_vertex_buffer_data, GL_STATIC_DRAW );

	t_mat4x4 projection_matrix;
	mat4x4_ortho( projection_matrix, 0.0f, (float)width, (float)height, 0.0f, 0.0f, 100.0f );
	glUniformMatrix4fv( glGetUniformLocation( program, "u_projection_matrix" ), 1, GL_FALSE, projection_matrix );

	for( ;; )
	{
		glClear( GL_COLOR_BUFFER_BIT );

		SDL_Event event;
		while( SDL_PollEvent( &event ) )
		{
			switch( event.type )
			{
				case SDL_KEYUP:
					if( event.key.keysym.sym == SDLK_ESCAPE )
						return 0;
					break;
			}
		}

		glBindVertexArray( vao );
		glDrawArrays( GL_TRIANGLES, 0, 6 );

		SDL_GL_SwapWindow( window );
		SDL_Delay( 1 );
	}

	SDL_GL_DeleteContext( context );
	SDL_DestroyWindow( window );
	SDL_Quit();

	return 0;
}
