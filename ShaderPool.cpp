#include "ShaderPool.hpp"
#include "akj_ogl.h"
#include "cs314_p4_log.hpp"


cShaderPool::cShaderPool(void)
{
}


cShaderPool::~cShaderPool(void)
{
}

akj::cGLShader* cShaderPool::LoadShader( cShaderDescription desc )
{
	p4::Log::Debug("Loading shader \"%s\" from files", desc.GetShaderName().c_str());
	std::unique_ptr<akj::cGLShader> unique_shader(new akj::cGLShader());
	akj::cGLShader* bare_shader = unique_shader.get(); 
	P4_ASSERT(bare_shader->SetFragmentShader(desc.GetFragmentFiles(), desc.GetNumFragmentStrings()));
	P4_ASSERT(bare_shader->SetVertexShader(desc.GetVertexFiles(), desc.GetNumVertexStrings()));
	bare_shader->LinkShaderProgram();
	mShaders.insert(std::make_pair(desc.GetShaderName(), std::move(unique_shader)));
	return bare_shader;
}

akj::cGLShader* cShaderPool::GetShader( const char* shader_name ) const
{
	auto iter = mShaders.find(std::string(shader_name));
	if(iter == mShaders.end()){
		return NULL;
	}
	return iter->second.get();
}
