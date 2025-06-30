#include "RootSignature_SkyDomeMesh.h"
#include "DrawBase.h"
#include <d3dx12.h>

RootSignature_SkyDomeMesh::RootSignature_SkyDomeMesh()
{
	// アプリケーションの入力アセンブラを使用する
	auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// ドメインシェーダーのルートシグネチャへのアクセスを拒否する
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
	// ハルシェーダーのルートシグネチャへのアクセスを拒否する
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
	// ジオメトリシェーダーのルートシグネチャへのアクセスを拒否する
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	// ルートパラメータを設定
	// 定数バッファとテクスチャの二つ
	CD3DX12_ROOT_PARAMETER rootParam[2] = {};
	// [0] b0：定数バッファ、頂点シェーダー用
	rootParam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
	// [1] t0：テクスチャ、ピクセルシェーダー用
	CD3DX12_DESCRIPTOR_RANGE texRange;
	// 初期化処理:SRVを設定、数は１個、t0にバインド
	texRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	rootParam[1].InitAsDescriptorTable(1, &texRange, D3D12_SHADER_VISIBILITY_PIXEL);

	// スタティックサンプラーの設定
	auto sampler = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

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
		IID_PPV_ARGS(m_pRootSignature.ReleaseAndGetAddressOf())); // ルートシグニチャ格納先のポインタ
	if (FAILED(hr))
	{
		printf("ルートシグネチャの生成に失敗");
		return;
	}

	m_IsValid = true;
}

bool RootSignature_SkyDomeMesh::IsValid()
{
	return m_IsValid;
}

ID3D12RootSignature* RootSignature_SkyDomeMesh::Get()
{
	return m_pRootSignature.Get();
}