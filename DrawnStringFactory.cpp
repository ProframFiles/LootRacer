#include "DrawnStringFactory.hpp"
#include "akjFont.hpp"
#include "cs314_p4_log.hpp"
#include <string>
#include <stdexcept>
#include "akj_ogl.h"
#include "cs314_math.hpp"

cDrawnStringFactory::cDrawnStringFactory(const akj::cGLShader* shader)
	:mNeedsVBORemap(false)
	,mShader(shader)
	,kMaxNumChars(1024)
	,mCurrentFontTexture(NULL)
	,mCurrentProfileTexture(NULL)
{
	P4_ASSERT(shader !=NULL);
	mVertexStore1.reserve(6*kMaxNumChars);
	mVertexStore2.reserve(6*kMaxNumChars);
	mVBO1.reset(new akj::cGLArrayBuffer("String VBO",6*sizeof(cTextVertex)*kMaxNumChars, GL_DYNAMIC_DRAW  ));
	cTextVertex::BindAttributes();
	mVBO2.reset(new akj::cGLArrayBuffer("String VBO",6*sizeof(cTextVertex)*kMaxNumChars, GL_DYNAMIC_DRAW  ));
	mCurrentVBO = mVBO2.get();
	mVertices = &mVertexStore2;
	cTextVertex::BindAttributes();
	mTimer.Start();
}


cDrawnStringFactory::~cDrawnStringFactory(void)
{
}

void cDrawnStringFactory::LoadFont(const std::string& font_file,const akj::cGLTexture* font_texture, const akj::cGLTexture* profile_texture )
{
	akj::SignedDistanceFieldFont* sdf = new akj::SignedDistanceFieldFont();
	P4_ASSERT(sdf != NULL);
	P4_ASSERT(font_texture != NULL);
	P4_ASSERT(profile_texture != NULL);
	mCurrentFont.reset(sdf);
	mCurrentFontTexture = font_texture;
	mCurrentProfileTexture = profile_texture;
	profile_texture->SetWrapMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	font_texture->SetWrapMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	font_texture->SetInterpMode(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

	mCurrentFont->createFromFile(font_file);
	mCurrentFont->freeImageData();
	p4::Log::Debug("font width = %d, height = %d", sdf->getWidth(), sdf->getHeight());
}

void cDrawnStringFactory::FillVertices(std::string ss, float x_scale, float y_scale)
{
	akj::SignedDistanceFieldFont* current_font = mCurrentFont.get();
	char charNow;
	char offSet=32;
	float curX =0.0f;
	float maxHeight=0.0f;
	float maxWidth=0.0f;
	float inv_w= x_scale*(1.0f/current_font->getWidth());
	float inv_h= (1.0f/current_font->getHeight());

	size_t string_length=ss.size();
	akj::kernPair kp;
	float startMeasure=0.0f;
	float offsetX=0.0f;
	float offsetY=0.0f;
	float adv=0.0f;

	//Find the total size of the string
	for(  size_t i = 0; i < string_length; ++i ){
		charNow=ss.at(i)-offSet;

		if (charNow<-1&&charNow!=-offSet){
			throw std::runtime_error("invalid character index");
		}
		if(charNow<0||charNow>=static_cast<int>(current_font->getNumChars())){
			continue;
		}
		maxWidth-=offsetX;
		maxWidth+=0.5f*adv;
		offsetX=current_font->getCharData(charNow).xOffset;
		adv=current_font->getCharData(charNow).xAdvance*inv_w;
		maxWidth += 0.5f*adv+offsetX;
		if(i==0){
			startMeasure=offsetX-adv*0.5f;
		}
		if(current_font->getCharData(charNow).yOffset>maxHeight){
			maxHeight=current_font->getCharData(charNow).yOffset>maxHeight;
		}
		if(i+1<string_length){
			kp.a=charNow+offSet;
			kp.b=ss.at(i+1);
			if(current_font->getKernPair(kp)){
				kp.x*=inv_w;
				maxWidth+=kp.x;
			}
		}
	}
	
	offsetY=0.0f;
	offsetX=0.0f;
	for(  unsigned int i = 0; i < string_length; ++i )
	{
		charNow=ss.at(i)-offSet;
		if(charNow<0||charNow>=static_cast<int>(current_font->getNumChars())){
			continue;
		}
		offsetX = 1.0f*current_font->getCharData(charNow).xOffset*x_scale;
		offsetY = current_font->getCharData(charNow).yOffset;

		float lx =  (curX + offsetX);
		const float ly = (offsetY);

		curX+=current_font->getCharData(charNow).xAdvance*inv_w;
		if(i+1<string_length){
			kp.a=charNow+offSet;
			kp.b=ss.at(i+1);
			if(current_font->getKernPair(kp)){
				kp.x*=inv_w;
				lx+=kp.x;
				curX+=kp.x;
			}
		}


		const int index = current_font->getCharData(charNow).indx;
		const int ID = current_font->getCharData(charNow).ID;
		p4::Log::TMI("char %s, index %d, id%d", charNow, index, ID );
		const akj::SignedDistanceFieldFont::vertexData& vertex = current_font->GetVertex(index);
		const float tu = vertex.xPos;
		const float tv = 1.0f-vertex.yPos;
		const float ws = vertex.width*x_scale;
		const float hs = vertex.height;
		mVertices->emplace_back(lx, ly, tu, tv-vertex.height);
		mVertices->emplace_back(lx+ws, ly, tu+vertex.width, tv-vertex.height);
		mVertices->emplace_back(lx, ly+hs, tu, tv );

		mVertices->emplace_back(lx, ly+hs, tu, tv);
		mVertices->emplace_back(lx+ws, ly, tu+vertex.width, tv-vertex.height);
		mVertices->emplace_back(lx+ws, ly+hs, tu+vertex.width, tv);
		
	}
}

void cDrawnStringFactory::UpdateVBO()
{
	if(!mNeedsVBORemap) return;
	
	std::vector<cTextVertex>* other_verts = mVertices == &mVertexStore1 ? &mVertexStore2 : &mVertexStore1;
	other_verts->clear();
	int vert_index = 0;
	for (cDrawnStringInternal& ds: mActiveStrings)
	{
		int old_index = ds.GetIndex();
		ds.SetIndex(vert_index);
		for (int i = 0; i < ds.GetNumChars()*6; i++)
		{
			other_verts->push_back(mVertices->at(i+old_index));
			vert_index++;
		}
		
	}
	mVertices = other_verts;
	mCurrentVBO = mCurrentVBO == mVBO1.get() ? mVBO2.get() : mVBO1.get();
	mCurrentVBO->ResetData(sizeof(cTextVertex)*mVertices->size());
	mCurrentVBO->SetData(sizeof(cTextVertex)*mVertices->size(), mVertices->data(),0);
	mNeedsVBORemap = false;
}

void cDrawnStringFactory::Draw()
{
	UpdateActiveStrings();
	UpdateVBO();
	mCurrentVBO->Bind();
	cTextVertex::BindAttributes();
	mShader->Use();
	mShader->BindUniformToFloat("uCurrentTime", mTimer.Readf());
	if(mCurrentFontTexture!=NULL && mCurrentProfileTexture != NULL){
		mShader->BindUniformToInt("uTexture0", mCurrentFontTexture->GetBoundTextureUnit());
		mShader->BindUniformToInt("uTexture1", mCurrentProfileTexture->GetBoundTextureUnit());
	}
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, 0.0, 1.0);
	mShader->BindProjectionMatrix();
	//reset model matrix
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	//mShader->BindModelViewMatrix();
	mCurrentVBO->Bind();
	glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(mVertices->size()));
	
	//restore model matrix
	glPopMatrix();

	//restore projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	//go back to model mode
	glMatrixMode(GL_MODELVIEW);
}

cDrawnStringInternal* cDrawnStringFactory::CreateDrawnStringImpl(const std::string& text, cCoord2 pxy, cCoord3 color, cCoord2 vxy, cCoord2 axy )
{
	P4_ASSERT((mVertices->size()/6+text.size()) < static_cast<size_t>(kMaxNumChars));
	int index = mVertices->empty() ? 0 :  static_cast<int>(mVertices->size());
	int num_chars = static_cast<int>(text.size());
	mActiveStrings.emplace_back(num_chars, index);
	FillVertices(text, 1.0f/1.788f, 1.0f);
	float start_time = mTimer.Readf();
	for (auto i = mVertices->begin()+index; i < mVertices->end(); ++i)
	{
		i->mPosition[0] += pxy.x;
		i->mPosition[1] += pxy.y;

		i->mVelocity[0] += vxy.x; 
		i->mVelocity[1] += vxy.y;

		i->mAcceleration[0] += axy.x;
		i->mAcceleration[1] += axy.y;

		i->mColor[0] = color.x;
		i->mColor[1] = color.y;
		i->mColor[2] = color.z;

		i->mStartTime = start_time;
	}
	mNeedsVBORemap = true;
	return &mActiveStrings.back();
}

void cDrawnStringFactory::UpdateActiveStrings()
{
	for (auto i = mActiveStrings.begin(); i != mActiveStrings.end(); ++i)
	{
		if(!i->IsActive()){
			i = mActiveStrings.erase(i);
			mNeedsVBORemap = true;
		}
	}
}

void cDrawnStringFactory::ScaleString( cDrawnStringInternal* s, cCoord2 scale )
{
	int index = s->GetIndex();
	int num_verts= s->GetNumChars()*6;
	const cCoord2 scale_change = cCoord2(scale.x/s->GetScale().x, scale.y/s->GetScale().y);
	if(scale_change.x == 1.0f && scale_change.y == 1.0f){
		return;
	}

	float max_x = -MaxFloat();
	float min_x = MaxFloat();
	float max_y = -MaxFloat();
	float min_y = MaxFloat();
	for (auto i = mVertices->begin()+index; i < mVertices->begin()+index+num_verts; ++i)
	{
		if(i->mPosition[0] > max_x){
			max_x = i->mPosition[0];
		}
		if(i->mPosition[0] < min_x){
			min_x = i->mPosition[0];
		}
		if(i->mPosition[1] > max_y){
			max_y = i->mPosition[1];
		}
		if(i->mPosition[1] < min_y){
			min_y = i->mPosition[1];
		}
	}
	cCoord2 center( (max_x + min_x)*0.5,(max_y + min_y)*0.5 );
	for (auto i = mVertices->begin()+index; i < mVertices->begin()+index+num_verts; ++i)
	{
		i->mPosition[0] = (i->mPosition[0] - center.x)*scale_change.x + center.x;
		i->mPosition[1] = (i->mPosition[1] - center.y)*scale_change.y + center.y;
	}
}
