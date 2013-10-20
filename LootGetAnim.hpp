#pragma once
#include "TimedItem.hpp"
class cLootGetAnim :
	public iTimedItem
{
public:
	virtual void Update(float elapsed_time) = 0;
	virtual bool IsDone() = 0;
	cLootGetAnim(void);
	~cLootGetAnim(void);
};

