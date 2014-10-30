#include "stdafx.h"
#include "ProcessManager.h"
#include "Timer.h"
#include "Logger.h"
#include "Renderer.h"
#include "LightManager.h"
#include "RTManager.h"
#include "PostProcessor.h"
#include "SamplerManager.h"
#include "RSManager.h"
#include "Contents.h"

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

	RenderStateManager::GetInstance()->Init();

	Contents::GetInstance()->Init();
	
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
	Renderer::GetInstance()->SetupViewPort();
	RTManager::GetInstance()->ClearRenderTargets();
	RTManager::GetInstance()->SetRenderTargetToGBuff();

	// render line
	Renderer::GetInstance()->Render();
	Renderer::GetInstance()->ClearBackBuff();
	PostProcessor::GetInstance()->Render();

	// render stage clean up
	// �̰� �ϳ� �߰��ߴٰ� �������� �������
	// directx debug�� ���� �ֿܼ� ��� ����� �ſ�ŭ
	// ���� �������� �����ϸ鼭 ���� �������� ����Ÿ������ �׸��� �Ǹ鼭 ������ �߻��ϴ� ����
	// ������ �������� ���������� ���ϰ� ���������ߴ��� ����
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
	RenderStateManager::Release();
}
