#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

namespace akj{
	class cGLShader;
}

class cShaderDescription{
public:
	cShaderDescription(const char* shader_name)
		:mShaderName(shader_name)
	{}
	~cShaderDescription(){}

	std::string GetShaderName() const {
		return mShaderName;
	}

	const char** GetFragmentFiles() {
		if(!mFragmentStrings.empty()){
			return &mFragmentStrings.at(0);
		}
		return NULL;
	}
	int GetNumFragmentStrings() const {
		return static_cast<int>(mFragmentStrings.size());
	}

	const char** GetVertexFiles() {
		if(!mVertexStrings.empty()){
			return &mVertexStrings.at(0);
		}
		return NULL;
	}
	int GetNumVertexStrings() const {
		return static_cast<int>(mVertexStrings.size());
	}

	const char** GetGeometryFiles() {
		if(!mGeometryStrings.empty()){
			return &mGeometryStrings.at(0);
		}
		return NULL;
	}
	int GetNumGeometryStrings() const {
		return static_cast<int>(mGeometryStrings.size());
	}


	cShaderDescription& AddFragmentFile(const char* fileName){
		mFragmentFiles.push_back(std::string(fileName));
		mFragmentStrings.clear();
		for (std::string& s: mFragmentFiles)
		{
			mFragmentStrings.push_back(s.c_str());
		}
		return *this;
	}
	cShaderDescription& AddVertexFile(const char* fileName){
		mVertexFiles.push_back(std::string(fileName));
		mVertexStrings.clear();
		for (std::string& s: mVertexFiles)
		{
			mVertexStrings.push_back(s.c_str());
		}
		return *this;
	}
	cShaderDescription& AddGeometryFile(const char* fileName){
		mGeometryFiles.push_back(std::string(fileName));
		mGeometryStrings.clear();
		for (std::string& s: mGeometryFiles)
		{
			mGeometryStrings.push_back(s.c_str());
		}
		return *this;
	}
	cShaderDescription& Reset(const char* shader_name){
		mShaderName = shader_name;
		mFragmentFiles.clear();
		mVertexFiles.clear();
		mGeometryFiles.clear();
		mFragmentStrings.clear();
		mVertexStrings.clear();
		mGeometryStrings.clear();
		return *this;
	}
private:
	std::string mShaderName;
	std::vector<std::string> mFragmentFiles;
	std::vector<std::string> mVertexFiles;
	std::vector<std::string> mGeometryFiles;
	std::vector<const char*> mFragmentStrings;
	std::vector<const char*> mVertexStrings;
	std::vector<const char*> mGeometryStrings;
};
class cShaderPool
{
	
public:
	cShaderPool(void);
	~cShaderPool(void);
	akj::cGLShader* LoadShader(cShaderDescription shader_desc);
	akj::cGLShader* GetShader(const char* shader_name) const;
	
private:
	std::unordered_map<std::string, std::unique_ptr<akj::cGLShader> > mShaders;

};

