#include "cs314_util.hpp"
#include <stdio.h>
#include "stb_image.h"
#include <stdlib.h>

int FileToString( const char* filename, std::string& out_contents )
{
	P4_ASSERT(filename != NULL);
	FILE *fp;
	fp = fopen(filename,"r");
	if (!fp) 
	{
		p4::Log::Error("Unable to open file '%s' for read into string (%s, %d)\n",filename, __FILE__, __LINE__);
		P4_TERMINATE("file load failed");
	}
	const size_t capacity_increment = 512;
	try
	{
		out_contents.clear();
		out_contents.reserve(capacity_increment);
		for (int this_char = fgetc(fp); this_char != EOF; this_char = fgetc(fp))
		{
			//embiggenate the buffer, if needed
			if(out_contents.capacity() == out_contents.size())
			{
				out_contents.reserve(out_contents.capacity() + capacity_increment);
			}
			out_contents.push_back(static_cast<char>(this_char));
		}
	}
	catch (const std::exception& e)
	{
		p4::Log::Error("caught exception '%s' while reading file '%s' into string (%s, %d)"
			,e.what() == NULL ? "NULL" : e.what() , filename, __FILE__, __LINE__);
		P4_TERMINATE("file load failed");
	}
	return 0;
}

void InterpToArray(float* out, int num_elements, float start_val, float end_val)
{

	for (int i = 0; i < num_elements; ++i)
	{
		const float interp = i*(1.0f/(num_elements-1));
		out[i] = start_val*(1.0f - interp)+ interp*end_val;
	}
}
void GerpToArray(float* out, int num_elements, float start_val, float end_val, float factor)
{
	float weight = 1.0f;
	double sum = 0.0;
	for (int i = 0; i < num_elements; ++i)
	{
		sum += weight;
		weight = weight*factor;
	}
	const float scale = 1.0f/static_cast<float>(sum);
	weight = 1.0f;
	float interp = 0.0f;
	for (int i = 0; i < num_elements; ++i)
	{
		out[i] = start_val*(1.0f - interp)+ interp*end_val;
		interp += weight*scale;
		weight = weight*factor;
	}
}
cImageData::cImageData():mDataPointer(NULL),mHeight(-1),mWidth(-1),mBytesPerPixel(-1),mFileName(""),mMipLevel(0),mMipDataPointer(NULL)
{
	
}

cImageData::cImageData( const char* file_name, int bpp /*= 0*/ ):mDataPointer(NULL),mHeight(-1),mWidth(-1),mBytesPerPixel(-1),mFileName(""),mMipLevel(0),mMipDataPointer(NULL)
{
	LoadFile(file_name, bpp);
}

cImageData::~cImageData()
{
	if(mDataPointer)
	{
		free(mDataPointer);
	}
	if(mMipDataPointer)
	{
		free(mMipDataPointer);
	}
}

void cImageData::LoadFile( const char* file_name, int bpp /* = 0*/ )
{
	mDataPointer = stbi_load(file_name, &mWidth, &mHeight, &mBytesPerPixel, bpp);
	if(mDataPointer)
	{
		mFileName = file_name;
	}
	if(bpp != 0)
	{
		mBytesPerPixel = bpp;
	}
}

void cImageData::GenerateMipData( int level )
{
	if(mDataPointer == NULL)
	{
		return;
	}

	const int mip_factor = 1 << level;
	const int row_mip_factor = mHeight > mip_factor ? mip_factor : mHeight;
	const int mip_mask = mip_factor - 1;
	const int old_mip_level = mMipLevel;

	mMipLevel = level;
	const int mip_width = MipWidth();
	const int mip_height = MipHeight();
	
	if(mMipDataPointer != NULL && old_mip_level > mMipLevel)
	{
		free(mMipDataPointer);
		mMipDataPointer = NULL;	
	}
	if(NULL == mMipDataPointer)
	{
		mMipDataPointer = static_cast<unsigned char*>(malloc(mip_height*mip_width*mBytesPerPixel));
	}
	for (int j = 0; j < mip_height; j++)
	{
		const int sample_row = j*mWidth*mip_factor*mBytesPerPixel;
		const int mip_row = j*mip_width*mBytesPerPixel;
		for (int ib = 0; ib < mip_width*mBytesPerPixel; ib += mBytesPerPixel)
		{
			const int sample_row_index = ib*mip_factor;
			for (int b = 0; b < mBytesPerPixel; b++)
			{
				double mip_sum = 0;
				for (int mip_sample = 0; mip_sample < mip_factor*row_mip_factor; mip_sample ++)
				{
					const int row = (mip_sample >> mMipLevel);
					const int index = sample_row + mBytesPerPixel*((mip_sample & mip_mask) + row*mWidth);
					mip_sum += mDataPointer[index + b + sample_row_index];
				}
				const unsigned char mip_avg = static_cast<unsigned char>(floor(mip_sum/(mip_factor*row_mip_factor)));

				mMipDataPointer[ mip_row + ib + b] = mip_avg;
			}
		}
	}

}

