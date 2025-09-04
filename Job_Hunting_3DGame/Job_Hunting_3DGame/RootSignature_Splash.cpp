#include "RootSignature_Splash.h"
#include "DrawBase.h"
#include <d3dx12.h>
#include "Debug_New.h"

RootSignature_Splash::RootSignature_Splash()
{
	// アプリケーションの入力アセンブラを使用する
	auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// ドメインシェーダーのルートシグネチャへのアクセスを拒否する
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
	// ハルシェーダーのルートシグネチャへのアクセスを拒否する
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
	// ジオメトリシェーダーのルートシグネチャへのアクセスを拒否する
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	// ディスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE texRange[1] = {};
	// t0に1個SRV
	texRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	// ルートパラメータ設定
	CD3DX12_ROOT_PARAMETER rootParam[2] = {};
	rootParam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootParam[1].InitAsDescriptorTable(1, &texRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	// スタティックサンプラーの設定
	auto sampler = CD3DX12_STATIC_SAMPLER_DESC(
		0,									// レジスター
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,	// フィルター
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,	// アドレスモード U
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,	// アドレスモード V
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,	// アドレスモード W
		0.0f,								// MipLODBias
		1,									// MaxAnisotropy
		D3D12_COMPARISON_FUNC_ALWAYS,		// 比較関数（使ってないなら ALWAYS でOK）
		D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
		0.0f,								// MinLOD
		D3D12_FLOAT32_MAX,					// MaxLOD
		D3D12_SHADER_VISIBILITY_PIXEL		// PSから見えるように明示
	);

	// ルートシグニチャの設定（設定したいルートパラメーターとスタティックサンプラーを入れる）
	D3D12_ROOT_SIGNATURE_DESC desc = {};
	// ルートパラメーターの個数をいれる
	desc.NumParameters = _countof(rootParam);
	// サンプラーの個数をいれる
	desc.NumStaticSamplers = 1;
	// ルートパラメーターのポインタをいれる
	desc.pParameters = rootParam;
	// サンプラーのポインタを入れる
	desc.pStaticSamplers = &sampler;
	// フラグを設定
	desc.Flags = flag;

	ComPtr<ID3DBlob> pBlob;
	ComPtr<ID3DBlob> pErrorBlob;

	// シリアライズ
	auto hr = D3D12SerializeRootSignature(
		&desc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		pBlob.GetAddressOf(),
		pErrorBlob.GetAddressOf());
	if (FAILED(hr))
	{
		printf("Particle Splash:ルートシグネチャシリアライズに失敗");
		return;
	}

	// ルートシグネチャ生成
	hr = g_DrawBase->Device()->CreateRootSignature(
		0, // GPUが複数ある場合のノードマスク（今回は1個しか無い想定なので0）
		pBlob->GetBufferPointer(), // シリアライズしたデータのポインタ
		pBlob->GetBufferSize(), // シリアライズしたデータのサイズ
		IID_PPV_ARGS(m_pRootSignature.GetAddressOf())); // ルートシグニチャ格納先のポインタ
	if (FAILED(hr))
	{
		printf("Particle Splash:ルートシグネチャの生成に失敗");
		return;
	}

	m_IsValid = true;
}

bool RootSignature_Splash::IsValid() const
{
	return m_IsValid;
}

ID3D12RootSignature* RootSignature_Splash::Get() const
{
	return m_pRootSignature.Get();
}