#pragma once
#include "Object.h"

class Model3D : public Object
{
private:
public:
	// �N���[�����\�b�h
	Object* clone() const override;

	/// @brief ����������
	/// @return �����������̐��ۂ�Ԃ��܂�
	bool Init()		override;
	/// @brief �X�V����
	void Update()	override;
	/// @brief �`�揈��
	void Draw()		override;
	/// @brief �I������
	void Uninit()	override;
};

extern Model3D* g_Model3D;