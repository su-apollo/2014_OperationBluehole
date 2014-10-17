#include "stdafx.h"
#include "ProcessManager.h"
#include "Timer.h"
#include "Logger.h"
#include "Renderer.h"

ProcessManager::ProcessManager()
{
}


ProcessManager::~ProcessManager()
{
}

BOOL ProcessManager::Init() const
{
	if (!Renderer::GetInstance()->Init())
		return FALSE;

	return TRUE;
}

BOOL ProcessManager::Process() const
{
	if (!mIsContinue)
		return FALSE;

	Timer::GetInstance()->OnTick();

	// todo : dispatch input

	Renderer::GetInstance()->Render();

	return TRUE;
}

void ProcessManager::Destroy() const
{
	Renderer::Release();
	Timer::Release();
}
