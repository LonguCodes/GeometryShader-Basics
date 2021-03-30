#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include "GL\glew.h"
#include "GL\freeglut.h"
#include <chrono>
#include <iostream>
#include <cmath>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective

using namespace std::chrono;

char* shaderLoadSource( const char* filePath );
unsigned int loadComputeShader( const char* filePath );
unsigned int loadDisplayShader( const char* vertex, const char* fragment, const char* geometry = nullptr );

#define UPDATE_RATE 50


#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000

float positions[] = {
	.5,.5,.5,
	.5,.5,-.5,
	.5,-.5,.5,
	.5,-.5,-.5,
	-.5,.5,.5,
	-.5,.5,-.5,
	-.5,-.5,.5,
	-.5,-.5,-.5,
};
float colors[] = {
	0,0,0,
	0,0,1,
	0,1,0,
	0,1,1,
	1,0,0,
	1,0,1,
	1,1,0,
	1,1,1,
};
unsigned int indicies[] = {
	1,0,2,
	1,2,3,
	4,5,7,
	6,4,7,
	2,6,7,
	3,2,7,
	0,1,4,
	4,1,5,
	0,4,2,
	2,4,6,
	1,3,7,
	1,7,5
};


unsigned int VBOposition;
unsigned int VBOcolor;
unsigned int VAO;
unsigned int EBO;

unsigned int displayShader;
unsigned int lowPolyDisplayShader;

glm::mat4 P = glm::perspective(120.0f, ( float ) SCREEN_HEIGHT/ ( float ) SCREEN_WIDTH,1.25f,100.0f);
void display();
void timer( int value );


int main( int argc, char** argv )
{
	glutInit( &argc, argv );
	glutInitContextVersion( 4, 3 );
	glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize( SCREEN_WIDTH, SCREEN_HEIGHT );
	glutInitWindowPosition( 0, 0 );
	glutCreateWindow( "Pierwszy prog" );
	glewInit();
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );


	displayShader = loadDisplayShader( "vertex_shader.glsl", "fragment_shader.glsl" );
	displayShader = loadDisplayShader( "vertex_shader_with_geom.glsl", "fragment_shader.glsl", "geometry_shader.glsl" );

	glUseProgram( displayShader );

	glUniformMatrix4fv( glGetUniformLocation( displayShader, "P" ), 1, false, ( const GLfloat* )&P );

	glGenVertexArrays( 1, &VAO );

	glBindVertexArray( VAO );

	glGenBuffers( 1, &VBOposition );
	glBindBuffer( GL_ARRAY_BUFFER, VBOposition );
	glBufferData( GL_ARRAY_BUFFER, sizeof( positions ), positions, GL_STREAM_DRAW );

	glEnableVertexAttribArray( 0 );

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );

	glGenBuffers( 1, &VBOcolor );
	glBindBuffer( GL_ARRAY_BUFFER, VBOcolor );
	glBufferData( GL_ARRAY_BUFFER, sizeof( colors ), colors, GL_STREAM_DRAW );

	glEnableVertexAttribArray( 1 );

	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );


	glGenBuffers( 1, &EBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indicies ), indicies, GL_STREAM_DRAW );
	
	glBindVertexArray( 0 );


	glutTimerFunc( 1000 / UPDATE_RATE, timer, 0 );
	glutDisplayFunc( display );
	glutMainLoop();

	return 0;
}

void timer( int value )
{
	glUseProgram( displayShader );
	glm::mat4 M = glm::rotate( glm::translate(glm::mat4(),glm::vec3(0,0,-5)),(float)value/UPDATE_RATE,glm::vec3(1,1.0f,0.0f));
	glUniformMatrix4fv( glGetUniformLocation( displayShader, "M" ), 1, false, ( const GLfloat* ) &M );

	glutPostRedisplay();
	glutTimerFunc( 1000 / UPDATE_RATE, timer, value+1 );
}

void display()
{
	glUseProgram( displayShader );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glBindVertexArray( VAO );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glUniformMatrix4fv( glGetUniformLocation( displayShader, "P" ), 1, false, ( const GLfloat* )&P );
	glDrawElements( GL_TRIANGLES,48,GL_UNSIGNED_INT, 0);

	glutSwapBuffers();
}


unsigned int loadDisplayShader( const char* vertex, const char* fragment, const char* geometry )
{
	unsigned int vertexShader = glCreateShader( GL_VERTEX_SHADER );
	unsigned int fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
	unsigned int geometryShader;
	if ( geometry )
		geometryShader = glCreateShader( GL_GEOMETRY_SHADER );

	char* vertexShaderSource = shaderLoadSource( vertex );
	char* fragmentShaderSource = shaderLoadSource( fragment );
	char* geometryShaderSource;
	if ( geometry )
		geometryShaderSource = shaderLoadSource( geometry );

	if ( !fragmentShaderSource || !vertexShaderSource || ( geometry && !geometryShader ) )
		return -1;

	glShaderSource( vertexShader, 1, ( const char** )&vertexShaderSource, NULL );
	glShaderSource( fragmentShader, 1, ( const char** )&fragmentShaderSource, NULL );
	if ( geometry )
		glShaderSource( geometryShader, 1, ( const char** )&geometryShaderSource, NULL );

	glCompileShader( vertexShader );
	glCompileShader( fragmentShader );
	if ( geometry )
		glCompileShader( geometryShader );

	unsigned int shaderProgram = glCreateProgram();

	glAttachShader( shaderProgram, vertexShader );
	glAttachShader( shaderProgram, fragmentShader );
	if ( geometry )
		glAttachShader( shaderProgram, geometryShader );

	glLinkProgram( shaderProgram );

	glDeleteShader( vertexShader );
	glDeleteShader( fragmentShader );
	if ( geometry )
		glDeleteShader( geometryShader );

	return shaderProgram;
}

char* shaderLoadSource( const char* filePath )
{
	const size_t blockSize = 512;
	FILE* fp;
	char buf[512];
	char* source = NULL;
	size_t tmp, sourceLength = 0;
	fopen_s( &fp, filePath, "r" );
	if ( !fp )
	{
		fprintf( stderr, "shaderLoadSource(): Unable to open %s for reading\n", filePath );
		return NULL;
	}

	while ( ( tmp = fread( buf, 1, blockSize, fp ) ) > 0 )
	{
		char* newSource = ( char* )malloc( sourceLength + tmp + 1 );
		if ( !newSource )
		{
			fprintf( stderr, "shaderLoadSource(): malloc failed\n" );
			if ( source )
				free( source );
			return NULL;
		}

		if ( source )
		{
			memcpy( newSource, source, sourceLength );
			free( source );
		}
		memcpy( newSource + sourceLength, buf, tmp );

		source = newSource;
		sourceLength += tmp;
	}

	fclose( fp );
	if ( source )
		source[sourceLength] = '\0';

	return source;
}
