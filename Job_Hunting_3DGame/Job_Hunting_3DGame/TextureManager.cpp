#include "TextureManager.h"
#include "Debug_New.h"
#include "Debug_Msg.h"

TextureManager& TextureManager::Instance()
{
	static TextureManager instance;
	return instance;
}

std::shared_ptr<Texture2D> TextureManager::GetWhiteTexture()
{
	if (!m_WhiteTexture)
	{
		m_WhiteTexture = std::make_shared<Texture2D>(Texture2D::GetWhite()->Resource());
	}
	return m_WhiteTexture;
}

std::shared_ptr<Texture2D> TextureManager::GetTexture(const std::wstring& _path)
{
	auto it = m_TexResource.find(_path);
	if (it != m_TexResource.end())
	{
		DEBUG_LOG(L"TextureManager:テクスチャ取得成功 ({})", _path);
		return it->second;
	}
	else
	{
		DEBUG_LOG_ERROR(L"TextureManager:テクスチャ取得失敗 ({})", _path);
		return GetWhiteTexture();
	}
}

std::shared_ptr<TextureCube> TextureManager::GetCubeMap(const std::wstring& _path)
{
	auto it = m_CubeMapResource.find(_path);
	if (it != m_CubeMapResource.end())
	{
		DEBUG_LOG(L"TextureManager:キューブマップ取得成功 ({})", _path);
		return it->second;
	}
	else
	{
		DEBUG_LOG_ERROR(L"TextureManager:キューブマップ取得失敗 ({})", _path);
		return nullptr;
	}
}

std::shared_ptr<Texture2D> TextureManager::LoadTexture(const std::wstring& _path)
{
	// テクスチャ読み込み
	std::shared_ptr<Texture2D> tex = std::make_shared<Texture2D>(_path);
	if (!tex->IsValid())
	{
		DEBUG_LOG_ERROR(L"TextureManager:無効テクスチャ ({}) -> 白色に置換", _path);
		tex = GetWhiteTexture();
	}
	DEBUG_LOG(L"TextureManager:新規テクスチャ ({}) 読み込み成功", _path);
	m_TexResource[_path] = tex;
	return tex;
}

std::shared_ptr<TextureCube> TextureManager::LoadCubeMap(const std::wstring& _path)
{
	// キューブマップ読み込み
	std::shared_ptr<TextureCube> cubeMap = std::make_shared<TextureCube>(_path);
	if (!cubeMap->IsValid())
	{
		DEBUG_LOG_ERROR(L"TextureManager:無効キューブマップ ({}) -> 白色に置換", _path);
		return nullptr;
	}
	DEBUG_LOG(L"TextureManager:新規キューブマップ ({}) 読み込み成功", _path);
	m_CubeMapResource[_path] = cubeMap;
	return cubeMap;
}

void TextureManager::Clear()
{
	m_TexResource.clear();
	m_CubeMapResource.clear();
	m_WhiteTexture.reset();
}