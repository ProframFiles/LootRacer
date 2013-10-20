#pragma once

class iTimedItem
{
public:
	iTimedItem()
		:isActive(true)
	{}
	virtual ~iTimedItem()
	{}
	virtual void Update(float elapsed_time) = 0;
	virtual bool IsDone() = 0;
	bool IsActive() const {
		return isActive;
	}
	void SetActive() {
		isActive = true;
	}
	void SetInactive() {
		isActive = false;
	}
private:
	bool isActive;
};
