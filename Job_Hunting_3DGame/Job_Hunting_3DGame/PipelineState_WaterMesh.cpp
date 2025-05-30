#include "PipelinState_WaterMesh.h"
#include "PipelineState.h"
#include "DrawBase.h"
#include <d3dx12.h>
#include <d3dcompiler.h>

#pragma comment(lib,"d3dcompiler.lib")

// パイプラインステートの設定
PipelineState::PipelineState()
{
	// ラスタライザーステート:デフォルト設定
	desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	// カリングなし:両面描画
	desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	// 描画モデルの埋め方
	desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	// ブレンドステート:デフォルト設定(不透明描画)
	desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	// 深度ステンシルステート:デフォルト設定(深度テストON,書きこみOK)
	desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
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

bool PipelineState::IsValid()
{
	return m_IsValid;
}

void PipelineState::SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout)
{
	desc.InputLayout = layout;
}

void PipelineState::SetRootSignature(ID3D12RootSignature* rootSignature)
{
	desc.pRootSignature = rootSignature;
}

void PipelineState::SetVS(std::wstring filePath)
{
	// 頂点シェーダー読み込み
	auto hr = D3DReadFileToBlob(filePath.c_str(), m_pVsBlob.GetAddressOf());
	if (FAILED(hr))
	{
		printf("頂点シェーダーの読み込みに失敗\n");
		return;
	}

	desc.VS = CD3DX12_SHADER_BYTECODE(m_pVsBlob.Get());
}

void PipelineState::SetPS(std::wstring filePath)
{
	// ピクセルシェーダー読み込み
	auto hr = D3DReadFileToBlob(filePath.c_str(), m_pPSBlob.GetAddressOf());
	if (FAILED(hr))
	{
		printf("ピクセルシェーダーの読み込みに失敗\n");
		return;
	}

	desc.PS = CD3DX12_SHADER_BYTECODE(m_pPSBlob.Get());
}

void PipelineState::Create()
{
	// パイプラインステートを生成
	auto hr = g_DrawBase->Device()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pPipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		printf("パイプラインステートの生成に失敗\n");
		return;
	}

	m_IsValid = true;
}

ID3D12PipelineState* PipelineState::Get()
{
	return m_pPipelineState.Get();
}