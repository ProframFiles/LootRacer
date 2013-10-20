#pragma once
#include "cs314_math.hpp"
#include <vector>
namespace akj{
	class cGLShader;
}
class cLightManager
{
public:
	cLightManager(void);
	~cLightManager(void);
	void SetLights(const akj::cGLShader* shader) const;
	cCoord4 mArrowLight;
	cCoord4 mAmbientLight;
	std::vector<cCoord4> mOrangeLightVec;
	std::vector<cCoord4> mBlueLightVec;
};

