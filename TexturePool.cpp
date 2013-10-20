#include "TexturePool.hpp"
#include "akj_ogl.h"

cTexturePool::cTexturePool(void)
{
}


cTexturePool::~cTexturePool(void)
{
}

akj::cGLTexture* cTexturePool::LoadTexture(const char* texture_name, const char* file_name, bool is_srgb)
{
	p4::Log::Debug("Loading texture \"%s\" from file %s",texture_name, file_name);
	akj::cGLTexture* bare_texture = NULL;
	if(!mTextures.empty()){
		auto texture_get = mTextures.find(std::string(file_name));
		if(texture_get != mTextures.end()){
			p4::Log::Debug("File %s is already loaded, retrieving", file_name);
			bare_texture = texture_get->second.get();
		}
	}
	if(bare_texture == NULL){
		std::unique_ptr<akj::cGLTexture> unique_texture(new akj::cGLTexture(texture_name));
		unique_texture->CreateTexture2D(file_name, is_srgb);
		bare_texture = unique_texture.get();
		mTextures.insert(std::make_pair(std::string(file_name), std::move(unique_texture)));
	}
	P4_ASSERT(bare_texture != NULL);
	mTextureHandles.insert(std::make_pair(std::string(texture_name), bare_texture));

	return bare_texture;
}

akj::cGLTexture* cTexturePool::LoadCubeMap( const char* texture_name, const char* base_name, bool is_srgb )
{
		p4::Log::Debug("Loading texture \"%s\" from file %s",texture_name, base_name);
	akj::cGLTexture* bare_texture = NULL;
	if(!mTextures.empty()){
		auto texture_get = mTextures.find(std::string(base_name));
		if(texture_get != mTextures.end()){
			p4::Log::Debug("File %s is already loaded, retrieving", base_name);
			bare_texture = texture_get->second.get();
		}
	}
	if(bare_texture == NULL){
		std::unique_ptr<akj::cGLTexture> unique_texture(new akj::cGLTexture(texture_name));
		unique_texture->CreateCubeMap(base_name, is_srgb);
		mTextures.insert(std::make_pair(std::string(base_name), std::move(unique_texture)));
		bare_texture = unique_texture.get();
	}
	
	mTextureHandles.insert(std::make_pair(std::string(texture_name), bare_texture));

	return bare_texture;
}

akj::cGLTexture* cTexturePool::GetTexture( const char* shader_name ) const
{
	auto iter = mTextureHandles.find(std::string(shader_name));
	if(iter == mTextureHandles.end()){
		return NULL;
	}
	return iter->second;
}


