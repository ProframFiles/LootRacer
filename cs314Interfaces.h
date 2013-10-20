#pragma once
class iDrawable
{
public:

	iDrawable(void)
	{}

	virtual ~iDrawable(void)
	{}

	virtual void Draw()=0;
};

class iVertexSpec
{
	virtual void BindAttributes() const =0;
};