#include "StaticObjectPool.hpp"
#include "ShaderPool.hpp"
#include "MeshPool.hpp"
#include "TexturePool.hpp"
#include "akj_ogl.h"
#include "DrawableObject.hpp"
#include "VertexArray.hpp"
#include "MeshPool.hpp"
#include <string>


cObjectPool::cObjectPool(	const cMeshPool& mesh_pool, 
										const cShaderPool& shader_pool, 
										const cTexturePool& texture_pool )
	: mMeshPool(mesh_pool)
	, mShaderPool(shader_pool)
	, mTexturePool(texture_pool)
{
	std::vector<const akj::cVertexArray*> meshes;
	for (auto iter = mMeshPool.begin(); iter != mMeshPool.end(); ++iter)
	{
		meshes.push_back(iter->second.get());
	}
	mVBO.reset(new akj::cGLStaticObjectBuffer("Static assets",meshes));
}


cObjectPool::~cObjectPool(void)
{
}
void cObjectPool::CreateDrawnObject( const char* object_name, const char* mesh_name, const char* shader_name, const char* texture_base_name)
{
	std::vector<std::string> suffixes;
	suffixes.push_back(std::string(" dm"));
	suffixes.push_back(std::string(" nm"));
	suffixes.push_back(std::string(" lm"));
	CreateDrawnObject(object_name, mesh_name, shader_name, texture_base_name, suffixes);
}
void cObjectPool::CreateDrawnObject( const char* object_name, const char* mesh_name, const char* shader_name, const char* texture_base_name, std::vector<std::string>& suffixes)

{
	std::vector<const akj::cGLTexture*> tex_vec;
	const akj::cVertexArray* mesh_ptr = mMeshPool.GetMesh(mesh_name);
	P4_ASSERT(mesh_ptr != NULL);
	const akj::cGLShader* shader_ptr = mShaderPool.GetShader(shader_name);
	P4_ASSERT(shader_ptr != NULL);
	std::string name_string(texture_base_name);
	
	for(std::string s: suffixes)
	{
		std::string full_name = name_string;
		full_name.append(s);
		const akj::cGLTexture* texture_ptr = mTexturePool.GetTexture(full_name.c_str());
		if(texture_ptr == NULL){
			std::string default_name = "default";
			default_name.append(s);
			texture_ptr = mTexturePool.GetTexture(default_name.c_str());
		}
		P4_ASSERT(texture_ptr!= NULL);
		tex_vec.push_back(texture_ptr);
	}
	int vbo_index = mVBO->FindObjectIndex(mesh_name);
	std::unique_ptr<cDrawableObject> object(new cDrawableObject(*mesh_ptr, *shader_ptr, tex_vec, *(mVBO->GetArrayBuffer()),vbo_index));

	mObjects.insert(std::make_pair(object_name, std::move(object) ));

}

void cObjectPool::PopulateDrawables( std::vector<iDrawable*>& drawables )
{
	for (auto iter = mObjects.begin(); iter != mObjects.end(); ++iter)
	{
		drawables.push_back(iter->second.get());
	}
}
void cObjectPool::SetPlayer( cPlayer* player )
{
	for (auto iter = mObjects.begin(); iter != mObjects.end(); ++iter)
	{
		iter->second.get()->SetPlayer(player);
	}
}
void cObjectPool::SetLightManager( cLightManager* lighting)
{
	for (auto iter = mObjects.begin(); iter != mObjects.end(); ++iter)
	{
		iter->second.get()->SetLights(lighting);
	}
}
cDrawableObject* cObjectPool::GetDrawableObject( const char* name ) const
{
	cDrawableObject* ret = NULL;
	auto found = mObjects.find(std::string(name));
	if(found != mObjects.end()){
		ret = found->second.get();
	}
	P4_ASSERT(ret != NULL);
	return ret;
}
