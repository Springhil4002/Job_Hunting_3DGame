#pragma once
#include <d3dx12.h>
#include "CommonTypes.h"
#include "ComPtr.h"

// 頂点データの定義
struct Vertex
{
	XMFLOAT3 position;	// 位置
	XMFLOAT3 normal;	// 法線
	XMFLOAT2 uv;		// UV座標
	XMFLOAT3 tangent;	// 接空間
	XMFLOAT4 color;		// 頂点カラー

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;
private:
	static const int InputElementCount = 5;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

// 変換行列の定義
struct alignas(256) Matrix
{
	Matrix4x4 world;	// ワールド行列
	Matrix4x4 view;		// ビュー行列
	Matrix4x4 proj;		// 投影行列

	float time;			// 波のための時間
	float padding[3];	// 16バイトアライメント
};

struct GerstnerParams
{
	DirectX::XMFLOAT4 amplitude[4];		// x=振幅
	DirectX::XMFLOAT4 direction[4];		// xy=方向
	DirectX::XMFLOAT4 waveLength[4];	// x=波長
	DirectX::XMFLOAT4 speed[4];			// x=速度
};

struct Mesh
{
	std::vector<Vertex> Vertices;	// 頂点データの配列
	std::vector<uint32_t> Indices;	// インデックスの配列
	std::wstring DiffuseMap;		// テクスチャのファイルパス
};