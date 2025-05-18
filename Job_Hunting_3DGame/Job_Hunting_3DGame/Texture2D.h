#pragma once
#include "ComPtr.h"
#include <string>
#include <d3dx12.h>

class DescriptorHandle;
class DescriptorHeap;

class Texture2D
{
private:
	// 読み込みの成否フラグ
	bool m_IsValid;
	// テクスチャ情報を保持するリソース
	ComPtr<ID3D12Resource> m_pResource;
	/// @brief コンストラクタ
	/// @param _path ファイルパス
	Texture2D(std::string _path);
	/// @brief コンストラクタ
	/// @param _path ファイルパス
	Texture2D(std::wstring _path);
	/// @brief コンストラクタ
	/// @param _buffer リソース
	Texture2D(ID3D12Resource* _buffer);
	
	/// @brief ファイルパスを読み込んでテクスチャリソースを作成
	/// @param _path ファイルパス
	/// @return 作成の成否を返します
	bool Load(std::string& _path);
	/// @brief ファイルパスを読み込んでテクスチャリソースを作成
	/// @param _path ファイルパス
	/// @return 作成の成否を返します
	bool Load(std::wstring& _path);
	/// @brief 色なしのテクスチャなどのリソースを作る関数
	/// @param _width 横幅
	/// @param _height 高さ
	/// @return テクスチャリソースを返します
	static ID3D12Resource* GetDefaultResource(size_t _width, size_t _height);

	// コピー禁止
	Texture2D(const Texture2D&) = delete;
	void operator = (const Texture2D&) = delete;
public:
	/// @brief ファイルパスを渡してテクスチャを取得する関数
	/// @param _path ファイルパス
	/// @return テクスチャを返します
	static Texture2D* Get(std::string _path); 
	/// @brief ファイルパスを渡してテクスチャを取得する関数
	/// @param _path ファイルパス
	/// @return テクスチャを返します
	static Texture2D* Get(std::wstring _path);
	/// @brief 白色テクスチャを生成する関数
	/// @return テクスチャを返します
	static Texture2D* GetWhite(); 
	/// @brief 読み込みの成否を取得する関数
	/// @return 読み込みの成否を返します
	bool IsValid(); 

	/// @brief テクスチャリソースを取得する関数
	/// @return テクスチャリソースを返します
	ID3D12Resource* Resource(); 
	/// @brief シェーダーリソースビューの設定を取得する関数
	/// @return シェーダーリソースビューを返します
	D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc();
};
