#ifndef AKJ_OGL_H
#define AKJ_OGL_H

#if defined(__APPLE_CC__)
#include <OpenGL/gl.h>
#elif defined(_WIN32)
#include <windows.h>
#include <GL/gl.h>
#include <GL/glext.h>
#else
#include <GL/gl.h>
#include <stdint.h>
#endif
#include "cs314_math.hpp"
#include "cs314_p4_log.hpp"
#include <memory>

#define AKJ_FLOAT_OFFSET(n_)(reinterpret_cast<const void*>((n_)*sizeof(GLfloat)))


#ifdef WIN32
extern PFNGLACTIVETEXTUREPROC glActiveTexture;
extern PFNGLBLENDEQUATIONPROC glBlendEquation;
#endif

extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders;
extern PFNGLISSHADERPROC glIsShader;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLVALIDATEPROGRAMPROC glValidateProgram;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLUNIFORM1IPROC glUniform1i;
extern PFNGLUNIFORM2FPROC glUniform2f;
extern PFNGLUNIFORM3FPROC glUniform3f;
extern PFNGLUNIFORM4FPROC glUniform4f;
extern PFNGLUNIFORM4FVPROC glUniform4fv;
extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
extern PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
extern PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
extern PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
extern PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
extern PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
extern PFNGLUNIFORM1FPROC glUniform1f;
extern PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLBUFFERSUBDATAPROC glBufferSubData;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLMAPBUFFERPROC glMapBuffer;

namespace akj{
	class cVertexArray;

	extern const GLfloat kCubeTexCoords[];
	extern const GLfloat kCubeNormals[]; 
	extern const GLfloat kBillBoardXTex[];
	extern const GLfloat kBillBoardXVerts[];
	extern const GLfloat kBillBoardXNormals[];
	void glInit();
	const char* glErrorStringFromCode(int code);
	int glCheckAllErrors(const char* file, int line);
	void glDrawBillBoardX();
	void glDrawRect2D();
	const GLubyte* glTestTexture();
	GLuint glLoadTexture2D(const char* file_name, bool is_srgb = false, int mip_levels = 0);
	GLuint glCreateCubeMap(const char* base_name, bool is_srgb = false);
	inline void glRotateaa(const cAngleAxis& aa)
	{
		if(aa.angle != 0.0f)
		{
			glRotatef(ToDegrees(aa.angle), aa.x, aa.y, aa.z);
		}
	}
	inline void glRotateq(const cUnitQuat& quaternion)
	{
		glRotateaa(quaternion.to_angle_axis());
	}
	inline void glScalec3(const cCoord3& coord)
	{
		glScalef(coord.x, coord.y, coord.z);
	}
	inline void gltranslatec3(const cCoord3& coord)
	{
		glTranslatef(coord.x, coord.y, coord.z);
	}

	class cGLObject
	{
	public:
		cGLObject(const char* object_name);
		cGLObject(const char* object_prefix, const char* object_name);
		virtual ~cGLObject(){};
		virtual GLuint GetID() const {return mObjectID;};
		virtual const char* GetName() const {return mObjectName.c_str();};
		virtual void Bind() const =0;
	protected:
		GLuint mObjectID;
		std::string mObjectName;
	};

	class cGLTexture : public cGLObject
	{
	public:
		cGLTexture(const char* object_name);
		cGLTexture(const char* object_prefix, const char* object_name);
		~cGLTexture();
		void CreateCubeMap( const char* base_name, bool is_srgb = false );
		void CreateTexture2D( const char* base_name, bool is_srgb = false, int miplevels = -1 );
		void CreateEmptyTexture2D( int width, int height, GLint internal_format );
		void SetWrapMode(GLuint sval, GLuint tval) const;
		GLint GetBoundTextureUnit() const {return mBoundTextureUnit;}
		void Bind() const;
		void SetInterpMode( GLuint shrink_mode, GLuint grow_mode ) const;
	private:
		GLint mBoundTextureUnit;
	};
	class cGLArrayBuffer : public cGLObject
	{
	public:
		cGLArrayBuffer(const char* buffer_name);
		cGLArrayBuffer(const char* object_prefix, const char* object_name);
		cGLArrayBuffer(const char* buffer_name, size_t bytes, const void* data, GLenum usage);
		cGLArrayBuffer(const char* buffer_name, size_t bytes, GLenum usage);
		~cGLArrayBuffer();
		void InitBuffer(size_t bytes, const void* data, GLenum usage);
		void SetData(size_t bytes, const void* data, size_t start_byte);
		void ResetData(size_t data);
		void* MapBuffer();
		void UnMapBuffer();
		void Bind() const;
	private:
		int mBufferSize;
	};
	class cGLStaticObjectBuffer : public cGLObject
	{
	public:
		cGLStaticObjectBuffer(const char* buffer_name, std::vector<const cVertexArray*> object_list );
		~cGLStaticObjectBuffer();
		void Bind() const;
		int FindObjectIndex(const char* name);
		const cGLArrayBuffer* GetArrayBuffer() const {
			return &mVertexBuffer;
		} 
		std::vector<const cVertexArray*>::const_iterator beginVertArrays() const{
			return mObjectList.begin();
		}
		std::vector<int>::const_iterator beginVBOIndices() const{
			return mIndexList.begin();
		}
		std::vector<const cVertexArray*>::const_iterator endVertArrays() const{
			return mObjectList.end();
		}
		std::vector<int>::const_iterator endVBOIndices() const{
			return mIndexList.end();
		}
	private:
		std::vector<const cVertexArray*> mObjectList;
		std::vector<int> mIndexList;
		cGLArrayBuffer mVertexBuffer;
	};
	class cGLCube : public cGLObject
	{
	public:
		cGLCube();
		~cGLCube(){};
		void Draw();
		void Bind() const;
		static const GLfloat kCubeVerts[];
		static const GLfloat kCubeTexCoords[];
		static const GLfloat kCubeNormals[];
	private:
		cGLArrayBuffer mVertexBuffer;
	};
	
	class cGLShader
	{
	public:
		cGLShader();
		~cGLShader();
		bool SetFragmentShader(const char** file_names, int num_files);
		bool SetVertexShader(const char** file_names, int num_files);
		bool LinkShaderProgram();
		bool Use() const;
		void BindProjectionMatrix() const;
		void BindModelViewMatrix() const;
		void BindNormalMatrix() const;
		void BindUniformToVec4Array( const char* uniform_name,const std::vector<cCoord4>& vec ) const;
		void BindUniformToInt(const char* uniform_name, int value) const;
		void BindUniformToFloat(const char* uniform_name, float value) const;
		void BindUniformToVec2(const char* uniform_name, float v1, float v2) const;
		void BindUniformToVec3(const char* uniform_name, const cCoord3& val) const;
		void BindUniformToVec4( const char* uniform_name, float v1, float v2, float v3, float v4 ) const;
		void BindUniformToVec4( const char* uniform_name, const cCoord4& val ) const;
		void BindUniformToQuat( const char* uniform_name,const cUnitQuat& q) const
		{
			BindUniformToVec4(uniform_name, q.s.x, q.s.y, q.s.z, q.w);
		}
		void BindUniformToVec4Array(const char* uniform_name, const float* first_element, int num_elements) const;
		GLint GetUniformLocation(const char * uniform_name) const;
		bool IsBound() const;
		//const char* GetSamplerFileName(const char * uniform_name);
	private:

		bool IsShaderAttached(GLuint shader_id);
		GLint GetShaderType(GLuint shader_id);
		int LoadShaderImpl(GLuint shader_id, const char** file_names, int num_files);
		
		GLuint mShaderProgram;
		GLuint mFragmentShader;
		GLuint mVertexShader;
		GLuint mProjectionUniform;
		GLuint mModelViewUniform;
		GLuint mNormalMatrixUniform;
		const char* mVersionString;
	};
}
#endif
