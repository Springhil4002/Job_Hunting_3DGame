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
	
	CD3DX12_ROOT_PARAMETER rootParam[1] = {};
	// b0の定数バッファを設定、全てのシェーダーから見えるようにする
	rootParam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); 
	// スタティックサンプラーの設定
	auto sampler = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

	// ルートシグニチャの設定（設定したいルートパラメーターとスタティックサンプラーを入れる）
	D3D12_ROOT_SIGNATURE_DESC desc = {};
	// ルートパラメーターの個数をいれる
	desc.NumParameters = std::size(rootParam);
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