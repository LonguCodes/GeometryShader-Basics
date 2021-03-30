# Shader geomteryczny
## Co
Shadery geometryczne to opcjonalne shadery bedace czescia procesu renderowania. Pozwalaja one na przeksztalcenie geometrii obiektu poprzez zmiane, a nawet dodanie lub usuniecie punktow. 

## Jak
Shader ladujemy, kompilujemy i linkujemy tak samo jak vertex lub fragment shader, za wyjatkiem sprecyzowania typu `GL_GEOMETRY_SHADER`.

```cpp
    ...
	unsigned int geometryShader = glCreateShader( GL_GEOMETRY_SHADER );
    ...
	char* geometryShaderSource = shaderLoadSource( geometry );
    ...
	glShaderSource( geometryShader, 1, ( const char** )&geometryShaderSource, NULL );
    ...
	glCompileShader( geometryShader );
	unsigned int shaderProgram = glCreateProgram();
	...
	glAttachShader( shaderProgram, geometryShader );
	glLinkProgram( shaderProgram );
```

## Wnetrze shadera

W naszym shaderze musimy zdefiniowac co przyjmujemy i co zwracamy. Mozemy to osiagnac za pomoca 2 zmiennych layout.
```glsl
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;
```
Wszystkie typy przyjmowane oraz zwracane mozemy sprawdzic [tutaj](https://www.khronos.org/opengl/wiki/Geometry_Shader). W tym przypadku przeksztalcamy pojedynczy trojkat, a wiec przyjmujemy `triangles` oraz zwracamy `triangles_strip` przy maksymalnej ilosci vertexow `3`, co resultuje w pojedynczym trojkacie w obie strony.

W funkcji main otrzymujemy dostep do funkcji `EmitVertex`, ktora zapisuje stan aktualnie ustaiony jako nowy vertex. Ustawianie wartosci dziala dokladnie tak samo jak w vertex shader - mozemy ustawic zmienna `gl_Position` oraz zmienne zdefiniowane ze slowem `out`. Wszystkie nieustawione zmienne zostana zachowane z vertex shadera. 

Wartosci, ktore dostajemy od vertex shadera mozemy pobrac za pomoca zmiennej `gl_in`. Jest to tablica zawierajaca wszystkie **wbudowane** wartosci. Wartosci wlasne (zdefiniowane za pomoca `out` w vertex shaderze) otrzymujemy poprzez zdefiniowanie tablicy o tym samym typie i nazwie ze slowem `in`

```glsl
// Vertex shader

out vec3 outColor;

// Geometry shader

in vec3 outColor[];

```

Po zakonczniu wszystkich operacji w funkcji main musimy wywowal funkcje `EndPrimitive`, ktora sfinalizuje nasz ksztalt.

Musimy takze oznaczyc, ze obiekt buffora w cpp powinien byc przypisany pod konkretny buffor w shaderze.
```cpp
glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 1, buffer );
```

Funkcja ta dziala podobnie jak `localtion` i `glVerterAtribbPointer`, ktorych uzywalismy przy "tradycyjnych shaderach"

## Przykladowe uzycia

Shadery geometryczne pozwalaja nam na definiowanie ksztaltow z prostszych obiektow, jak np zamiany punktow w kola.
```glsl
#version 430 core
layout (points) in;
layout (line_strip, max_vertices = 17) out;

#define pi 3.141592653589793238462643383279

uniform mat4 P;
uniform float pointSize;

void main() {    

    for(int i =0;i<17;i++){
        gl_Position = P*( gl_in[0].gl_Position + vec4(sin(i/8.0*pi), cos(i/8.0*pi), 0.0, 0.0)*pointSize);
        EmitVertex();
    }
    EndPrimitive();
}  
```

Moga byc takze uzyte do pomocy przy tworzeniu naszej sceny. Przydatnym wykorzystaniem jest narysowanie wektorow normalnych dla kazdego punktu.

```glsl
// Wziete ze strony https://learnopengl.com/Advanced-OpenGL/Geometry-Shader

#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.4;
  
uniform mat4 projection;

void GenerateLine(int index)
{
    gl_Position = projection * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = projection * (gl_in[index].gl_Position + 
                                vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
} 
```
