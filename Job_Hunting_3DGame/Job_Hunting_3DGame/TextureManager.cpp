#include "TextureManager.h"

TextureManager& TextureManager::Instance()
{
	static TextureManager instance;
	return instance;
}

std::shared_ptr<Texture2D> TextureManager::LoadTexture(const std::wstring& _path)
{
	auto it = m_TexResource.find(_path);
	if (it != m_TexResource.end())
	{
		// ���ɂ���Ȃ炻���Ԃ�
		return it->second;
	}

	// �e�N�X�`���ǂݍ���
	std::shared_ptr<Texture2D> tex = std::make_shared<Texture2D>(_path);
	if (!tex->IsValid())
	{
		// �����ȏꍇ�́A���F�e�N�X�`���ɒu��������
		tex = GetWhiteTexture();
	}
	m_TexResource[_path] = tex;
	return tex;
}

std::shared_ptr<Texture2D> TextureManager::GetWhiteTexture()
{
	if (!m_WhiteTexture)
	{
		m_WhiteTexture = std::make_shared<Texture2D>(Texture2D::GetWhite()->Resource());
	}
	return m_WhiteTexture;
}

void TextureManager::Clear()
{
	m_TexResource.clear();
	m_WhiteTexture.reset();
}