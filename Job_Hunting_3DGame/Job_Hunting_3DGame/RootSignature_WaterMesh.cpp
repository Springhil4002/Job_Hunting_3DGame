#include "RootSignature_WaterMesh.h"
#include "DrawBase.h"
#include <d3dx12.h>

RootSignature_WaterMesh::RootSignature_WaterMesh()
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
	CD3DX12_DESCRIPTOR_RANGE texRange;
	// t0に1個SRV
	texRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_ROOT_PARAMETER rootParam[4] = {};
	// b0:のMatrix定数バッファを設定、全てのシェーダーから見えるようにする
	rootParam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); 
	// b1:Gerstner波定数バッファを設定、全てのシェーダーから見えるようにする
	rootParam[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); 
	// b2:ライト用定数バッファを設定、全てのシェーダーから見えるようにする
	rootParam[2].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	// t0:WaterMesh用のテクスチャ設定、ピクセルシェーダーから見えるようにする。
	rootParam[3].InitAsDescriptorTable(1, &texRange, D3D12_SHADER_VISIBILITY_PIXEL);
		
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
		printf("ルートシグネチャシリアライズに失敗");
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
		printf("ルートシグネチャの生成に失敗");
		return;
	}

	m_IsValid = true;
}

bool RootSignature_WaterMesh::IsValid()
{
	return m_IsValid;
}

ID3D12RootSignature* RootSignature_WaterMesh::Get()
{
	return m_pRootSignature.Get();
}