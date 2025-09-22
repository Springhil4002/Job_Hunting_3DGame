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

// 頂点データ(インスタンシング)の定義
struct VertexInstance
{
	XMFLOAT3 position;	// 位置
	XMFLOAT3 normal;	// 法線
	XMFLOAT2 uv;		// UV座標
	XMFLOAT3 tangent;	// 接空間
	XMFLOAT4 color;		// 頂点カラー

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;
private:
	static const int InputElementCount = 8;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

// 3Dオブジェクト用の変換行列の定義
struct alignas(256) Matrix
{
	Matrix4x4 world;	// ワールド行列
	Matrix4x4 view;		// ビュー行列
	Matrix4x4 proj;		// 投影行列
	float time;			// 波のための時間
	XMFLOAT3 cameraPos;	// カメラ位置
	float alpha;		// 透明度
};

struct alignas(256) MatrixUI
{
	Matrix4x4 world;	// ワールド行列
	Matrix4x4 view;		// ビュー行列
	Matrix4x4 proj;		// 投影行列
	float alpha;		// 透明度
	float padding[3];	// パディング
	XMFLOAT4 uv;		// UV情報
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

struct LightPalams
{
	XMFLOAT3 lightDir;		// ライトの方向
	float pad1;				// パディング
	XMFLOAT4 lightColor;	// ライトの色
};

struct SkyVertex {
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 UV;
	static const D3D12_INPUT_LAYOUT_DESC InputLayout;
private:
	static const int InputElementCount = 2;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct VertexUI
{
	XMFLOAT3 pos;	// 位置
	XMFLOAT2 uv;	// テクスチャ座表
	XMFLOAT4 color;	// 色
	static const D3D12_INPUT_LAYOUT_DESC InputLayout;
private:
	static const int InputElementCount = 3;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct Mesh_UI
{
	std::vector<VertexUI> vertices;	// 頂点データの配列
	std::vector<uint32_t> indices;	// インデックスの配列
	std::wstring DiffuseMap;		// テクスチャのファイルパス
};