#pragma once
#include "System/ComPtr.h"
#include <string>
#include <d3dx12.h>

class DescriptorHandle;
class DescriptorHeap;

class TextureCube
{
private:
	// 読み込みの成否フラグ
	bool m_IsValid;
	// テクスチャ情報を保持するリソース
	ComPtr<ID3D12Resource> m_pResource;
	/// @brief ファイルパスを読み込んでテクスチャリソースを作成
	/// @param _path ファイルパス
	/// @return 作成の成否を返します
	bool Load(std::string& _path);
	/// @brief ファイルパスを読み込んでテクスチャリソースを作成
	/// @param _path ファイルパス
	/// @return 作成の成否を返します
	bool LoadFromFile(const std::wstring& _path);
	/// @brief 色なしのテクスチャなどのリソースを作る関数
	/// @param _width 横幅
	/// @param _height 高さ
	/// @return テクスチャリソースを返します
	static ID3D12Resource* GetDefaultResource(size_t _width, size_t _height);

	// コピー禁止
	TextureCube(const TextureCube&) = delete;
	void operator = (const TextureCube&) = delete;
public:
	/// @brief コンストラクタ
	/// @param _path ファイルパス
	TextureCube(std::string _path);
	/// @brief コンストラクタ
	/// @param _path ファイルパス
	TextureCube(std::wstring _path);
	/// @brief コンストラクタ
	/// @param _buffer リソース
	TextureCube(ID3D12Resource* _buffer);
	/// @brief ファイルパスを渡してテクスチャを取得する関数
	/// @param _path ファイルパス
	/// @return テクスチャを返します
	static std::shared_ptr<TextureCube> Get(std::string _path);
	/// @brief ファイルパスを渡してテクスチャを取得する関数
	/// @param _path ファイルパス
	/// @return テクスチャを返します
	static std::shared_ptr<TextureCube> Get(std::wstring _path);
	/// @brief 白色テクスチャを生成する関数
	/// @return テクスチャを返します
	static std::shared_ptr<TextureCube> GetWhite();
	/// @brief 読み込みの成否を取得する関数
	/// @return 読み込みの成否を返します
	bool IsValid() const;

	/// @brief テクスチャリソースを取得する関数
	/// @return テクスチャリソースを返します
	ID3D12Resource* Resource() const;
	/// @brief シェーダーリソースビューの設定を取得する関数
	/// @return シェーダーリソースビューを返します
	D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() const;
};