#include "Texture2D.h"
#include "DrawBase.h"
#include <DirectXTex.h>

#pragma comment(lib, "DirectXTex.lib")

// マルチバイト文字列をワイド文字列に変換
std::wstring GetWideString(const std::string& _str)
{
	auto num1 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, _str.c_str(), -1, nullptr, 0);

	std::wstring wstr;
	wstr.resize(num1);

	auto num2 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, _str.c_str(), -1, &wstr[0], num1);

	assert(num1 == num2);
	return wstr;
}

/// @brief ファイル拡張子を取得する関数
/// @param _path ファイルパス
/// @return ワイド文字列を返します
std::wstring FileExtension(const std::wstring& _path)
{
	auto idx = _path.rfind(L'.');
	return _path.substr(idx + 1, _path.length() - idx - 1);
}

Texture2D::Texture2D(std::string _path)
{
	m_IsValid = Load(_path);
}

Texture2D::Texture2D(std::wstring _path)
{
	m_IsValid = Load(_path);
}

Texture2D::Texture2D(ID3D12Resource* _buffer)
{
	m_pResource = _buffer;
	m_IsValid = m_pResource != nullptr;
}

bool Texture2D::Load(std::string& _path)
{
	auto wpath = GetWideString(_path);
	return Load(wpath);
}

bool Texture2D::Load(std::wstring& _path)
{
	// テクスチャのロード
	DirectX::TexMetadata meta = {};
	DirectX::ScratchImage scratch = {};
	auto ext = FileExtension(_path);

	HRESULT hr = S_FALSE;
	// 拡張子に応じて画像を読み込む
	if (ext == L"png") 
	{
		hr = LoadFromWICFile(_path.c_str(), DirectX::WIC_FLAGS_NONE, &meta, scratch);
	}
	else if (ext == L"tga") 
	{
		hr = LoadFromTGAFile(_path.c_str(), &meta, scratch);
	}
	// 失敗時エラー出力
	if (FAILED(hr))
	{
		return false;
	}

	// 読み込んだ画像のメタ情報とピクセル情報を取得
	auto img = scratch.GetImage(0, 0, 0);
	auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	auto desc = CD3DX12_RESOURCE_DESC::Tex2D(meta.format,
		meta.width,
		meta.height,
		static_cast<UINT16>(meta.arraySize),
		static_cast<UINT16>(meta.mipLevels));

	// テクスチャリソースを生成
	hr = g_DrawBase->Device()->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(m_pResource.ReleaseAndGetAddressOf())
	);

	// 失敗時エラー出力
	if (FAILED(hr))
	{
		return false;
	}
	// 画像のピクセルデータをGPUメモリに送る
	hr = m_pResource->WriteToSubresource(0,
		// リソース全体に書きこみ
		nullptr,		
		// 送り先のメモリ
		img->pixels,
		// 一行あたりのデータ量
		static_cast<UINT>(img->rowPitch),
		// 一枚の画像全体のデータ量
		static_cast<UINT>(img->slicePitch) 
	);
	// 失敗時エラー出力
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

Texture2D* Texture2D::Get(std::string _path)
{
	auto wpath = GetWideString(_path);
	return Get(wpath);
}

Texture2D* Texture2D::Get(std::wstring _path)
{
	auto tex = new Texture2D(_path);
	if (!tex->IsValid())
	{
		// 読み込み失敗なら白色テクスチャを返す
		return GetWhite(); 
	}
	return tex;
}

Texture2D* Texture2D::GetWhite()
{
	// 白色テクスチャを生成
	ID3D12Resource* buff = GetDefaultResource(4, 4);
	// ピクセル全体に0xff(RGBA:ALL255)を設定
	std::vector<unsigned char> data(4 * 4 * 4);
	std::fill(data.begin(), data.end(), 0xff);
	// 作ったテクスチャをGPUにアップロード
	auto hr = buff->WriteToSubresource(0, nullptr, data.data(), 4 * 4, data.size());
	if (FAILED(hr))
	{
		return nullptr;
	}

	return new Texture2D(buff);;
}

ID3D12Resource* Texture2D::GetDefaultResource(size_t _width, size_t _height)
{
	// テクスチャリソースの設定
	auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		// 32ビットのRGBAカラー:各8ビット
		DXGI_FORMAT_R8G8B8A8_UNORM, 
		// テクスチャの幅と高さ
		_width, _height
	);

	// テクスチャのピーププロパティ設定
	auto texHeapProp = CD3DX12_HEAP_PROPERTIES(
		// CPUが直接書き込めるようなメモリ
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, 
		// CPUアクセス可能なピープ
		D3D12_MEMORY_POOL_L0
	);

	ID3D12Resource* buff = nullptr;
	auto result = g_DrawBase->Device()->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE, 
		&resDesc,
		// ピクセルシェーダー用のテクスチャとして使えるように
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&buff)
	);
	
	if (FAILED(result))
	{
		assert(SUCCEEDED(result));
		return nullptr;
	}
	return buff;
}

bool Texture2D::IsValid()
{
	return m_IsValid;
}

ID3D12Resource* Texture2D::Resource()
{
	return m_pResource.Get();
}

D3D12_SHADER_RESOURCE_VIEW_DESC Texture2D::ViewDesc()
{
	// シェーダーリソースビューの初期化
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	// テクスチャリソースのフォーマットを設定
	desc.Format = m_pResource->GetDesc().Format;
	// シェーダーにテクスチャのマッピング読み込みを指定(基本的にはデフォルト)
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	// リソースの次元指定:2Dテクスチャ
	desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; 
	// ミップマップレベルの数:1(ミップマップを使用しない)
	desc.Texture2D.MipLevels = 1; 
	// SRV設定を返す
	return desc;
}