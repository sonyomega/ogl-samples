//**********************************
// OpenGL blend index
// 28/05/2010
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
	std::string const SAMPLE_NAME = "OpenGL blend index";
	GLint const SAMPLE_MAJOR_VERSION = 3;
	GLint const SAMPLE_MINOR_VERSION = 3;
	std::string const VERTEX_SHADER_SOURCE(glf::DATA_DIRECTORY + "330/blend-index.vert");
	std::string const FRAGMENT_SHADER_SOURCE(glf::DATA_DIRECTORY + "330/blend-index.frag");
	std::string const TEXTURE_DIFFUSE(glf::DATA_DIRECTORY + "kueken256-rgb8.dds");

	struct vertex
	{
		vertex
		(
			glm::vec2 const & Position,
			glm::vec2 const & Texcoord
		) :
			Position(Position),
			Texcoord(Texcoord)
		{}

		glm::vec2 Position;
		glm::vec2 Texcoord;
	};

	// With DDS textures, v texture coordinate are reversed, from top to bottom
	GLsizei const VertexCount = 6;
	GLsizeiptr const VertexSize = VertexCount * sizeof(vertex);
	vertex const VertexData[VertexCount] =
	{
		vertex(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 1.0f)),
		vertex(glm::vec2( 1.0f,-1.0f), glm::vec2(1.0f, 1.0f)),
		vertex(glm::vec2( 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
		vertex(glm::vec2( 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
		vertex(glm::vec2(-1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
		vertex(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 1.0f))
	};
}

sample::sample
(
	std::string const & Name, 
	glm::ivec2 const & WindowSize,
	glm::uint32 VersionMajor,
	glm::uint32 VersionMinor
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
	bool Version = (MajorVersion * 10 + MinorVersion) >= (SAMPLE_MAJOR_VERSION * 10 + SAMPLE_MINOR_VERSION);
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
	if(Validated)
		Validated = this->initTexture2D();

	return Validated && glf::checkError("sample::begin");
}

bool sample::end()
{
	glDeleteBuffers(1, &this->BufferName);
	glDeleteProgram(this->ProgramName);
	glDeleteTextures(1, &this->Texture2DName);
	glDeleteVertexArrays(1, &this->VertexArrayName);

	return glf::checkError("sample::end");
}

void sample::render()
{
	// Setup blending
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_COLOR, GL_SRC1_COLOR);

	// Compute the MVP (Model View Projection matrix)
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -TranlationCurrent.y));
	glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, RotationCurrent.y, glm::vec3(-1.f, 0.f, 0.f));
	glm::mat4 View = glm::rotate(ViewRotateX, RotationCurrent.x, glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	glViewport(0, 0, this->WindowSize.x, this->WindowSize.y);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(this->ProgramName);

	glUniform1i(this->UniformDiffuse, 0);
	glUniformMatrix4fv(this->UniformMVP, 1, GL_FALSE, &MVP[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->Texture2DName);

	glBindVertexArray(this->VertexArrayName);
	glDrawArrays(GL_TRIANGLES, 0, VertexCount);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);

	glf::checkError("sample::render");
}

bool sample::initProgram()
{
	bool Validated = true;
	
	// Create program
	if(Validated)
	{
		this->ProgramName = glf::createProgram(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE);
		glLinkProgram(this->ProgramName);
		Validated = glf::checkProgram(this->ProgramName);
	}

	if(Validated)
	{
		UniformMVP = glGetUniformLocation(this->ProgramName, "MVP");
		UniformDiffuse = glGetUniformLocation(this->ProgramName, "Diffuse");
	}

	return glf::checkError("sample::initProgram");
}

bool sample::initArrayBuffer()
{
	glGenBuffers(1, &this->BufferName);

    glBindBuffer(GL_ARRAY_BUFFER, this->BufferName);
    glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return glf::checkError("sample::initArrayBuffer");;
}

bool sample::initTexture2D()
{
	glGenTextures(1, &this->Texture2DName);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->Texture2DName);

	// Set filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Set image
	gli::image Image = gli::import_as(TEXTURE_DIFFUSE);
	for(std::size_t Level = 0; Level < Image.levels(); ++Level)
	{
		glTexImage2D(
			GL_TEXTURE_2D, 
			GLint(Level), 
			GL_RGB, 
			GLsizei(Image[Level].dimensions().x), 
			GLsizei(Image[Level].dimensions().y), 
			0,  
			GL_BGR, 
			GL_UNSIGNED_BYTE, 
			Image[Level].data());
	}

	return glf::checkError("initTexture2D");
}

bool sample::initVertexArray()
{
	// Create a dummy vertex array object where all the attribute buffers and element buffers would be attached 
	glGenVertexArrays(1, &this->VertexArrayName);
    glBindVertexArray(this->VertexArrayName);
		glBindBuffer(GL_ARRAY_BUFFER, this->BufferName);
		glVertexAttribPointer(glf::semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), GLF_BUFFER_OFFSET(0));
		glVertexAttribPointer(glf::semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), GLF_BUFFER_OFFSET(sizeof(glm::vec2)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(glf::semantic::attr::POSITION);
		glEnableVertexAttribArray(glf::semantic::attr::TEXCOORD);
	glBindVertexArray(0);

	return glf::checkError("sample::initVertexArray");
}

int main(int argc, char* argv[])
{
	glm::ivec2 ScreenSize = glm::ivec2(640, 480);

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