#pragma once
#include "cs314Interfaces.h"
#include "VertexArray.hpp"
#include "akj_ogl.h"
class cPlayer;
class cLightManager;
class cDrawableObject: public iDrawable
{
public:
	enum eObjectState{
		DRAWABLE_DEFAULT,
		DRAWABLE_INVISIBLE
	};
	typedef std::vector<const akj::cGLTexture*> tTextureList;
	cDrawableObject(const akj::cVertexArray& mesh,
					const akj::cGLShader& shader,
					const tTextureList& textures,
					const akj::cGLArrayBuffer& vbo,
					int vbo_index);
	~cDrawableObject(void);
	virtual void Draw();
	void TransformModelView();
	cUnitQuat Orientation();
	cCoord3 Position(){
		return mPosition;
	}
	void SetVisibility(bool is_visible){
		mIsVisible = is_visible;
	}
	void SetPlayer(const cPlayer* player){
		mPlayer = player;
	}
	void SetLights(cLightManager* lighting){
		mLightManager = lighting;
	}
	void SetDepthFunction(const GLuint val){
		mDepthFunction = val; 
	}
	const akj::cGLShader& GetShader() const {
		return mShader; 
	}
	cCoord3 GetPosition() const{
		return mPosition;
	}
	void SetOrientation(cUnitQuat rot){
		mOrientation = rot;
	}
	void FaceTowards(cCoord3 point){
		cCoord3 vec = mPosition - point;
		mOrientation = cUnitQuat(cCoord3::ZAxis(),-vec);
	}
	void RotateToFace(float rotation_radians){
		
		
		mOrientation *= cUnitQuat(cAngleAxis(rotation_radians, cCoord3::YAxis()));
		
		
	}
	void SetOrientation(cCoord3 direction){
		mOrientation = cUnitQuat(cCoord3::YAxis(), direction);
	}
	void MultiplyOrientation(cUnitQuat q){
		mOrientation *= q;
	}
	void SetPosition(const cCoord3& pos){
		mPosition = pos;
	}
	void SetScale(const cCoord3& scale){
		mScale = scale;
	}
private:
	cCoord3 mPosition;
	cUnitQuat mOrientation;
	cCoord3 mScale;
	const cPlayer* mPlayer;
 	const cLightManager* mLightManager;
	const akj::cVertexArray& mMesh;
	const akj::cGLShader& mShader;

	GLuint mDepthFunction;
	const GLuint DepthFunction() const { return mDepthFunction; }
	
	const akj::cGLArrayBuffer& mVBO;
	int mStartingVBOIndex;
	tTextureList mTextures;
	bool mIsVisible;

};

