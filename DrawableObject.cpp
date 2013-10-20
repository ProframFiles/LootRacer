#include "DrawableObject.hpp"
#include "Player.hpp"
#include "LightManager.hpp"

cDrawableObject::cDrawableObject(const akj::cVertexArray& mesh,
								 const akj::cGLShader& shader,
								 const tTextureList& textures,
								 const akj::cGLArrayBuffer& vbo,
								 int vbo_index)
		: mMesh(mesh)
		, mShader(shader)
		, mTextures(textures)
		, mVBO(vbo)
		, mStartingVBOIndex(vbo_index)
		, mIsVisible(true)
		, mScale(1.0f)
		, mPlayer(NULL)
		, mLightManager(NULL)
		, mDepthFunction(GL_LEQUAL)
	{
		P4_ASSERT(mStartingVBOIndex >= 0);
		p4::Log::Debug("Constructed drawable using mesh \"%s\" in VBO \"%s\"",mesh.GetName(), vbo.GetName());
		p4::Log::Debug("...with %d triangles, starting at index %d in the VBO",(mesh.NumVerts()/3), mStartingVBOIndex);
	}

cDrawableObject::~cDrawableObject(void)
{
}

void cDrawableObject::Draw()
{
	if(!mIsVisible){
		return;
	}
	if(!mShader.IsBound()){
		mShader.Use();
		mShader.BindProjectionMatrix();
	}
	glDepthFunc(mDepthFunction);
	glPushMatrix();
	TransformModelView();
	mShader.BindModelViewMatrix();
	mShader.BindNormalMatrix();

	if(mPlayer != NULL){
		mShader.BindUniformToVec4("uPlayerPosition", cCoord4(mPlayer->GetPosition(),mPlayer->GetGroundDistance()));
	}
	if(mLightManager != NULL){
		mLightManager->SetLights(&mShader);
	}

	// bind textures
	char textureString[] = "uTexture#";
	int number_index = static_cast<int>(strlen(textureString)-1);
	P4_ASSERT(mTextures.size() < 10);
	int num_textures =  static_cast<int>(mTextures.size());
	for(int i = 0; i < num_textures; ++i ){
		textureString[number_index] = static_cast<char>(i + '0');
		mShader.BindUniformToInt(textureString, mTextures.at(i)->GetBoundTextureUnit());
	}

	mVBO.Bind();
	mMesh.SetGLAttributes();
	P4_ASSERT(akj::glCheckAllErrors(__FILE__,__LINE__) == 0);
	glDrawArrays(GL_TRIANGLES, mStartingVBOIndex*3, mMesh.NumVerts());
	glPopMatrix();
}

void cDrawableObject::TransformModelView()
{
	
	akj::gltranslatec3(Position());
	akj::glRotateq(mOrientation);
	akj::glScalec3(mScale);
}
