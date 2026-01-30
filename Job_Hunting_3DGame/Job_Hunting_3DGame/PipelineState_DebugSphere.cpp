#include "PipelineState_DebugSphere.h"
#include "DrawBase.h"
#include <d3dx12.h>
#include <d3dcompiler.h>
#include "Debug_New.h"
#include "Debug_Msg.h"

#pragma comment(lib,"d3dcompiler.lib")

// パイプラインステートの設定
PipelineState_DebugSphere::PipelineState_DebugSphere()
{
	// zeroMemoryで初期化
	ZeroMemory(&desc, sizeof(desc));

	// ラスタライザーステート:デフォルト設定
	desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	// ワイヤーフレーム描画
	desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; 
	// カリングなし:両面描画
	desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	// ブレンドステート
	desc.BlendState = InitBlendState();
	// 深度ステンシルステート
	desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	desc.DepthStencilState.DepthEnable = TRUE;
	desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	// サンプルマスク:全てのサンプルを有効化(通常はこれ)
	desc.SampleMask = UINT_MAX;
	// 描画プリミティブ:三角形描画に指定
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// 描画対象:1つ
	desc.NumRenderTargets = 1;
	//　フォーマット:sRGB変換付きのRGBA8形式
	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 深度バッファのフォーマット32bitFloat
	desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	// マルチサンプリング:なし
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
}

bool PipelineState_DebugSphere::IsValid() const
{
	return m_IsValid;
}

void PipelineState_DebugSphere::SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout)
{
	desc.InputLayout = layout;
}

void PipelineState_DebugSphere::SetRootSignature(ID3D12RootSignature* rootSignature)
{
	desc.pRootSignature = rootSignature;
}

void PipelineState_DebugSphere::SetVS(std::wstring filePath)
{
	// 頂点シェーダー読み込み
	auto hr = D3DReadFileToBlob(filePath.c_str(), m_pVsBlob.GetAddressOf());
	if (FAILED(hr))
	{
		DEBUG_LOG_ERROR(L"PSO_Sphere:頂点シェーダーの読み込みに失敗");
		return;
	}

	desc.VS = CD3DX12_SHADER_BYTECODE(m_pVsBlob.Get());
}

void PipelineState_DebugSphere::SetPS(std::wstring filePath)
{
	// ピクセルシェーダー読み込み
	auto hr = D3DReadFileToBlob(filePath.c_str(), m_pPSBlob.GetAddressOf());
	if (FAILED(hr))
	{
		DEBUG_LOG_ERROR(L"PSO_Sphere:ピクセルシェーダーの読み込みに失敗");
		return;
	}

	desc.PS = CD3DX12_SHADER_BYTECODE(m_pPSBlob.Get());
}

void PipelineState_DebugSphere::Create()
{
	// パイプラインステートを生成
	HRESULT hr = g_DrawBase->Device()->CreateGraphicsPipelineState(&desc,IID_PPV_ARGS(m_pPipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		DEBUG_LOG_ERROR(L"PSO_Sphere:パイプラインステートの生成に失敗");
		m_IsValid = false;
		return;
	}
	m_IsValid = true;
}

ID3D12PipelineState* PipelineState_DebugSphere::Get()
{
	return m_pPipelineState.Get();
}

D3D12_BLEND_DESC PipelineState_DebugSphere::InitBlendState()
{
	D3D12_BLEND_DESC desc = {};
	desc.AlphaToCoverageEnable = FALSE;
	desc.IndependentBlendEnable = FALSE;

	desc.RenderTarget[0].BlendEnable = TRUE;
	desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return desc;
}