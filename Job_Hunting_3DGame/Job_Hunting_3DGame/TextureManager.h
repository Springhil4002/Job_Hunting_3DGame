#pragma once
#include <memory>
#include <unordered_map>
#include "Texture2D.h"

class TextureManager
{
private:
	std::unordered_map<std::wstring, std::shared_ptr<Texture2D>> m_TexResource;
	std::shared_ptr<Texture2D> m_WhiteTexture;

	TextureManager() = default;
	~TextureManager() = default;

	// ƒRƒs[‹Ö~
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;
public:
	static TextureManager& Instance();
	std::shared_ptr<Texture2D> LoadTexture(const std::wstring& _path);
	std::shared_ptr<Texture2D> GetWhiteTexture();
	void Clear();
};

