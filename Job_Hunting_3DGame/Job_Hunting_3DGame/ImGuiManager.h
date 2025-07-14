#pragma once
#include <d3d12.h>
#include "System/ComPtr.h"
#include "System/ImGui/imgui.h"
#include "System/ImGui/imgui_impl_dx12.h"
#include "System/ImGui/imgui_impl_win32.h"

class DrawBase;

class ImGuiManager
{
private:
	ComPtr<ID3D12DescriptorHeap> m_pSrvHeap;
	DrawBase* m_pDrawBase;
	static const UINT NumDescriptors = 1;
public:
	bool Init(HWND _hwnd, DrawBase* _drawBase);
	void Begin();
	void End();
	void ShutDown();
};

