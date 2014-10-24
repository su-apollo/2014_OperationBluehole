#include "stdafx.h"
#include "ProcessManager.h"
#include "Timer.h"
#include "Logger.h"
#include "Renderer.h"
#include "LightManager.h"
#include "RTManager.h"
#include "PostProcessor.h"
#include "SamplerManager.h"

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

	if (!RTManager::GetInstance()->Init())
		return FALSE;

	if (!PostProcessor::GetInstance()->Init())
		return FALSE;

	if (!SamplerManager::GetInstance()->Init())
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

	// clear
	Renderer::GetInstance()->SetRasterizeStage();
	Renderer::GetInstance()->SetupViewPort();
	Renderer::GetInstance()->ClearBackBuff();
	Renderer::GetInstance()->ClearDepthStencilBuff();
	RTManager::GetInstance()->ClearRenderTargets();
	RTManager::GetInstance()->SetRenderTargetToGBuff();

	// render line
	Renderer::GetInstance()->Render();
	PostProcessor::GetInstance()->Render();

	// render stage clean up
	// 이거 하나 추가했다고 프레임이 대폭상승
	// directx debug로 인해 콘솔에 찍는 비용이 매우큼
	// 때문에 프레임이 안정적이지 못하고 오락가락했던것 같음
	PostProcessor::GetInstance()->RenderCleanUp();
	// end
	Renderer::GetInstance()->SwapChain();

	return TRUE;
}

void ProcessManager::Destroy() const
{
	Renderer::Release();
	Timer::Release();
	Camera::Release();
	LightManager::Release();
	RTManager::Release();
	PostProcessor::Release();
	SamplerManager::Release();
}
