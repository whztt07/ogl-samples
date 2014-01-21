//**********************************
// OpenGL Samples Pack
// ogl-samples.g-truc.net
//**********************************
// OpenGL Point
// 24/09/2011 - 16/02/2013
//**********************************
// Christophe Riccio
// ogl-samples@g-truc.net
//**********************************
// G-Truc Creation
// www.g-truc.net
//**********************************

#include <glf/glf.hpp>

namespace
{
	glf::window Window("gl-320-primitive-point");

	char const * VERT_SHADER_SOURCE("gl-320/primitive-point.vert");
	char const * FRAG_SHADER_SOURCE("gl-320/primitive-point.frag");

	std::size_t const VertexCount(4000);

	namespace shader
	{
		enum type
		{
			VERT,
			FRAG,
			MAX
		};
	}//namespace shader

	std::vector<GLuint> ShaderName(shader::MAX);
	GLuint VertexArrayName(0);
	GLuint ProgramName(0);
	GLuint BufferName(0);
	GLint UniformMVP(0);
	GLint UniformMV(0);
}//namespace

bool initProgram()
{
	bool Validated = true;
	
	if(Validated)
	{
		glf::compiler Compiler;
		ShaderName[shader::VERT] = Compiler.create(GL_VERTEX_SHADER, glf::DATA_DIRECTORY + VERT_SHADER_SOURCE, "--version 150 --profile core");
		ShaderName[shader::FRAG] = Compiler.create(GL_FRAGMENT_SHADER, glf::DATA_DIRECTORY + FRAG_SHADER_SOURCE, "--version 150 --profile core");
		Validated = Validated && Compiler.check();

		ProgramName = glCreateProgram();
		glAttachShader(ProgramName, ShaderName[shader::VERT]);
		glAttachShader(ProgramName, ShaderName[shader::FRAG]);

		glBindAttribLocation(ProgramName, glf::semantic::attr::POSITION, "Position");
		glBindAttribLocation(ProgramName, glf::semantic::attr::COLOR, "Color");
		glBindFragDataLocation(ProgramName, glf::semantic::frag::COLOR, "Color");
		glLinkProgram(ProgramName);
		Validated = Validated && glf::checkProgram(ProgramName);
	}

	// Get variables locations
	if(Validated)
	{
		UniformMVP = glGetUniformLocation(ProgramName, "MVP");
		UniformMV = glGetUniformLocation(ProgramName, "MV");
	}

	return Validated && glf::checkError("initProgram");
}

// Buffer update using glBufferSubData
bool initBuffer()
{
	// Generate a buffer object
	glGenBuffers(1, &BufferName);

	// Bind the buffer for use
	glBindBuffer(GL_ARRAY_BUFFER, BufferName);

	// Reserve buffer memory but don't copy the values
	glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(VertexCount * sizeof(glf::vertex_v4fc4f)), NULL, GL_STATIC_DRAW);

	glf::vertex_v4fc4f* Data = (glf::vertex_v4fc4f*)glMapBufferRange(
		GL_ARRAY_BUFFER, 
		0,				// Offset
		GLsizeiptr(VertexCount * sizeof(glf::vertex_v4fc4f)),	// Size,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	for(std::size_t i = 0; i < VertexCount; ++i)
	{
		//Data[i].Position = glm::vec4(glm::linearRand(glm::vec2(-1), glm::vec2(1)), glm::gaussRand(0.0f, 1.0f), 1);
		//Data[i].Position = glm::vec4(glm::linearRand(glm::vec2(-1), glm::vec2(1)), /*glm::gaussRand(0.0f, 1.0f)*/0, 1);
		//Data[i].Position = glm::vec4(glm::sphericalRand(1.0f), 1);
		Data[i].Position = glm::vec4(glm::gaussRand(glm::vec3(0), glm::vec3(1)), 1);
		//Data[i].Position = glm::vec4(glm::circularRand(1.0f), 0, 1);
		//Data[i].Position = glm::vec4(glm::diskRand(1.0f), 0, 1);
		//Data[i].Position = glm::vec4(glm::ballRand(1.0f), 1);
		Data[i].Color = glm::vec4(1);
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	// Unbind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return glf::checkError("initArrayBuffer");
}

bool initVertexArray()
{
	glGenVertexArrays(1, &VertexArrayName);
	glBindVertexArray(VertexArrayName);
		glBindBuffer(GL_ARRAY_BUFFER, BufferName);
		glVertexAttribPointer(glf::semantic::attr::POSITION, 4, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v4fc4f), GLF_BUFFER_OFFSET(0));
		glVertexAttribPointer(glf::semantic::attr::COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v4fc4f), GLF_BUFFER_OFFSET(sizeof(glm::vec4)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(glf::semantic::attr::POSITION);
		glEnableVertexAttribArray(glf::semantic::attr::COLOR);
	glBindVertexArray(0);

	return glf::checkError("initVertexArray");
}

bool begin()
{
	bool Validated = true;

	if(Validated)
		Validated = initProgram();
	if(Validated)
		Validated = initBuffer();
	if(Validated)
		Validated = initVertexArray();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_PROGRAM_POINT_SIZE);
	//glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
	glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_UPPER_LEFT);

	return Validated && glf::checkError("begin");
}

bool end()
{
	for(std::size_t i = 0; 0 < shader::MAX; ++i)
		glDeleteShader(ShaderName[i]);
	glDeleteBuffers(1, &BufferName);
	glDeleteProgram(ProgramName);
	glDeleteVertexArrays(1, &VertexArrayName);

	return glf::checkError("end");
}

void display()
{
	glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Window.TranlationCurrent.y));
	glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, Window.RotationCurrent.y, glm::vec3(1.f, 0.f, 0.f));
	glm::mat4 View = glm::rotate(ViewRotateX, Window.RotationCurrent.x, glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;
	glm::mat4 MV = View * Model;

	float Depth(1.0f);
	glViewport(0, 0, Window.Size.x, Window.Size.y);
	glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f)[0]);
	glClearBufferfv(GL_DEPTH, 0, &Depth);

	glDisable(GL_SCISSOR_TEST);

	glUseProgram(ProgramName);
	glUniformMatrix4fv(UniformMV, 1, GL_FALSE, &MV[0][0]);
	glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);

	glBindVertexArray(VertexArrayName);

	glDrawArraysInstanced(GL_POINTS, 0, VertexCount, 1);

	glf::checkError("display");

}

int main(int argc, char* argv[])
{
	return glf::run(argc, argv, glf::CORE, 3, 2);
}
