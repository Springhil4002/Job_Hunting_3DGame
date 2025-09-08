#include "PipelineState_General.h"
#include "DrawBase.h"
#include <d3dx12.h>
#include <d3dcompiler.h>

#pragma comment(lib,"d3dcompiler.lib")

// 現状このパイプラインステートを使える対象
// Player、Goal,Model3D

PipelineState_General::PipelineState_General()
{
    // zeroMemoryで初期化
    ZeroMemory(&desc, sizeof(desc));

    // ラスタライザーステート:デフォルト設定
    desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    // カリングなし:両面描画
    desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    // 描画モデルの埋め方
    desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    // ブレンドステート（不透明描画）
    desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    // 深度ステンシル:デフォルト設定
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

bool PipelineState_General::IsValid() const
{
    return m_IsValid;
}

void PipelineState_General::SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout)
{
    desc.InputLayout = layout;
}

void PipelineState_General::SetRootSignature(ID3D12RootSignature* rootSignature)
{
    desc.pRootSignature = rootSignature;
}

void PipelineState_General::SetVS(std::wstring filePath)
{
    HRESULT hr = D3DReadFileToBlob(filePath.c_str(), m_pVsBlob.GetAddressOf());
    if (FAILED(hr))
    {
        printf("PipelineState_General: VS読み込み失敗\n");
        return;
    }
    desc.VS = CD3DX12_SHADER_BYTECODE(m_pVsBlob.Get());
}

void PipelineState_General::SetPS(std::wstring filePath)
{
    HRESULT hr = D3DReadFileToBlob(filePath.c_str(), m_pPSBlob.GetAddressOf());
    if (FAILED(hr))
    {
        printf("PipelineState_General: PS読み込み失敗\n");
        return;
    }
    desc.PS = CD3DX12_SHADER_BYTECODE(m_pPSBlob.Get());
}

void PipelineState_General::Create()
{
    HRESULT hr = g_DrawBase->Device()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pPipelineState.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        printf("PipelineState_General: PSO生成失敗\n");
        m_IsValid = false;
        return;
    }
    m_IsValid = true;
}

ID3D12PipelineState* PipelineState_General::Get()
{
    return m_pPipelineState.Get();
}