#pragma once
#define NOMINMAX
#include <d3d12.h>
#include <DirectXMath.h>
#include <string>
#include <vector>

struct Mesh;
struct Vertex;

// Assimpの構造体
struct aiMesh;
struct aiMaterial;

// インポートするときのパラメータ
struct ImportSettings
{
	// 読み込むモデルのファイルパス
	const wchar_t* fileName = nullptr;	
	// 読み込んだメッシュを入れる配列
	std::vector<Mesh>& meshs;
	// U座標を反転させるか
	bool inverse_U = false;
	// V座標を反転させるか
	bool inverse_V = false;
};

class AssimpLoader
{
private:
	/// @brief メッシュ読み込み関数
	/// @param _dst メッシュデータの配列
	/// @param _src Assimpのメッシュ構造体
	/// @param _inverse_U U座標の反転フラグ
	/// @param _inverse_V V座標の反転フラグ
	void LoadMesh(Mesh& _dst, const aiMesh* _src, bool _inverse_U, bool _inverse_V);
	/// @brief テクスチャ読み込み関数
	/// @param _fileName モデルファイルパス名
	/// @param _dst メッシュ構造体
	/// @param _src Assimpのマテリアル構造体
	void LoadTexture(const wchar_t* _fileName, Mesh& _dst, const aiMaterial* _src);
public:
	/// @brief モデルを読み込む関数
	/// @param _Setting モデルを読み込むのに必要なパラメータ群
	/// @return 読み込みの成否を返します
	bool Load(ImportSettings _Setting);
};

