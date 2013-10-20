#include "akj_ogl.h"
#include "cs314_util.hpp"
#include "cs314_math.hpp"
#include <stdio.h>
#include <assert.h>
#include "VertexArray.hpp"

#ifndef STRINGIFY
#define STRINGIFY(var) #var
#endif

static const double kLogOf2 =  0.6931471805599453;
#ifdef _WIN32
	#define AKJ_GET_GL_FUNCTION_ADDRESS(name,type) {name = (type)wglGetProcAddress(#name); P4_ASSERT(name != NULL);}
#else   //linux or mac
	#include "GL/glx.h"
	#define AKJ_GET_GL_FUNCTION_ADDRESS(name,type)	{name = (type)glXGetProcAddress((const GLubyte*)#name); P4_ASSERT(name != NULL);}
#endif 

#define PRINT_GL_INT_DEF(enum) {GLint i = 0; glGetIntegerv(enum, &i); p4::Log::Debug("%d = %s", i, #enum);}


#ifdef WIN32
	PFNGLACTIVETEXTUREPROC glActiveTexture = NULL;
	PFNGLBLENDEQUATIONPROC glBlendEquation = NULL;
#endif
PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
PFNGLCREATESHADERPROC glCreateShader = NULL;
PFNGLCOMPILESHADERPROC glCompileShader = NULL;
PFNGLDELETEPROGRAMPROC glDeleteProgram = NULL;
PFNGLDELETESHADERPROC glDeleteShader = NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL;
PFNGLGETSHADERIVPROC glGetShaderiv = NULL;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = NULL;
PFNGLATTACHSHADERPROC glAttachShader = NULL;
PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders = NULL;
PFNGLISSHADERPROC glIsShader = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
PFNGLGETPROGRAMIVPROC glGetProgramiv = NULL;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
PFNGLVALIDATEPROGRAMPROC glValidateProgram = NULL;
PFNGLUSEPROGRAMPROC glUseProgram = NULL;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = NULL;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
PFNGLUNIFORM1IPROC glUniform1i = NULL;
PFNGLUNIFORM2FPROC glUniform2f = NULL;
PFNGLUNIFORM3FPROC glUniform3f = NULL;
PFNGLUNIFORM4FPROC glUniform4f = NULL;
PFNGLUNIFORM4FVPROC glUniform4fv = NULL;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = NULL;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = NULL;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = NULL;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = NULL;
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers = NULL;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers = NULL;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer = NULL;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage = NULL;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer = NULL;
PFNGLUNIFORM1FPROC glUniform1f = NULL;
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate = NULL;
PFNGLGENBUFFERSPROC glGenBuffers = NULL;
PFNGLBINDBUFFERPROC glBindBuffer = NULL;
PFNGLBUFFERDATAPROC glBufferData = NULL;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL;
PFNGLBUFFERSUBDATAPROC glBufferSubData = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = NULL;
PFNGLMAPBUFFERPROC glMapBuffer = NULL;





namespace akj{

void glDrawBillBoardX()
{
	glNormalPointer(GL_FLOAT, 0, kBillBoardXNormals);
	glVertexPointer(3, GL_FLOAT, 0, kBillBoardXVerts);
	glTexCoordPointer(3, GL_FLOAT, 0, kBillBoardXTex);
	glDrawArrays(GL_TRIANGLES, 0, 18);
}
void glDrawRect2D()
{
	glNormalPointer(GL_FLOAT, 0, cGLCube::kCubeNormals);
	glVertexPointer(3, GL_FLOAT, 0, cGLCube::kCubeVerts);
	glTexCoordPointer(3, GL_FLOAT, 0, cGLCube::kCubeTexCoords);
	glDrawArrays(GL_TRIANGLES, 12, 6);
}
void glInit()
{
	AKJ_GET_GL_FUNCTION_ADDRESS(glCreateProgram, PFNGLCREATEPROGRAMPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glCreateShader, PFNGLCREATESHADERPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glCompileShader, PFNGLCOMPILESHADERPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glDeleteShader, PFNGLDELETESHADERPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glDeleteProgram, PFNGLDELETEPROGRAMPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glShaderSource, PFNGLSHADERSOURCEPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glGetShaderiv, PFNGLGETSHADERIVPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glAttachShader, PFNGLATTACHSHADERPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glGetAttachedShaders, PFNGLGETATTACHEDSHADERSPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glIsShader, PFNGLISSHADERPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glLinkProgram, PFNGLLINKPROGRAMPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glGetProgramiv, PFNGLGETPROGRAMIVPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glGetProgramInfoLog, PFNGLGETPROGRAMINFOLOGPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glValidateProgram, PFNGLVALIDATEPROGRAMPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glUseProgram, PFNGLUSEPROGRAMPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glGetAttribLocation, PFNGLGETATTRIBLOCATIONPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glGetUniformLocation, PFNGLGETUNIFORMLOCATIONPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glCreateProgram, PFNGLCREATEPROGRAMPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glUniform1i, PFNGLUNIFORM1IPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glUniform2f, PFNGLUNIFORM2FPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glUniform3f, PFNGLUNIFORM3FPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glUniform4f, PFNGLUNIFORM4FPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glUniform4fv, PFNGLUNIFORM4FVPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glGenFramebuffers, PFNGLGENFRAMEBUFFERSPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glDeleteFramebuffers, PFNGLDELETEFRAMEBUFFERSPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glFramebufferTexture2D, PFNGLFRAMEBUFFERTEXTURE2DPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glCheckFramebufferStatus, PFNGLCHECKFRAMEBUFFERSTATUSPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glBindFramebuffer, PFNGLBINDFRAMEBUFFERPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glGenRenderbuffers, PFNGLGENRENDERBUFFERSPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glDeleteRenderbuffers, PFNGLDELETERENDERBUFFERSPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glBindRenderbuffer, PFNGLBINDRENDERBUFFERPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glRenderbufferStorage, PFNGLRENDERBUFFERSTORAGEPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glFramebufferRenderbuffer, PFNGLFRAMEBUFFERRENDERBUFFERPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glUniform1f, PFNGLUNIFORM1FPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glBlendFuncSeparate, PFNGLBLENDFUNCSEPARATEPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glGenBuffers, PFNGLGENBUFFERSPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glBindBuffer, PFNGLBINDBUFFERPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glBufferData, PFNGLBUFFERDATAPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glDeleteBuffers, PFNGLDELETEBUFFERSPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glBufferSubData, PFNGLBUFFERSUBDATAPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glVertexAttribPointer, PFNGLVERTEXATTRIBPOINTERPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glUniformMatrix4fv, PFNGLUNIFORMMATRIX4FVPROC);
	AKJ_GET_GL_FUNCTION_ADDRESS(glMapBuffer, PFNGLMAPBUFFERPROC);

	#ifdef WIN32
		AKJ_GET_GL_FUNCTION_ADDRESS(glActiveTexture, PFNGLACTIVETEXTUREPROC);
		AKJ_GET_GL_FUNCTION_ADDRESS(glBlendEquation, PFNGLBLENDEQUATIONPROC);
	#endif // WIN32

	PRINT_GL_INT_DEF(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
	PRINT_GL_INT_DEF(GL_MAX_VERTEX_ATTRIBS);
	PRINT_GL_INT_DEF(GL_MAX_TEXTURE_COORDS);
	PRINT_GL_INT_DEF(GL_MAX_TEXTURE_IMAGE_UNITS);
	PRINT_GL_INT_DEF(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS);
	PRINT_GL_INT_DEF(GL_MAX_VERTEX_UNIFORM_COMPONENTS);
	PRINT_GL_INT_DEF(GL_MAX_VARYING_FLOATS);
	PRINT_GL_INT_DEF(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
	PRINT_GL_INT_DEF(GL_MAX_CUBE_MAP_TEXTURE_SIZE);
}

const char* glErrorStringFromCode( int code )
{
	const char* ret = "";
	switch (code)
	{
	case GL_NO_ERROR:
		ret = "GL_NO_ERROR";
		break;
	case GL_INVALID_ENUM:
		ret = "GL_INVALID_ENUM";
		break;
	case GL_INVALID_VALUE:
		ret = "GL_INVALID_VALUE";
		break;
	case GL_INVALID_OPERATION:
		ret = "GL_INVALID_OPERATION";
		break;
	case GL_STACK_OVERFLOW:
		ret = "GL_STACK_OVERFLOW";
		break;
	case GL_STACK_UNDERFLOW:
		ret = "GL_STACK_UNDERFLOW";
		break;
	case GL_OUT_OF_MEMORY:
		ret = "GL_OUT_OF_MEMORY";
		break;
	case GL_TABLE_TOO_LARGE:
		ret = "TABLE_TOO_LARGE";
		break;
	default:
		ret = "Unknown error";
	}
	return ret;
}

int glCheckAllErrors( const char* file, int line )
{
	int error_count = 0;
	for (GLint error_code = glGetError(); error_code != GL_NO_ERROR; error_code = glGetError())
	{
		if(0 == error_count)
		{
			p4::Log::Warn("found glError(s) at %s: %d", file, line);
		}
		++error_count;

		p4::Log::Warn("## %3d >> glError %x : %s", error_count, error_code, glErrorStringFromCode(error_code));
	}
	if(error_count > 0)
	{
		p4::Log::Warn("## end errors");
	}
	return error_count;
}



const GLubyte* glTestTexture()
{
	static const GLubyte kTestImage[64] = 
	{
		0,   128,   124, 0,
		 255,   0,   0, 255,
		   0, 255,   0, 255,
		   0,   0, 255, 0,
		 255, 255,   0, 0,
		   0, 255, 255, 255,
		 255,   0, 255, 0,
		 255, 255,   0, 255,
		 255,   0, 255, 255,
		   0, 255,   0, 0,
		   0,   0, 255, 255,
		   0, 128,   0, 255,
		   0, 128,   0, 255,
		   0,   0, 128, 255,
		   0,   0, 128, 255,
		   0,   0, 128, 255,
	};
	return kTestImage;
}

cGLObject::cGLObject( const char* object_name ):mObjectName(object_name),mObjectID(GL_INVALID_VALUE)
{
}

cGLObject::cGLObject( const char* prefix, const char* object_name ): mObjectID(GL_INVALID_VALUE)
{
	mObjectName = prefix;
	mObjectName.append(object_name);
}


cGLShader::cGLShader()
	:mShaderProgram(0)
	,mFragmentShader(0)
	,mVertexShader(0)
	,mProjectionUniform(-1)
	,mModelViewUniform(-1)
	,mNormalMatrixUniform(-1)
	,mVersionString("#version 330\n")
{
	mShaderProgram = glCreateProgram();
	mFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	mVertexShader = glCreateShader(GL_VERTEX_SHADER);
	if(glCheckAllErrors(__FILE__, __LINE__))
	{
		p4::Log::Error("there were errors constructing the shader object.");
	}
}

cGLShader::~cGLShader()
{
	glDeleteProgram(mShaderProgram);
	glDeleteShader(mFragmentShader);
	glDeleteShader(mVertexShader);
}

bool cGLShader::SetFragmentShader( const char** file_names, int num_files )
{
	if(GL_FRAGMENT_SHADER != GetShaderType(mFragmentShader))
	{
		p4::Log::Error("can't set fragment shader: mFragmentShader is not a valid shader object");
		return false;
	}
	if(GL_NO_ERROR != LoadShaderImpl(mFragmentShader, file_names, num_files))
	{
		return false;
	}
	return true;
}

bool cGLShader::SetVertexShader( const char** file_names, int num_files  )
{
	if(GL_VERTEX_SHADER != GetShaderType(mVertexShader))
	{
		p4::Log::Error("can't set fragment shader: mVertexShader is not a valid shader object");
		return false;
	}
	if(GL_NO_ERROR != LoadShaderImpl(mVertexShader, file_names, num_files))
	{
		return false;
	}
	return true;
}

int cGLShader::LoadShaderImpl( GLuint shader_id, const char** file_names, int num_files )
{
	const int max_files = 8;
	if(num_files > max_files)
	{
		p4::Log::Error("Shader compilation failed: too many files (yours:%d, max:%d)", num_files, max_files);
		return 1;
	}
	const char* string_pointers[max_files+1];
	string_pointers[0] = mVersionString;
	//init the others
	for (int i = 1; i < max_files+1; i++)
	{
		string_pointers[i] = NULL;
	}
	//load the files
	std::string shader_text[max_files];
	for (int i = 0; i < num_files; i++)
	{
		if(FileToString(file_names[i], shader_text[i]) != 0)
		{
			return 1;
		}
		string_pointers[i+1] = shader_text[i].c_str();
	}
	
	

	glShaderSource(shader_id, num_files+1, string_pointers, NULL);
	if(glCheckAllErrors(__FILE__, __LINE__))
	{
		p4::Log::Error("there were errors setting shader source for file '%s' (ID 0x%x).",  file_names[num_files-1], shader_id );
		return 1;
	}
	if(GL_FALSE == glIsShader(shader_id))
	{
		p4::Log::Error("somehow this is not a shader object now (id = %x).", shader_id );
		return 1;
	}

	glCompileShader(shader_id);
	
	if(glCheckAllErrors(__FILE__, __LINE__))
	{
		p4::Log::Warn("there were errors compiling shader %x, file: %s.", shader_id, file_names[num_files-1] );
		return 1;
	}

	const char* compile_status_string = "Failed";
	GLint compiled_ok = GL_FALSE;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);
	if(compiled_ok == GL_TRUE)
	{
		compile_status_string = "Succeeded";
	}
	p4::Log::Info("%s: Compilation of shader %s.", compile_status_string, file_names[num_files-1]);
	GLint log_length = 0; 
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
	std::string info_log;
	if(log_length > 1)
	{
		GLsizei actual_length = 0;
		info_log.resize(log_length, ' ');
		glGetShaderInfoLog(shader_id, log_length, &actual_length, &info_log[0]);
		info_log.resize(actual_length);
		p4::Log::Info("log:\n%s", info_log.c_str());
	}
	
	if(glCheckAllErrors(__FILE__, __LINE__) || GL_FALSE == compiled_ok)
	{
		p4::Log::Warn("there were errors compiling shader %x, file: %s.", shader_id, file_names[num_files-1] );
		return 1;
	}

	if(!IsShaderAttached(shader_id))
	{
		glAttachShader(mShaderProgram, shader_id);
	}

	if(glCheckAllErrors(__FILE__, __LINE__))
	{
		p4::Log::Warn("there were errors attaching shader to program for shader %x, file: %s.", shader_id, file_names[num_files-1] );
		return 1;
	}
	
	//everything went better than expected :)
	return GL_NO_ERROR;
}

bool cGLShader::IsShaderAttached( GLuint shader_id )
{
	const GLsizei max_shaders = 4;
	GLsizei actual_shaders = 0;
	GLuint shaders[max_shaders];
	for (int i = 0 ; i < max_shaders; ++i)
	{
		shaders[i] = 0;
	}
	glGetAttachedShaders(mShaderProgram, max_shaders, &actual_shaders, shaders);
	for (int i = 0; i < actual_shaders; ++i)
	{
		if(shader_id == shaders[i])
		{
			return true;
		}
	}
	return false;
}

GLint cGLShader::GetShaderType( GLuint shader_id )
{
	GLint shader_type = 0;
	glGetShaderiv(shader_id, GL_SHADER_TYPE, &shader_type);
	if(glCheckAllErrors(__FILE__, __LINE__))
	{
		p4::Log::Warn("there were errors checking the shader type for shader %x", shader_id);
	}
	return shader_type;
}

bool cGLShader::LinkShaderProgram()
{
	glLinkProgram(mShaderProgram);
	int num_errors = glCheckAllErrors(__FILE__, __LINE__);
	if(num_errors >0)
	{
		p4::Log::Warn("there were errors linking the shader program for program %x", mShaderProgram);
	}
	const char* link_status_string = "Failed";
	GLint linked_ok = GL_FALSE;
	glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &linked_ok);
	if(linked_ok == GL_TRUE)
	{
		link_status_string = "Succeeded";
	}
	p4::Log::Info("%s: Linking of shader.", link_status_string);

	GLint log_length = 0; 
	glGetProgramiv(mShaderProgram, GL_INFO_LOG_LENGTH, &log_length);
	std::string info_log;
	if(log_length > 1)
	{
		GLsizei actual_length = 0;
		info_log.resize(log_length, ' ');
		glGetProgramInfoLog(mShaderProgram, log_length, &actual_length, &info_log[0]);
		info_log.resize(actual_length);
		p4::Log::Info("log:\n%s", info_log.c_str());
	}

	mProjectionUniform = glGetUniformLocation(mShaderProgram, "uProjectionMatrix");
	mModelViewUniform = glGetUniformLocation(mShaderProgram, "uModelViewMatrix");
	mNormalMatrixUniform =glGetUniformLocation(mShaderProgram, "uNormalMatrix");
	
	P4_ASSERT(num_errors==0);

	return linked_ok == GL_TRUE;
}

bool cGLShader::Use() const
{
	GLint is_linked = GL_FALSE;
	glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &is_linked);
	P4_ASSERT(is_linked);

	glUseProgram(mShaderProgram);
	if(glCheckAllErrors(__FILE__, __LINE__))
	{
		p4::Log::Warn("there were errors when trying to use the shader program\n");
		return false;
	}
	return true;
}

GLint cGLShader::GetUniformLocation( const char * uniform_name ) const
{
	return glGetUniformLocation(mShaderProgram, uniform_name);
}

void cGLShader::BindUniformToInt( const char* uniform_name, int value ) const
{
	Use();
	GLint uniform_location = GetUniformLocation(uniform_name);
	glUniform1i(uniform_location, value);
	if(glCheckAllErrors(__FILE__,__LINE__)){
		p4::Log::Debug("failed to bind %s, value %d", uniform_name, value);
	}
	
}

void cGLShader::BindUniformToFloat( const char* uniform_name, float value ) const
{
	Use();
	GLint uniform_location = GetUniformLocation(uniform_name);
	glUniform1f(uniform_location, value);
	glCheckAllErrors(__FILE__,__LINE__);
}

void cGLShader::BindUniformToVec4Array( const char* uniform_name,const float* first_element, int num_elements ) const
{
	Use();
	GLint uniform_location = GetUniformLocation(uniform_name);
	glUniform4fv(uniform_location, num_elements, first_element);
	glCheckAllErrors(__FILE__,__LINE__);
}

void cGLShader::BindUniformToVec2( const char* uniform_name, float v1, float v2 ) const 
{
	Use();
	GLint uniform_location = GetUniformLocation(uniform_name);
	glUniform2f(uniform_location, v1, v2);
	glCheckAllErrors(__FILE__,__LINE__);
}
void cGLShader::BindUniformToVec4( const char* uniform_name,const cCoord4& val  ) const
{
	Use();
	GLint uniform_location = GetUniformLocation(uniform_name);
	glUniform4f(uniform_location, val.x, val.y, val.z, val.w);
	glCheckAllErrors(__FILE__,__LINE__);
}

void cGLShader::BindUniformToVec4( const char* uniform_name, float v1, float v2, float v3, float v4  ) const
{
	Use();
	GLint uniform_location = GetUniformLocation(uniform_name);
	glUniform4f(uniform_location, v1, v2, v3, v4);
	
	glCheckAllErrors(__FILE__,__LINE__);
}

void cGLShader::BindUniformToVec4Array( const char* uniform_name,const std::vector<cCoord4>& vec ) const
{
	Use();
	GLint uniform_location = GetUniformLocation(uniform_name);
	glUniform4fv(uniform_location, static_cast<GLsizei>(vec.size()), reinterpret_cast<const GLfloat*>(&vec.at(0)));
}
void cGLShader::BindUniformToVec3( const char* uniform_name, const cCoord3& val ) const
{
	Use();
	GLint uniform_location = GetUniformLocation(uniform_name);
	glUniform3f(uniform_location, val.x, val.y, val.z);
	glCheckAllErrors(__FILE__,__LINE__);
}

void cGLShader::BindProjectionMatrix() const
{
	Use();
	if(mProjectionUniform < 0){
		return;
	}
	GLfloat projection_matrix[16];
	glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix);
	glUniformMatrix4fv(mProjectionUniform, 1, GL_FALSE, projection_matrix );
	glCheckAllErrors(__FILE__,__LINE__);
}
void cGLShader::BindModelViewMatrix() const
{
	Use();
	if(mModelViewUniform < 0){
		return;
	}
	GLfloat modelview_matrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);
	glUniformMatrix4fv(mModelViewUniform, 1, GL_FALSE, modelview_matrix );
	glCheckAllErrors(__FILE__,__LINE__);
}
void cGLShader::BindNormalMatrix() const
{
	Use();
	if(mNormalMatrixUniform < 0){
		return;
	}
	GLfloat normal_matrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, normal_matrix);
	InverseTranspose4f(normal_matrix);
	glUniformMatrix4fv(mNormalMatrixUniform, 1, GL_FALSE, normal_matrix );
	glCheckAllErrors(__FILE__,__LINE__);
}
bool cGLShader::IsBound() const
{
	GLint shader_program = -1;
	glGetIntegerv(GL_CURRENT_PROGRAM, &shader_program);
	return shader_program >=0 && shader_program == mShaderProgram;
}


const GLfloat cGLCube::kCubeVerts[108] =
{
	 0.5f, -0.5f, -0.5f, //0
	 0.5f, -0.5f,  0.5f, //1
	 0.5f,  0.5f, -0.5f, //2
	 0.5f,  0.5f, -0.5f, //2
	 0.5f, -0.5f,  0.5f, //1
	 0.5f,  0.5f,  0.5f, //3

	 0.5f,  0.5f, -0.5f, //2
	 0.5f,  0.5f,  0.5f, //3
	-0.5f,  0.5f, -0.5f, //6
	-0.5f,  0.5f, -0.5f, //6
	 0.5f,  0.5f,  0.5f, //3
	-0.5f,  0.5f,  0.5f, //7

	 0.5f, -0.5f,  0.5f, //1
	-0.5f, -0.5f,  0.5f, //5
	 0.5f,  0.5f,  0.5f, //3
	 0.5f,  0.5f,  0.5f, //3
	-0.5f, -0.5f,  0.5f, //5
	-0.5f,  0.5f,  0.5f, //7

	 0.5f, -0.5f, -0.5f, //0
	-0.5f, -0.5f, -0.5f, //4
	 0.5f, -0.5f,  0.5f, //1
	 0.5f, -0.5f,  0.5f, //1
	-0.5f, -0.5f, -0.5f, //4
	-0.5f, -0.5f,  0.5f, //5

	 0.5f,  0.5f, -0.5f, //2
	-0.5f,  0.5f, -0.5f, //6
	 0.5f, -0.5f, -0.5f, //0
	 0.5f, -0.5f, -0.5f, //0
	-0.5f,  0.5f, -0.5f, //6
	-0.5f, -0.5f, -0.5f, //4
	
	-0.5f, -0.5f, -0.5f, //4
	-0.5f, -0.5f,  0.5f, //5
	-0.5f,  0.5f, -0.5f, //6
	-0.5f,  0.5f, -0.5f, //6
	-0.5f, -0.5f,  0.5f, //5
	-0.5f,  0.5f,  0.5f  //7
};

const GLfloat cGLCube::kCubeNormals[108] = 
{
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,

	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,

	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,

	0.0f, -1.0f,  0.0f,
	0.0f, -1.0f,  0.0f,
	0.0f, -1.0f,  0.0f,
	0.0f, -1.0f,  0.0f,
	0.0f, -1.0f,  0.0f,
	0.0f, -1.0f,  0.0f,

	0.0f,  0.0f, -1.0f,
	0.0f,  0.0f, -1.0f,
	0.0f,  0.0f, -1.0f,
	0.0f,  0.0f, -1.0f,
	0.0f,  0.0f, -1.0f,
	0.0f,  0.0f, -1.0f,

	-1.0f,  0.0f,  0.0f,
	-1.0f,  0.0f,  0.0f,
	-1.0f,  0.0f,  0.0f,
	-1.0f,  0.0f,  0.0f,
	-1.0f,  0.0f,  0.0f,
	-1.0f,  0.0f,  0.0f
};

const GLfloat cGLCube::kCubeTexCoords[108] = 
{
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,

	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,

	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,

	-1.0f,  0.0f,  0.0f,
	-1.0f,  0.0f,  0.0f,
	-1.0f,  0.0f,  0.0f,
	-1.0f,  0.0f,  0.0f,
	-1.0f,  0.0f,  0.0f,
	-1.0f,  0.0f,  0.0f,

	0.0f,  -1.0f,  0.0f,
	0.0f,  -1.0f,  0.0f,
	0.0f,  -1.0f,  0.0f,
	0.0f,  -1.0f,  0.0f,
	0.0f,  -1.0f,  0.0f,
	0.0f,  -1.0f,  0.0f,

	0.0f,  0.0f,  -1.0f,
	0.0f,  0.0f,  -1.0f,
	0.0f,  0.0f,  -1.0f,
	0.0f,  0.0f,  -1.0f,
	0.0f,  0.0f,  -1.0f,
	0.0f,  0.0f,  -1.0f
};
const GLfloat kBillBoardXVerts[54] =
{
	0.0f, -0.5f, -0.5f, //0
	0.0f, -0.5f,  0.5f, //1
	0.0f,  0.5f, -0.5f, //2
	0.0f,  0.5f, -0.5f, //2
	0.0f, -0.5f,  0.5f, //1
	0.0f,  0.5f,  0.5f, //3

	0.5f,  0.0f, -0.5f, //2
	0.5f,  0.0f,  0.5f, //3
	-0.5f,  0.0f, -0.5f, //6
	-0.5f,  0.0f, -0.5f, //6
	0.5f,  0.0f,  0.5f, //3
	-0.5f,  0.0f,  0.5f, //7

	0.5f, -0.5f,  0.0f, //1
	-0.5f, -0.5f,  0.0f, //5
	0.5f,  0.5f,  0.0f, //3
	0.5f,  0.5f,  0.0f, //3
	-0.5f, -0.5f,  0.0f, //5
	-0.5f,  0.5f,  0.0f, //7
};
const GLfloat kBillBoardXTex[54] = 
{
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,

	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,

	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f
};
const GLfloat kBillBoardXNormals[54] = 
{
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,
	1.0f,  0.0f,  0.0f,

	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  0.0f,

	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f,
	0.0f,  0.0f,  1.0f
};
void cGLTexture::CreateTexture2D( const char* file_name, bool is_srgb /*= false*/, int mip_levels /*= -1*/ )
{
	cImageData image(file_name);
	if(NULL != image.Data())
	{
		GLint tex_unit_index;
		glGetIntegerv(GL_ACTIVE_TEXTURE, &tex_unit_index);
		tex_unit_index -= GL_TEXTURE0;
		glBindTexture(GL_TEXTURE_2D, mObjectID);
		if(glCheckAllErrors(__FILE__,__LINE__) > 0)
		{
			return;
		}
		mBoundTextureUnit = tex_unit_index;
		p4::Log::Info("loading image %s ", file_name);
		int bpp = image.BytesPerPixel();
		P4_ASSERT(bpp == 3 || bpp == 4 || bpp ==1);
		GLint internal_format = bpp;
		GLint my_format = bpp == 4 ? GL_RGBA : (( bpp == 3 ) ? GL_RGB : GL_RED);
		if(is_srgb)
		{
			internal_format = bpp == 4 ? GL_SRGB8_ALPHA8: (( bpp == 3 ) ? GL_SRGB8 : GL_R8);
		}
		else
		{
			internal_format = bpp == 4 ? GL_RGBA8: (( bpp == 3 ) ? GL_RGB8 : GL_R8);
		}

		if(mip_levels < 0)
		{
			const int largest_side = image.Width() > image.Height() ? image.Width() : image.Height();
			mip_levels = static_cast<int>(floor(log(largest_side)/kLogOf2));
		}
		if(mip_levels > 0)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mip_levels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		p4::Log::Info("Size = %dx%d, channels = %d", image.Width(), image.Height(), image.BytesPerPixel() );
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, 
			image.Width(), image.Height(), 0,
			my_format, GL_UNSIGNED_BYTE, image.Data());
		if(glCheckAllErrors(__FILE__, __LINE__))
		{
			p4::Log::Warn("there were errors when initializing new 2D texture %s", image.FileName());
		}
		else
		{
			for (int i = 1; i <= mip_levels; i++)
			{
				image.GenerateMipData(i);
				glTexImage2D(GL_TEXTURE_2D, i, internal_format, 
					image.MipWidth(), image.MipHeight(), 0,
					my_format, GL_UNSIGNED_BYTE, image.MipData());
			}
			if(glCheckAllErrors(__FILE__, __LINE__))
			{
				p4::Log::Warn("there were errors when initializing new 2D texture %s", image.FileName());
			}
			else
			{
				p4::Log::Info("done");
			}
		}
		glActiveTexture(GL_TEXTURE0 + tex_unit_index + 1);
	}
}

void cGLTexture::CreateCubeMap( const char* base_name, bool is_srgb /*= false*/ )
{
	static const char* prefixes[6] = {
		"posx_",
		"negx_",
		"posy_",
		"negy_",
		"posz_",
		"negz_"
	}; 
	static const GLint locations[6] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};
	GLint tex_unit_index;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &tex_unit_index);
	tex_unit_index -= GL_TEXTURE0;
	glBindTexture(GL_TEXTURE_CUBE_MAP, mObjectID);
	if(glCheckAllErrors(__FILE__,__LINE__) > 0)
	{
		return;
	}
	mBoundTextureUnit = tex_unit_index;
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	for (int i = 0; i < 6; i++)
	{
		std::string file_name(prefixes[i]);
		file_name.append(base_name);
		cImageData image(file_name.c_str());
		p4::Log::Info("loading image %s ", file_name.c_str());
		int bpp = image.BytesPerPixel();
		GLint internal_format = bpp;
		GLint my_format = bpp == 4 ? GL_RGBA : GL_RGB;
		if(is_srgb)
		{
			internal_format = bpp == 4 ? GL_SRGB8_ALPHA8 : GL_SRGB8;
		}
		else
		{
			internal_format = bpp == 4 ? GL_RGBA8 : GL_RGB8;
		}
		const int largest_side = image.Width() > image.Height() ? image.Width() : image.Height();
		const int mip_levels = static_cast<int>(floor(log(largest_side)/kLogOf2));
		p4::Log::Info("Size = %dx%d, channels = %d", image.Width(), image.Height(), image.BytesPerPixel() );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mip_levels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		glTexImage2D(locations[i], 0, internal_format, 
			image.Width(), image.Height(), 0,
			my_format, GL_UNSIGNED_BYTE, image.Data());
		if(glCheckAllErrors(__FILE__, __LINE__))
		{
			p4::Log::Warn("there were errors when initializing new 2D texture %s", image.FileName());
		}
		else
		{
			for (int mip = 1; mip <= mip_levels; mip++)
			{
				image.GenerateMipData(mip);
				glTexImage2D(locations[i], mip, internal_format, 
					image.MipWidth(), image.MipHeight(), 0,
					my_format, GL_UNSIGNED_BYTE, image.MipData());
			}
			if(glCheckAllErrors(__FILE__, __LINE__))
			{
				p4::Log::Warn("there were errors when initializing new 2D texture %s", image.FileName());
			}
			else
			{
				p4::Log::Info("done");
			} 
		}
	}
	glActiveTexture(GL_TEXTURE0 + tex_unit_index + 1);
}





cGLTexture::cGLTexture(const char* object_name):cGLObject(object_name),mBoundTextureUnit(-1)
{
	glGenTextures(1, &mObjectID);
	glCheckAllErrors(__FILE__, __LINE__);
}
cGLTexture::cGLTexture(const char* object_prefix, const char* object_name)
	: cGLObject(object_prefix, object_name)
	, mBoundTextureUnit(-1)
{
	glGenTextures(1, &mObjectID);
	glCheckAllErrors(__FILE__, __LINE__);
}
cGLTexture::~cGLTexture()
{
	glDeleteTextures(1, &mObjectID);
}

void cGLTexture::CreateEmptyTexture2D( int width, int height, GLint internal_format )
{
	GLint tex_unit_index;
	GLint currently_bound = GL_INVALID_VALUE;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currently_bound);
	glGetIntegerv(GL_ACTIVE_TEXTURE, &tex_unit_index);
	tex_unit_index -= GL_TEXTURE0;
	glBindTexture(GL_TEXTURE_2D, mObjectID);
	if(glCheckAllErrors(__FILE__,__LINE__) > 0)
	{
		return;
	}
	mBoundTextureUnit = tex_unit_index;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	if(glCheckAllErrors(__FILE__, __LINE__))
	{
		p4::Log::Warn("there were errors when initializing an empty 2D texture");
	}

	glActiveTexture(GL_TEXTURE0 + tex_unit_index + 1);
}

void cGLTexture::Bind() const
{
	if(mBoundTextureUnit != -1)
	{
		return;
	}
}

void cGLTexture::SetWrapMode( GLuint sval, GLuint tval ) const
{
	glBindTexture(GL_TEXTURE_2D, mObjectID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sval);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tval);
}

void cGLTexture::SetInterpMode( GLuint shrink_mode, GLuint grow_mode ) const
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, grow_mode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, shrink_mode);
}


cGLArrayBuffer::cGLArrayBuffer( const char* buffer_name )
	: cGLObject(buffer_name)
{
	glGenBuffers(1, &mObjectID);
	glCheckAllErrors(__FILE__, __LINE__);
}
cGLArrayBuffer::cGLArrayBuffer(const char* object_prefix, const char* object_name)
	: cGLObject(object_prefix, object_name)
{
	glGenBuffers(1, &mObjectID);
	glCheckAllErrors(__FILE__, __LINE__);
}
cGLArrayBuffer::cGLArrayBuffer( const char* buffer_name, size_t bytes, const void* data, GLenum usage )
	: cGLObject(buffer_name)
{
	glGenBuffers(1, &mObjectID);
	glCheckAllErrors(__FILE__, __LINE__);
	InitBuffer(bytes, data, usage);
}

cGLArrayBuffer::cGLArrayBuffer( const char* buffer_name, size_t bytes, GLenum usage )
	: cGLObject(buffer_name)
{
	glGenBuffers(1, &mObjectID);
	glCheckAllErrors(__FILE__, __LINE__);
	InitBuffer(bytes, NULL, usage);
}

cGLArrayBuffer::~cGLArrayBuffer()
{
	glDeleteBuffers(1, &mObjectID);
}

void cGLArrayBuffer::InitBuffer( size_t bytes, const void* data, GLenum usage )
{
	Bind();
	glBufferData(GL_ARRAY_BUFFER, bytes, data, usage);
	glCheckAllErrors(__FILE__, __LINE__);
}

void cGLArrayBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, mObjectID);
}

void cGLArrayBuffer::SetData( size_t bytes, const void* data, size_t start_byte )
{
	Bind();
	glBufferSubData(GL_ARRAY_BUFFER, start_byte, bytes, data);
	glCheckAllErrors(__FILE__, __LINE__);
}

void cGLArrayBuffer::ResetData(size_t size )
{
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW );
}

void* cGLArrayBuffer::MapBuffer()
{
	Bind();
	void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	P4_ASSERT(ptr != NULL);
	return ptr;
}

void cGLArrayBuffer::UnMapBuffer()
{

}


cGLCube::cGLCube()
	:cGLObject("GL cube")
	,mVertexBuffer("cube vertex buffer",sizeof(kCubeVerts)+sizeof(kCubeTexCoords)+sizeof(kCubeNormals), GL_STATIC_DRAW)
{
	mVertexBuffer.SetData(sizeof(kCubeVerts), kCubeVerts, 0);
	mVertexBuffer.SetData(sizeof(kCubeTexCoords), kCubeTexCoords, sizeof(kCubeVerts));
	mVertexBuffer.SetData(sizeof(kCubeNormals), kCubeNormals, sizeof(kCubeVerts)+sizeof(kCubeTexCoords));
}
void cGLCube::Bind() const
{
	mVertexBuffer.Bind();
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glTexCoordPointer(3, GL_FLOAT, 0, reinterpret_cast<GLvoid*>(sizeof(kCubeVerts)));
	glNormalPointer( GL_FLOAT, 0, reinterpret_cast<GLvoid*>(sizeof(kCubeVerts)+sizeof(kCubeTexCoords)));
}
void cGLCube::Draw() 
{
	Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
}


cGLStaticObjectBuffer::cGLStaticObjectBuffer( const char* buffer_name, std::vector<const cVertexArray*> object_list )
	:cGLObject(buffer_name)
	,mObjectList(object_list)
	,mVertexBuffer("object buffer: ", buffer_name)
{
	mIndexList.reserve(object_list.size());
	size_t total_triangles = 0;
	size_t total_floats = 0;
	for (const cVertexArray* vertex_array: object_list)
	{
		mIndexList.push_back(static_cast<int>(total_triangles));
		total_triangles += vertex_array->NumVerts()/3;
		total_floats += vertex_array->NumVerts()*vertex_array->FloatsPerVert();
	}
	std::vector<float> temp_vector(total_floats);
	size_t current_index = 0;
	auto temp_iter = temp_vector.begin();
	for (const cVertexArray* vertex_array: object_list)
	{
		size_t num_floats = vertex_array->NumVerts()*vertex_array->FloatsPerVert();

		for (auto ifloat = vertex_array->begin(); ifloat != vertex_array->end(); ++ifloat)
		{
			*temp_iter = *ifloat;
			++temp_iter;
		}
	}
	mVertexBuffer.InitBuffer(total_floats*sizeof(float), &temp_vector.at(0), GL_STATIC_DRAW );

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}

cGLStaticObjectBuffer::~cGLStaticObjectBuffer()
{

}

void cGLStaticObjectBuffer::Bind() const
{
	mVertexBuffer.Bind();
	
}

int cGLStaticObjectBuffer::FindObjectIndex( const char* name )
{
	//TODO: this
	auto index_iter = mIndexList.begin();
	for(const cVertexArray* mesh: mObjectList){
		if(strcmp(mesh->GetName(),name) == 0){
			return *index_iter;
		}
		index_iter++;
	}
	return -1;
}

}//end namespace akj
