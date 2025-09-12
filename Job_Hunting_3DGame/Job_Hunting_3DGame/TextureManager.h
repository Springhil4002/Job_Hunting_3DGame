#pragma once
#include <memory>
#include <unordered_map>
#include "Texture2D.h"
#include "TextureCube.h"

class TextureManager
{
private:
	std::unordered_map<std::wstring, std::shared_ptr<Texture2D>> m_TexResource;
	std::unordered_map<std::wstring, std::shared_ptr<TextureCube>> m_CubeMapResource;
	std::shared_ptr<Texture2D> m_WhiteTexture;

	TextureManager() = default;
	~TextureManager() = default;

	// ƒRƒs[‹Ö~
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;
public:
	static TextureManager& Instance();
	std::shared_ptr<Texture2D> GetWhiteTexture();
	std::shared_ptr<Texture2D> GetTexture(const std::wstring& _path);
	std::shared_ptr<TextureCube> GetCubeMap(const std::wstring& _path);
	std::shared_ptr<Texture2D> LoadTexture(const std::wstring& _path);
	std::shared_ptr<TextureCube> LoadCubeMap(const std::wstring& _path);
	void Clear();
};

