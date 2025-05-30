#pragma once
#include "System/ComPtr.h"
#include <d3dx12.h>
#include <string>

class PipelineState_WaterMesh
{
private:
	// 生成に成否のフラグ
	bool m_IsValid = false;
	// パイプラインステートの設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	// パイプラインステート
	ComPtr<ID3D12PipelineState> m_pPipelineState = nullptr;
	// 頂点シェーダー
	ComPtr<ID3DBlob> m_pVsBlob;
	// ピクセルシェーダー
	ComPtr<ID3DBlob> m_pPSBlob;
public:
	/// @brief コンストラクタである程度の設定をする
	PipelineState_WaterMesh();
	/// @brief 生成の成否を返す処理
	/// @return　生成の成否を返します 
	bool IsValid();
	/// @brief 入力レイアウトを設定する処理
	/// @param _layout 入力レイアウト
	void SetInputLayout(D3D12_INPUT_LAYOUT_DESC _layout);
	/// @brief ルートシグネチャを設定する処理
	/// @param _rootSignature ルートシグネチャ
	void SetRootSignature(ID3D12RootSignature* _rootSignature);
	/// @brief 頂点シェーダーを設定する処理
	/// @param _filePath ファイルパス名
	void SetVS(std::wstring _filePath);
	/// @brief // ピクセルシェーダーを設定する処理
	/// @param _filePath ファイルパス名
	void SetPS(std::wstring _filePath);
	/// @brief パイプラインステートを生成する処理
	void Create();
	/// @brief パイプラインステートのポインタのゲッター関数
	/// @return パイプラインステートのポインタ
	ID3D12PipelineState* Get();
};

