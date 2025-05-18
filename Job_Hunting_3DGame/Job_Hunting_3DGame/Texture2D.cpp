#include "Texture2D.h"
#include "DrawBase.h"
#include <DirectXTex.h>

#pragma comment(lib, "DirectXTex.lib")

// �}���`�o�C�g����������C�h������ɕϊ�
std::wstring GetWideString(const std::string& _str)
{
	auto num1 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, _str.c_str(), -1, nullptr, 0);

	std::wstring wstr;
	wstr.resize(num1);

	auto num2 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, _str.c_str(), -1, &wstr[0], num1);

	assert(num1 == num2);
	return wstr;
}

/// @brief �t�@�C���g���q���擾����֐�
/// @param _path �t�@�C���p�X
/// @return ���C�h�������Ԃ��܂�
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
	// �e�N�X�`���̃��[�h
	DirectX::TexMetadata meta = {};
	DirectX::ScratchImage scratch = {};
	auto ext = FileExtension(_path);

	HRESULT hr = S_FALSE;
	// �g���q�ɉ����ĉ摜��ǂݍ���
	if (ext == L"png") 
	{
		hr = LoadFromWICFile(_path.c_str(), DirectX::WIC_FLAGS_NONE, &meta, scratch);
	}
	else if (ext == L"tga") 
	{
		hr = LoadFromTGAFile(_path.c_str(), &meta, scratch);
	}
	// ���s���G���[�o��
	if (FAILED(hr))
	{
		return false;
	}

	// �ǂݍ��񂾉摜�̃��^���ƃs�N�Z�������擾
	auto img = scratch.GetImage(0, 0, 0);
	auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	auto desc = CD3DX12_RESOURCE_DESC::Tex2D(meta.format,
		meta.width,
		meta.height,
		static_cast<UINT16>(meta.arraySize),
		static_cast<UINT16>(meta.mipLevels));

	// �e�N�X�`�����\�[�X�𐶐�
	hr = g_DrawBase->Device()->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(m_pResource.ReleaseAndGetAddressOf())
	);

	// ���s���G���[�o��
	if (FAILED(hr))
	{
		return false;
	}
	// �摜�̃s�N�Z���f�[�^��GPU�������ɑ���
	hr = m_pResource->WriteToSubresource(0,
		// ���\�[�X�S�̂ɏ�������
		nullptr,		
		// �����̃�����
		img->pixels,
		// ��s������̃f�[�^��
		static_cast<UINT>(img->rowPitch),
		// �ꖇ�̉摜�S�̂̃f�[�^��
		static_cast<UINT>(img->slicePitch) 
	);
	// ���s���G���[�o��
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
		// �ǂݍ��ݎ��s�Ȃ甒�F�e�N�X�`����Ԃ�
		return GetWhite(); 
	}
	return tex;
}

Texture2D* Texture2D::GetWhite()
{
	// ���F�e�N�X�`���𐶐�
	ID3D12Resource* buff = GetDefaultResource(4, 4);
	// �s�N�Z���S�̂�0xff(RGBA:ALL255)��ݒ�
	std::vector<unsigned char> data(4 * 4 * 4);
	std::fill(data.begin(), data.end(), 0xff);
	// ������e�N�X�`����GPU�ɃA�b�v���[�h
	auto hr = buff->WriteToSubresource(0, nullptr, data.data(), 4 * 4, data.size());
	if (FAILED(hr))
	{
		return nullptr;
	}

	return new Texture2D(buff);;
}

ID3D12Resource* Texture2D::GetDefaultResource(size_t _width, size_t _height)
{
	// �e�N�X�`�����\�[�X�̐ݒ�
	auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		// 32�r�b�g��RGBA�J���[:�e8�r�b�g
		DXGI_FORMAT_R8G8B8A8_UNORM, 
		// �e�N�X�`���̕��ƍ���
		_width, _height
	);

	// �e�N�X�`���̃s�[�v�v���p�e�B�ݒ�
	auto texHeapProp = CD3DX12_HEAP_PROPERTIES(
		// CPU�����ڏ������߂�悤�ȃ�����
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, 
		// CPU�A�N�Z�X�\�ȃs�[�v
		D3D12_MEMORY_POOL_L0
	);

	ID3D12Resource* buff = nullptr;
	auto result = g_DrawBase->Device()->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE, 
		&resDesc,
		// �s�N�Z���V�F�[�_�[�p�̃e�N�X�`���Ƃ��Ďg����悤��
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
	// �V�F�[�_�[���\�[�X�r���[�̏�����
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	// �e�N�X�`�����\�[�X�̃t�H�[�}�b�g��ݒ�
	desc.Format = m_pResource->GetDesc().Format;
	// �V�F�[�_�[�Ƀe�N�X�`���̃}�b�s���O�ǂݍ��݂��w��(��{�I�ɂ̓f�t�H���g)
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	// ���\�[�X�̎����w��:2D�e�N�X�`��
	desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; 
	// �~�b�v�}�b�v���x���̐�:1(�~�b�v�}�b�v���g�p���Ȃ�)
	desc.Texture2D.MipLevels = 1; 
	// SRV�ݒ��Ԃ�
	return desc;
}