#pragma once
#include <math.h>
#include <DirectXMath.h>
#include <unordered_map>
#include <string>
#include "System/CommonTypes.h"
#include "Tags.h"

class Object {
protected:
	// SRT���(�p�����)
	XMFLOAT3 m_Position = { 0.0f,0.0f,0.0f };
	XMFLOAT3 m_Rotation = { 0.0f,0.0f,0.0f };
	XMFLOAT3 m_Scale	= { 1.0f,1.0f,1.0f };
public:
	// �^�O�̃C���X�^���X
	Tags m_tags;

	Object() = default;
	~Object() = default;

	/// @brief �I�u�W�F�N�g�N���[���֐�
	/// @return �I�u�W�F�N�g�̃|�C���^��Ԃ��܂�
	virtual Object* clone() const = 0;	

	/// @brief ����������
	virtual bool Init() { return true; };
	/// @brief �X�V����
	virtual void Update() {};
	/// @brief �`�揈��
	virtual void Draw()   {};
	/// @brief �I������
	virtual void Uninit() {};

	/// @brief ���W�擾�֐�
	/// @return ���W��Ԃ��܂�
	DirectX::XMVECTOR GetPos() const;
	/// @brief ��]�擾�֐�
	/// @return ��]��Ԃ��܂�
	DirectX::XMVECTOR GetRota() const;
	/// @brief �X�P�[���擾�֐�
	/// @return �X�P�[����Ԃ��܂�
	DirectX::XMVECTOR GetScale() const;

	/// @brief ���W�ݒ�֐�
	/// @param _pos ��:DirectX::XMVECTOR(0.0f,0.0f,0.0f)
	void SetPos(DirectX::XMVECTOR _pos);
	/// @brief ��]�ݒ�֐�
	/// @param _rota ��:DirectX::XMVECTOR(0.0f,0.0f,0.0f)
	void SetRota(DirectX::XMVECTOR _rota);
	/// @brief �X�P�[���ݒ�֐�
	/// @param _scale ��:DirectX::XMVECTOR(0.0f,0.0f,0.0f)
	void SetScale(DirectX::XMVECTOR _scale);
};