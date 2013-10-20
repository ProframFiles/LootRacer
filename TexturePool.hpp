#pragma once
#include "akj_ogl.h"
#include <string>
#include <unordered_map>

class cTexturePool
{
public:
	cTexturePool(void);
	~cTexturePool(void);
	akj::cGLTexture* LoadTexture( const char* texture_name, const char* file_name, bool is_srgb = false );
	akj::cGLTexture* LoadCubeMap( const char* texture_name, const char* base_name, bool is_srgb = false );
	akj::cGLTexture* GetTexture( const char* texture_name ) const;
private:
	std::unordered_map<std::string, akj::cGLTexture* > mTextureHandles;
	std::unordered_map<std::string, std::unique_ptr<akj::cGLTexture> > mTextures;
};

