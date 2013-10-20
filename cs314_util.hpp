#ifndef CS314_UTIL_HPP
#define CS314_UTIL_HPP
#include <string>
#include "cs314_math.hpp"
#include "cs314_p4_log.hpp"




void InterpToArray(float* out, int num_elements, float start_val, float end_val);
void GerpToArray(float* out, int num_elements, float start_val, float end_val, float factor);

int FileToString(const char* filename, std::string& out_contents);

class cImageData
{
public:
	cImageData();
	cImageData(const char* file_name, int bpp = 0);
	~cImageData();
	//leave bpp at 0 to set it automatically
	void LoadFile(const char* file_name, int bpp = 0);
	unsigned char* Data() {return mDataPointer;}
	unsigned char* MipData() {return mMipDataPointer;}
	void GenerateMipData(int level);
	
	inline int MipWidth() const { return GreaterOf(mWidth >> mMipLevel, 1); }
	inline int MipHeight() const { return GreaterOf(mHeight >> mMipLevel, 1); }

	inline int Width() const { return mWidth; }
	inline int Height() const { return mHeight; }
	inline int BytesPerPixel() const { return mBytesPerPixel; }
	inline const char* FileName() const { return mFileName; }

private:
	unsigned char* mDataPointer;
	unsigned char* mMipDataPointer;
	const char* mFileName;
	int mMipLevel;

	int mWidth;
	int mHeight;
	int mBytesPerPixel;
};



#endif