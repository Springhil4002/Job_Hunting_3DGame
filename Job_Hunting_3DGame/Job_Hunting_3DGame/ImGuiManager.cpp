#include "ImGuiManager.h"
#include "DrawBase.h"
#include "Debug_New.h"

bool ImGuiManager::Init(HWND _hwnd, DrawBase* _drawBase)
{
	if (!_hwnd || !_drawBase) return false;
	if (ImGui::GetCurrentContext()) ShutDown();	

	m_pDrawBase = _drawBase;

	// SRVヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = NumDescriptors;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	HRESULT hr = m_pDrawBase->Device()->CreateDescriptorHeap(
		&desc, IID_PPV_ARGS(&m_pSrvHeap));
	if (FAILED(hr))
	{
		return false;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();

	if (!ImGui_ImplWin32_Init(_hwnd))
	{
		return false;
	}

	ImGui_ImplDX12_InitInfo initInfo = {};
	initInfo.Device = m_pDrawBase->Device();
	initInfo.CommandQueue = m_pDrawBase->CommandQueue();
	initInfo.NumFramesInFlight = m_pDrawBase->FRAME_BUFFER_COUNT;
	initInfo.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	initInfo.DSVFormat = DXGI_FORMAT_UNKNOWN;
	initInfo.SrvDescriptorHeap = m_pSrvHeap.Get();
	initInfo.LegacySingleSrvCpuDescriptor = m_pSrvHeap->GetCPUDescriptorHandleForHeapStart();
	initInfo.LegacySingleSrvGpuDescriptor = m_pSrvHeap->GetGPUDescriptorHandleForHeapStart();
	
	if (!ImGui_ImplDX12_Init(&initInfo))
	{
		return false;
	}

	return true;
}

void ImGuiManager::Begin()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::End()
{
	ImGui::Render();
	ID3D12GraphicsCommandList* cmdList = m_pDrawBase->CommandList();
	
	// Imgui用のSRVヒープをセット
	ID3D12DescriptorHeap* heaps[] = { m_pSrvHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(heaps), heaps);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);
}

void ImGuiManager::ShutDown()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	m_pDrawBase = nullptr;
	m_pSrvHeap.Reset();
}