#include "stdafx.h"
#include "ProcessManager.h"
#include "Timer.h"
#include "Logger.h"
#include "Renderer.h"
#include "LightManager.h"
#include "GBuffManager.h"

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

	if (!GBuffManager::GetInstance()->Init())
		return FALSE;

	return TRUE;
}

BOOL ProcessManager::Process() const
{
	if (!mIsContinue)
		return FALSE;

	Timer::GetInstance()->OnTick();

	// todo : dispatch input

	Camera::GetInstance()->Update();

	Renderer::GetInstance()->Render();

	return TRUE;
}

void ProcessManager::Destroy() const
{
	Renderer::Release();
	Timer::Release();
	Camera::Release();
	LightManager::Release();
}
