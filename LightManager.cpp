#include "LightManager.hpp"
#include "akj_ogl.h"

cLightManager::cLightManager(void)
	:mAmbientLight(0.0f)
{
	mBlueLightVec.reserve(4);
	mOrangeLightVec.reserve(4);
}


cLightManager::~cLightManager(void)
{
}

void cLightManager::SetLights(const akj::cGLShader* shader ) const
{
	shader->BindUniformToVec4("uAmbient", mAmbientLight);
	shader->BindUniformToVec4("uPointerLoc", mArrowLight);
	shader->BindUniformToVec4Array("uOrangeLights", mOrangeLightVec);
	shader->BindUniformToVec4Array("uBlueLights", mBlueLightVec);
}
