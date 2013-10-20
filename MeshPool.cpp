#include "MeshPool.hpp"
#include "objreader.h"
#include "cs314_p4_log.hpp"

cMeshPool::cMeshPool(void)
{
}

cMeshPool::~cMeshPool(void)
{
}

void cMeshPool::LoadObjFile( const char* file_name, const char* prefix )
{
	
	cs314::cObjReader reader(file_name);
	//reader.resizeObject();
	akj::cVertexArray* mesh_ptr = NULL;
	while((mesh_ptr = reader.GetNextVertexArray(prefix)) != NULL){
		p4::Log::Debug("Loaded object \"%s\" from file %f",mesh_ptr->GetName(), file_name);
		mMeshes.insert(std::make_pair(std::string(mesh_ptr->GetName()), std::move(std::unique_ptr<akj::cVertexArray>(mesh_ptr))));
	}

}

akj::cVertexArray* cMeshPool::GetMesh( const char* mesh_name ) const
{
	auto iter = mMeshes.find(std::string(mesh_name));
	if(iter == mMeshes.end()){
		return NULL;
	}
	return iter->second.get();
}
