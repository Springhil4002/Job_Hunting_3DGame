#pragma once
#include <d3dx12.h>
#include "ComPtr.h"

class ConstantBuffer
{
private:
    // �萔�o�b�t�@�����ɐ����������̃t���O
    bool m_IsValid = false; 
    // �萔�o�b�t�@
    ComPtr<ID3D12Resource> m_pBuffer; 
    // �萔�o�b�t�@�r���[�̐ݒ�
    D3D12_CONSTANT_BUFFER_VIEW_DESC m_Desc; 
    void* m_pMappedPtr = nullptr;

    // �R�s�[�֎~
    ConstantBuffer(const ConstantBuffer&) = delete;
    void operator = (const ConstantBuffer&) = delete;
public:
    /// @brief  �R���X�g���N�^�Œ萔�o�b�t�@�𐶐�
    /// @param size �o�b�t�@�T�C�Y
    ConstantBuffer(size_t _size); 
    /// @brief �o�b�t�@�����ɐ��۔��菈��
    /// @return �o�b�t�@�����̐��ۂ�Ԃ��܂�
    bool IsValid(); 

    /// @brief �o�b�t�@��GPU��̃A�h���X�擾����
    /// @return �o�b�t�@��GPU��̃A�h���X��Ԃ��܂�
    D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const; 
    /// @brief �萔�o�b�t�@�r���[�擾����
    /// @return �萔�o�b�t�@�r���[��Ԃ�
    D3D12_CONSTANT_BUFFER_VIEW_DESC ViewDesc(); 
    /// @brief �萔�o�b�t�@�Ƀ}�b�s���O���ꂽ�|�C���^���擾
    /// @return �萔�o�b�t�@�Ƀ}�b�s���O���ꂽ�|�C���^��Ԃ�
    void* GetPtr() const; 

    // �e���v���[�g
    template<typename T>
    T* GetPtr()
    {
        return reinterpret_cast<T*>(GetPtr());
    }
};
