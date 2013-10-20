#pragma once
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>

namespace akj{
	class cVertexArray;
	class cGLShader;
	class cGLArrayBuffer;
	class cGLStaticObjectBuffer;
}
class cDrawableObject;
class iDrawable;
class tTextureList;
class cMeshPool;
class cTexturePool;
class cShaderPool;
class cPlayer;
class cLightManager;

class cObjectPool
{
public:
	cObjectPool(const cMeshPool& mesh_pool, const cShaderPool& shader_pool, const cTexturePool& texture_pool);
	~cObjectPool(void);
	void CreateDrawnObject( const char* object_name, const char* mesh_name, const char* shader_name, const char* texture_base_name, std::vector<std::string>& suffixes);
	void CreateDrawnObject(const char* object_name,	const char* mesh_name, const char* shader_name, const char* texture_base_name);
	void PopulateDrawables(std::vector<iDrawable*>& drawables);
	cDrawableObject* GetDrawableObject( const char* texture_name ) const;
	void SetPlayer( cPlayer* player );
	void SetLightManager( cLightManager* lighting);
private:
	const cMeshPool& mMeshPool;
	const cShaderPool& mShaderPool;
	const cTexturePool& mTexturePool;
	std::unique_ptr<akj::cGLStaticObjectBuffer> mVBO;
	std::unordered_map<std::string, std::unique_ptr<cDrawableObject> > mObjects;
};

