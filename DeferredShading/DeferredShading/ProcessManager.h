#pragma once
#include "Singleton.h"





class ProcessManager : public Singleton<ProcessManager>
{
public:
	ProcessManager();
	~ProcessManager();

	BOOL Init() const;
	BOOL Process() const;
	void Destroy() const;
	void Stop() { mIsContinue = FALSE; }

private:
	BOOL mIsContinue = TRUE;

};
