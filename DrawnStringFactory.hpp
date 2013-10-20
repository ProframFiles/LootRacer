#pragma once
#include <memory>
#include <string>
#include <vector>
#include <list>
#include "TextVertex.hpp"
#include "cs314_timing.hpp"
#include "cs314_math.hpp"
#include "DrawnString.hpp"

namespace akj{
	class SignedDistanceFieldFont;
	class cGLArrayBuffer;
	class cGLTexture;
	class cGLShader;
}

typedef std::unique_ptr<cDrawnString> tStringHandle;
class cDrawnStringFactory
{
public:
	cDrawnStringFactory(const akj::cGLShader* shader);
	~cDrawnStringFactory(void);
	void LoadFont(const std::string& font_file,const akj::cGLTexture* font_texture, const akj::cGLTexture* profile_texture  );
	std::unique_ptr<cDrawnString> CreateDrawnString(const std::string& text, cCoord2 pxy, cCoord3 color){
		cDrawnStringInternal* factory_string = CreateDrawnStringImpl(text, pxy, color, cCoord2(0.0f, 0.0f), cCoord2(0.0f, 0.0f));
		cDrawnString* ptr = new cDrawnString(factory_string, this,color, pxy);
		return std::unique_ptr<cDrawnString>( ptr);
	}
	void Draw();
	friend class cDrawnString;
private:
	cDrawnStringInternal* CreateDrawnStringImpl(const std::string& text, cCoord2 pxy, cCoord3 color, cCoord2 vxy, cCoord2 axy);
	void FillVertices(std::string ss,float  x_scale, float y_scale);
	void UpdateVBO();
	void UpdateActiveStrings();
	void ScaleString(cDrawnStringInternal* s, cCoord2 scale);
	std::unique_ptr<akj::SignedDistanceFieldFont> mCurrentFont;
	const akj::cGLTexture* mCurrentFontTexture;
	const akj::cGLTexture* mCurrentProfileTexture;
	const akj::cGLShader* mShader;
	std::unique_ptr<akj::cGLArrayBuffer> mVBO1;
	std::unique_ptr<akj::cGLArrayBuffer> mVBO2;
	akj::cGLArrayBuffer* mCurrentVBO;

	std::vector<cTextVertex> mVertexStore1;
	std::vector<cTextVertex> mVertexStore2;
	std::vector<cTextVertex>* mVertices;
	std::list<cDrawnStringInternal> mActiveStrings;
	bool mNeedsVBORemap;
	cStopWatch mTimer;
	const int kMaxNumChars;
};

