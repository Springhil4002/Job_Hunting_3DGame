#pragma once
#include "System/ComPtr.h"

struct ID3D12RootSignature;

class RootSignature
{
private:
	// ルートシグネチャの生成に成功したかのフラグ
	bool m_IsValid = false; 
	// ルートシグネチャ
	ComPtr<ID3D12RootSignature> m_pRootSignature = nullptr; 
public:
	// コンストラクタでルートシグネチャを生成
	RootSignature(); 
	/// @brief // ルートシグネチャの生成に成否を取得する処理
	/// @return ルートシグネチャの生成成否を返す
	bool IsValid(); 
	/// @brief ルートシグネチャを取得する処理
	/// @return // ルートシグネチャを返す
	ID3D12RootSignature* Get(); 
};

