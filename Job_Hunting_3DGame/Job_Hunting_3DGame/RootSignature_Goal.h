#pragma once
#include "System/ComPtr.h"

struct ID3D12RootSignature;

class RootSignature_Goal
{
private:
	// ルートシグネチャの生成に成功したかのフラグ
	bool m_IsValid = false;
	// ルートシグネチャ
	ComPtr<ID3D12RootSignature> m_pRootSignature = nullptr;
public:
	// コンストラクタでルートシグネチャを生成
	RootSignature_Goal();
	/// @brief	ルートシグネチャの生成に成否を取得する処理
	/// @return ルートシグネチャの生成成否を返す
	bool IsValid();
	/// @brief	ルートシグネチャを取得する処理
	/// @return ルートシグネチャを返す
	ID3D12RootSignature* Get();
};