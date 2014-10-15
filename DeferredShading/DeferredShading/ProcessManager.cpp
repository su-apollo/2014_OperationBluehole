#include "stdafx.h"
#include "ProcessManager.h"


ProcessManager::ProcessManager()
{
}


ProcessManager::~ProcessManager()
{
}

BOOL ProcessManager::Init() const
{
	// todo : renderer init

	return TRUE;
}

BOOL ProcessManager::Process() const
{
	if (!mIsContinue)
		return FALSE;

	// todo : timer set

	// todo : dispatch input

	// todo : rendering

	return TRUE;
}

void ProcessManager::Destroy() const
{
	// todo : release renderer
}
