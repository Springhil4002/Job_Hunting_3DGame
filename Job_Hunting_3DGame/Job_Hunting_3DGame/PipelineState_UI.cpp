#include "PipelineState_UI.h"
#include "DrawBase.h"
#include <d3dx12.h>
#include <d3dcompiler.h>
#include "Debug_New.h"

#pragma comment(lib,"d3dcompiler.lib")

PipelineState_UI::PipelineState_UI()
{
	// zeroMemoryで初期化
	ZeroMemory(&desc, sizeof(desc));

	// ラスタライザーステート:デフォルト設定
	desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	// カリングなし:両面描画
	desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	// 描画モデルの埋め方
	desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	// ブレンドステート
	desc.BlendState = InitBlendState();
	// 深度ステンシルステート
	desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	desc.DepthStencilState.DepthEnable = FALSE;		// 深度テスト無効
	desc.DepthStencilState.StencilEnable = FALSE;	// ステンシル無効
	desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	// サンプルマスク:全てのサンプルを有効化(通常はこれ)
	desc.SampleMask = UINT_MAX;
	// 描画プリミティブ:三角形描画に指定
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// 描画対象:1つ
	desc.NumRenderTargets = 1;
	//　フォーマット:sRGB変換付きのRGBA8形式
	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 深度バッファのフォーマット32bitFloat
	desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	// マルチサンプリング:なし
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
}

bool PipelineState_UI::IsValid() const
{
	return m_IsValid;
}

void PipelineState_UI::SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout)
{
	desc.InputLayout = layout;
}

void PipelineState_UI::SetRootSignature(ID3D12RootSignature* rootSignature)
{
	desc.pRootSignature = rootSignature;
}

void PipelineState_UI::SetVS(std::wstring filePath)
{
	// 頂点シェーダー読み込み
	auto hr = D3DReadFileToBlob(filePath.c_str(), m_pVsBlob.GetAddressOf());
	if (FAILED(hr))
	{
		printf("PSO_UI:頂点シェーダーの読み込みに失敗\n");
		return;
	}

	desc.VS = CD3DX12_SHADER_BYTECODE(m_pVsBlob.Get());
}

void PipelineState_UI::SetPS(std::wstring filePath)
{
	// ピクセルシェーダー読み込み
	auto hr = D3DReadFileToBlob(filePath.c_str(), m_pPSBlob.GetAddressOf());
	if (FAILED(hr))
	{
		printf("PSO_UI:ピクセルシェーダーの読み込みに失敗\n");
		return;
	}

	desc.PS = CD3DX12_SHADER_BYTECODE(m_pPSBlob.Get());
}

void PipelineState_UI::Create()
{
	// パイプラインステートを生成
	auto hr = g_DrawBase->Device()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pPipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		printf("PSO_UI:パイプラインステートの生成に失敗\n");
		return;
	}

	m_IsValid = true;
}

ID3D12PipelineState* PipelineState_UI::Get()
{
	return m_pPipelineState.Get();
}

D3D12_BLEND_DESC PipelineState_UI::InitBlendState()
{
	// ブレンドステートの初期化
	D3D12_BLEND_DESC blendDesc = {};

	// アルファカバレッジ無効
	blendDesc.AlphaToCoverageEnable = FALSE;
	// 独立ブレンド無効
	blendDesc.IndependentBlendEnable = FALSE;
	// ブレンド有効
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	// ソースブレンド:アルファ
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	// デスティネーションブレンド:逆アルファ
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	// ブレンド演算:加算
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	// アルファソースブレンド:1
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	// アルファデスティネーションブレンド:0
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	// アルファブレンド演算:加算
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	// 全てのカラー出力を有効化
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return blendDesc;
}