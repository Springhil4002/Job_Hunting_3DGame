#include "AssimpLoader.h"
#include "System/SharedStruct.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <d3dx12.h>
#include <filesystem>

namespace fs = std::filesystem;

/// @brief ファイルパスを受け取り、ディレクトリ部分を返す関数
/// @param _origin ファイルパス
/// @return ディレクトリ部分
std::wstring GetDirectoryPath(const std::wstring& _origin)
{
	fs::path p = _origin;
	return p.remove_filename().wstring();
}

/// @brief UTF-16の文字列をUTF-8に変換する処理
/// @param _value UTF-16文字列
/// @return UTF-8のstd::stringを返す
std::string ToUTF8(const std::wstring& _value)
{
	// バッファサイズの取得
	auto length = WideCharToMultiByte(
		CP_UTF8, 0U, _value.data(), -1, nullptr, 0, nullptr, nullptr);

	if (length == 0) return{};
	
	// 文字列を書きこみためのバッファ確保
	std::vector<char> buffer(length);

	// 作ったバッファに変換した文字列を入れる
	WideCharToMultiByte(
		CP_UTF8, 0U, _value.data(), -1, buffer.data(), length, nullptr, nullptr);
	
	return std::string(buffer.data());
}

/// @brief std::string(マルチバイト文字列)からstd::wstring(ワイド文字列)に変換する関数
/// @param _str std::string文字列
/// @return std::wstring文字列を返します
std::wstring ToWideString(const std::string& _str)
{
	// 変換後の文字数を取得
	auto num = MultiByteToWideChar(
		CP_UTF8, 0, _str.c_str(), -1, nullptr, 0);
	if (num == 0) return {};

	// 適切なサイズの配列を確保
	std::wstring wstr(num, 0);

	// 作った配列に変換後の文字列を入れる
	auto conv= MultiByteToWideChar(
		CP_UTF8, 0, _str.c_str(), -1, &wstr[0], num);
	if (conv == 0) return {};
	
	// null文字終端を除外
	wstr.pop_back();
	return wstr;
}

/// @brief 3Dモデルを読み込み、Mesh構造体に変換する関数
/// @param _settings 読み込みに必要な構造体
/// @return 読み込みの成否を返します
bool AssimpLoader::Load(ImportSettings _settings)
{
	// nullチェック
	if (_settings.fileName == nullptr)
	{
		return false;
	}

	// 構造体の各メンバを変数に代入して扱いやすくする
	auto& meshes = _settings.meshs;
	auto inverse_U = _settings.inverse_U;
	auto inverse_V = _settings.inverse_V;
	// ファイルパスを変換してAssimpでうけとれるようにする
	auto path = ToUTF8(_settings.fileName);

	// Assimpの読み込み設定
	Assimp::Importer importer;
	unsigned int flag = 0;
	// 三角形に変換
	flag |= aiProcess_Triangulate;
	// すべてのノード変換を適用
	flag |= aiProcess_PreTransformVertices;
	// タンジェント・バイタンジェント生成
	flag |= aiProcess_CalcTangentSpace;
	// スムーズな法線生成
	flag |= aiProcess_GenSmoothNormals;
	// UV座標を生成
	flag |= aiProcess_GenUVCoords;
	// 使われてないマテリアルを除去
	flag |= aiProcess_RemoveRedundantMaterials;
	// Meshを統合・整理
	flag |= aiProcess_OptimizeMeshes;

	// ファイルを読み込んでシーンデータを得る
	auto scene = importer.ReadFile(path, flag);

	// 読み込み失敗時はエラー出力
	if (scene == nullptr)
	{
		// もし読み込みエラーがでたら表示する
		printf(importer.GetErrorString());
		printf("\n");
		return false;
	}

	// Mesh構造体を初期化
	meshes.clear();
	// 読み込んだメッシュの数分だけ確保
	meshes.resize(scene->mNumMeshes);
	
	// 読み込んだデータを自分で定義したMesh構造体に変換する
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		const auto pMesh = scene->mMeshes[i];
		LoadMesh(meshes[i], pMesh, inverse_U, inverse_V);
		const auto pMaterial = scene->mMaterials[i];
		LoadTexture(_settings.fileName, meshes[i], pMaterial);
	}

	scene = nullptr;

	return true;
}

/// @brief Assimpで読み込んだメッシュデータを自作Meshクラスに変換する関数
/// @param _dst 自作メッシュ構造体
/// @param _src Assimpメッシュ構造体
/// @param _inverse_U U座標の反転フラグ
/// @param _inverse_V V座標の反転フラグ
void AssimpLoader::LoadMesh(Mesh& _dst, const aiMesh* _src, bool _inverse_U, bool _inverse_V)
{
	// メッシュに情報が無いように代替値を用意
	aiVector3D zero3D(0.0f, 0.0f, 0.0f);
	aiColor4D zeroColor(0.0f, 0.0f, 0.0f, 0.0f);

	// 取得した頂点数分、配列を確保
	_dst.Vertices.resize(_src->mNumVertices);

	// 頂点数文の各頂点データを変換
	for (auto i = 0u; i < _src->mNumVertices; ++i)
	{
		auto position = &(_src->mVertices[i]);
		auto normal	= &(_src->mNormals[i]);
		auto uv = (_src->HasTextureCoords(0)) ? &(_src->mTextureCoords[0][i]) : &zero3D;
		auto tangent = (_src->HasTangentsAndBitangents()) ? &(_src->mTangents[i]) : &zero3D;
		auto color = (_src->HasVertexColors(0)) ? &(_src->mColors[0][i]) : &zeroColor;

		// 反転フラグがあったらUVを反転
		if (_inverse_U)
		{
			uv->x = 1 - uv->x;
		}
		if (_inverse_V)
		{
			uv->y = 1 - uv->y;
		}

		// DirectX形式に変換
		Vertex vertex = {};
		vertex.position = XMFLOAT3(position->x, position->y, position->z);
		vertex.normal	= XMFLOAT3(normal->x, normal->y, normal->z);
		vertex.uv		= XMFLOAT2(uv->x, uv->y);
		vertex.tangent	= XMFLOAT3(tangent->x, tangent->y, tangent->z);
		vertex.color	= XMFLOAT4(color->r, color->g, color->b, color->a);

		_dst.Vertices[i] = vertex;
	}

	// ポリゴン単位を三角形とする
	_dst.Indices.resize(_src->mNumFaces * 3);
	// 各ポリゴンの頂点インデックスを一つの連続した配列に入れる
	for (auto i = 0u; i < _src->mNumFaces; ++i)
	{
		const auto& face = _src->mFaces[i];

		_dst.Indices[i * 3 + 0] = face.mIndices[0];
		_dst.Indices[i * 3 + 1] = face.mIndices[1];
		_dst.Indices[i * 3 + 2] = face.mIndices[2];
	}
}

/// @brief Assimpで読み込んだマテリアルからテクスチャにパスを自作Mesh構造体に設定する関数
/// @param _filename モデルファイルのパス
/// @param _dst Mesh構造体
/// @param _src Assimpで読み込んだマテリアル情報
void AssimpLoader::LoadTexture(const wchar_t* _filename, Mesh& _dst, const aiMaterial* _src)
{
	// テクスチャのパスを取得
	aiString path;
	if (_src->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)
	{
		// 相対パスであるテクスチャパスをフルパスに変換する
		auto dir = GetDirectoryPath(_filename);
		auto file = std::string(path.C_Str());
		_dst.DiffuseMap = dir + ToWideString(file);
	}
	else
	{
		// テクスチャがないならクリア
		_dst.DiffuseMap.clear();
	}
}