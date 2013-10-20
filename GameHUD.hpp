#pragma once
#include "cs314Interfaces.h"
#include <string>

class cDrawnStringFactory;

class cGameHUD :
	public iDrawable
{
public:
	cGameHUD(cDrawnStringFactory& string_factory);
	~cGameHUD(void);
	cDrawnStringFactory& mDrawnStringFactory;

	std::string mTargetString;
	std::string yString;
	std::string xString;
	std::string zString;

};

