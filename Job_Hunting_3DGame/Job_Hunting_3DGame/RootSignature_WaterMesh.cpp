#include "RootSignature_WaterMesh.h"
#include "DrawBase.h"
#include <d3dx12.h>
#include "Debug_New.h"
#include "Debug_Msg.h"

RootSignature_WaterMesh::RootSignature_WaterMesh()
{
	auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	
	CD3DX12_DESCRIPTOR_RANGE texRange[1] = {};
	texRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);

	CD3DX12_ROOT_PARAMETER rootParam[5] = {};
	rootParam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); 
	rootParam[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); 
	rootParam[2].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootParam[3].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootParam[4].InitAsDescriptorTable(1, &texRange[0], D3D12_SHADER_VISIBILITY_PIXEL);
	
	auto sampler = CD3DX12_STATIC_SAMPLER_DESC(
		0,									
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,	
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,	
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,	
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,	
		0.0f,								
		1,									
		D3D12_COMPARISON_FUNC_ALWAYS,		
		D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
		0.0f,							
		D3D12_FLOAT32_MAX,					
		D3D12_SHADER_VISIBILITY_PIXEL		
	);

	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.NumParameters = _countof(rootParam);
	desc.NumStaticSamplers = 1;
	desc.pParameters = rootParam;
	desc.pStaticSamplers = &sampler;
	desc.Flags = flag;

	ComPtr<ID3DBlob> pBlob;
	ComPtr<ID3DBlob> pErrorBlob;

	auto hr = D3D12SerializeRootSignature(
		&desc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		pBlob.GetAddressOf(),
		pErrorBlob.GetAddressOf());
	if (FAILED(hr))
	{
		//DEBUG_LOG_ERROR(L"UI:ルートシグネチャシリアライズに失敗");
		return;
	}

	hr = g_DrawBase->Device()->CreateRootSignature(
		0, 
		pBlob->GetBufferPointer(), 
		pBlob->GetBufferSize(),
		IID_PPV_ARGS(m_pRootSignature.GetAddressOf())); 
	if (FAILED(hr))
	{
		//DEBUG_LOG_ERROR(L"UI:ルートシグネチャの生成に失敗");
		return;
	}

	m_IsValid = true;
}

bool RootSignature_WaterMesh::IsValid() const
{
	return m_IsValid;
}

ID3D12RootSignature* RootSignature_WaterMesh::Get()
{
	return m_pRootSignature.Get();
}