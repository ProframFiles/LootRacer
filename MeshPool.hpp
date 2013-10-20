#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "VertexArray.hpp"


class cMeshPool
{
public:
	typedef std::unordered_map<std::string, std::unique_ptr<akj::cVertexArray> > tMeshMap;

	cMeshPool(void);
	~cMeshPool(void);
	void LoadObjFile( const char* fileName,const char* prefix = "" );
	tMeshMap::const_iterator begin() const{
		return mMeshes.begin();
	}
	tMeshMap::const_iterator end() const{
		return mMeshes.end();
	}
	akj::cVertexArray* GetMesh(const char* mesh_name) const;
	
private:
	tMeshMap mMeshes;
};

