//**********************************
// OpenGL Geometry Shader
// 26/08/2009
//**********************************
// Christophe Riccio
// g.truc.creation@gmail.com
//**********************************
// G-Truc Creation
// www.g-truc.net
//**********************************

#include "sample.hpp"

namespace
{
	std::string const SAMPLE_NAME = "OpenGL Geometry Shader";	
	GLint const SAMPLE_MAJOR_VERSION = 3;
	GLint const SAMPLE_MINOR_VERSION = 3;
	std::string const VERT_SHADER_SOURCE(glf::DATA_DIRECTORY + "330/flat-color.vert");
	std::string const GEOM_SHADER_SOURCE(glf::DATA_DIRECTORY + "330/flat-color.geom");
	std::string const FRAG_SHADER_SOURCE(glf::DATA_DIRECTORY + "330/flat-color.frag");

	GLsizei const VertexCount = 4;
	GLsizeiptr const PositionSize = VertexCount * sizeof(glm::vec2);
	glm::vec2 const PositionData[VertexCount] =
	{
		glm::vec2(-1.0f,-1.0f),
		glm::vec2( 1.0f,-1.0f),
		glm::vec2( 1.0f, 1.0f),
		glm::vec2(-1.0f, 1.0f)
	};

	GLsizei const ElementCount = 6;
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
	GLushort const ElementData[ElementCount] =
	{
		0, 1, 2, 
		2, 3, 0
	};

}//namespace

sample::sample
(
	std::string const & Name, 
	glm::ivec2 const & WindowSize,
	glm::uint32 const & VersionMajor,
	glm::uint32 const & VersionMinor
) :
	window(Name, WindowSize, VersionMajor, VersionMinor),
	ProgramName(0)
{}

sample::~sample()
{}

bool sample::check() const
{
	GLint MajorVersion = 0;
	GLint MinorVersion = 0;
	glGetIntegerv(GL_MAJOR_VERSION, &MajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &MinorVersion);
	bool Version = (MajorVersion * 10 + MinorVersion) >= (glf::SAMPLE_MAJOR * 10 + glf::SAMPLE_MINOR);
	return Version && glf::checkError("sample::check");
}

bool sample::begin(glm::ivec2 const & WindowSize)
{
	this->WindowSize = WindowSize;

	bool Validated = true;
	if(Validated)
		Validated = this->initProgram();
	if(Validated)
		Validated = this->initArrayBuffer();
	if(Validated)
		Validated = this->initVertexArray();

	return Validated && glf::checkError("sample::begin");
}

bool sample::end()
{
	glDeleteVertexArrays(1, &this->VertexArrayName);
	glDeleteBuffers(1, &this->ArrayBufferName);
	glDeleteBuffers(1, &this->ElementBufferName);
	glDeleteProgram(this->ProgramName);

	return glf::checkError("sample::end");
}

void sample::render()
{
	// Compute the MVP (Model View Projection matrix)
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -TranlationCurrent.y));
	glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, RotationCurrent.y, glm::vec3(-1.f, 0.f, 0.f));
	glm::mat4 View = glm::rotate(ViewRotateX, RotationCurrent.x, glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	// Set the display viewport
	glViewport(0, 0, this->WindowSize.x, this->WindowSize.y);

	// Clear color buffer with black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Bind program
	glUseProgram(this->ProgramName);

	// Set the value of MVP uniform.
	glUniformMatrix4fv(this->UniformMVP, 1, GL_FALSE, &MVP[0][0]);

	// Bind vertex array & draw 
	glBindVertexArray(this->VertexArrayName);
		glDrawElements(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Unbind program
	glUseProgram(0);

	glf::checkError("sample::render");
}

bool sample::initProgram()
{
	bool Validated = true;
	
	// Create program
	if(Validated)
	{
		GLuint VertShader = glf::createShader(GL_VERTEX_SHADER, VERT_SHADER_SOURCE);
		glf::checkError("VertShader");
		GLuint GeomShader = glf::createShader(GL_GEOMETRY_SHADER, GEOM_SHADER_SOURCE);
		glf::checkError("GeomShader");
		GLuint FragShader = glf::createShader(GL_FRAGMENT_SHADER, FRAG_SHADER_SOURCE);
		glf::checkError("FragShader");

		this->ProgramName = glCreateProgram();
		glAttachShader(this->ProgramName, VertShader);
		glAttachShader(this->ProgramName, GeomShader);
		glAttachShader(this->ProgramName, FragShader);
		glDeleteShader(VertShader);
		glDeleteShader(GeomShader);
		glDeleteShader(FragShader);

		glLinkProgram(this->ProgramName);
		Validated = glf::checkProgram(this->ProgramName);

		this->ProgramName = glf::createProgram(VERT_SHADER_SOURCE, FRAG_SHADER_SOURCE);
		glLinkProgram(this->ProgramName);
		Validated = glf::checkProgram(this->ProgramName);
	}

	// Get variables locations
	if(Validated)
	{
		UniformMVP = glGetUniformLocation(this->ProgramName, "MVP");
		UniformDiffuse = glGetUniformLocation(this->ProgramName, "Diffuse");
	}

	// Set some variables 
	if(Validated)
	{
		// Bind the program for use
		glUseProgram(this->ProgramName);

		// Set uniform value
		glUniform4fv(this->UniformDiffuse, 1, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

		// Unbind the program
		glUseProgram(0);
	}

	return Validated && glf::checkError("sample::initProgram");
}

bool sample::initVertexArray()
{
	// Build a vertex array object
	glGenVertexArrays(1, &this->VertexArrayName);
    glBindVertexArray(this->VertexArrayName);
		glBindBuffer(GL_ARRAY_BUFFER, this->ArrayBufferName);
		glVertexAttribPointer(glf::semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(glf::semantic::attr::POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ElementBufferName);
	glBindVertexArray(0);

	return glf::checkError("sample::initVertexArray");
}

bool sample::initArrayBuffer()
{
	// Generate a buffer object
	glGenBuffers(1, &this->ElementBufferName);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ElementBufferName);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &this->ArrayBufferName);
    glBindBuffer(GL_ARRAY_BUFFER, this->ArrayBufferName);
    glBufferData(GL_ARRAY_BUFFER, PositionSize, PositionData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return glf::checkError("sample::initArrayBuffer");
}

int main(int argc, char* argv[])
{
	glm::ivec2 ScreenSize(640, 480);

	sample* Sample = new sample(
		SAMPLE_NAME, 
		ScreenSize, 
		SAMPLE_MAJOR_VERSION,
		SAMPLE_MINOR_VERSION);

	if(Sample->check())
	{
		Sample->begin(ScreenSize);
		Sample->run();
		Sample->end();

		delete Sample;
		return 0;
	}

	fprintf(stderr, "OpenGL Error: this sample failed to run\n");

	delete Sample;
	Sample = 0;
	return 1;
}