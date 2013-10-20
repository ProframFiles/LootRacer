#include "TextVertex.hpp"
#include "akj_ogl.h"

cTextVertex::~cTextVertex(void)
{
}

void cTextVertex::BindAttributes()
{
	const int byte_stride = ByteStride();
	//texCoord
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, byte_stride, AKJ_FLOAT_OFFSET(0) );
	
	//normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, byte_stride, AKJ_FLOAT_OFFSET(2) );

	//velocity
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, byte_stride, AKJ_FLOAT_OFFSET(4) );

	//acceleration
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, byte_stride, AKJ_FLOAT_OFFSET(6) );
	
	// start time
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, byte_stride, AKJ_FLOAT_OFFSET(8) );
	
	// other stuff
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, byte_stride, AKJ_FLOAT_OFFSET(9) );

}

int cTextVertex::ByteStride()
{
	return 12*sizeof(GLfloat);
}
