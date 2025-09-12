#include "TextureManager.h"
#include "Debug_New.h"

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
		printf("TextureManager:テクスチャ取得成功 %ls\n", _path.c_str());
		return it->second;
	}
	else
	{
		printf("TextureManager:テクスチャ取得失敗 %ls\n", _path.c_str());
		return GetWhiteTexture();
	}
}

std::shared_ptr<TextureCube> TextureManager::GetCubeMap(const std::wstring& _path)
{
	auto it = m_CubeMapResource.find(_path);
	if (it != m_CubeMapResource.end())
	{
		printf("TextureManager:キューブマップ取得成功 %ls\n", _path.c_str());
		return it->second;
	}
	else
	{
		printf("TextureManager:キューブマップ取得失敗 %ls\n", _path.c_str());
		return nullptr;
	}
}

std::shared_ptr<Texture2D> TextureManager::LoadTexture(const std::wstring& _path)
{
	auto it = m_TexResource.find(_path);
	if (it != m_TexResource.end())
	{
		printf("TextureManager:既存テクスチャ %ls を再利用します\n", _path.c_str());
		return it->second;
	}

	// テクスチャ読み込み
	std::shared_ptr<Texture2D> tex = std::make_shared<Texture2D>(_path);
	if (!tex->IsValid())
	{
		printf("TextureManager:無効テクスチャ %ls →白色テクスチャに置換します\n", _path.c_str());
		tex = GetWhiteTexture();
	}
	printf("TextureManager:新規テクスチャ %ls 読み込み成功\n", _path.c_str());
	m_TexResource[_path] = tex;
	return tex;
}

std::shared_ptr<TextureCube> TextureManager::LoadCubeMap(const std::wstring& _path)
{
	auto it = m_CubeMapResource.find(_path);
	if (it != m_CubeMapResource.end())
	{
		printf("TextureManager:既存キューブマップ %ls を再利用します\n", _path.c_str());
		return it->second;
	}

	// キューブマップ読み込み
	std::shared_ptr<TextureCube> cubeMap = std::make_shared<TextureCube>(_path);
	if (!cubeMap->IsValid())
	{
		printf("TextureManager:無効キューブマップ %ls →白色テクスチャに置換します\n", _path.c_str());
		return nullptr;
	}
	printf("TextureManager:新規キューブマップ %ls 読み込み成功\n", _path.c_str());
	m_CubeMapResource[_path] = cubeMap;
	return cubeMap;
}

void TextureManager::Clear()
{
	m_TexResource.clear();
	m_CubeMapResource.clear();
	m_WhiteTexture.reset();
}