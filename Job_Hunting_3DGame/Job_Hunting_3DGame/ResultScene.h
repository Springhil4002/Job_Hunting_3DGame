#pragma once
#include "BaseScene.h"

class ResultScene : public BaseScene
{
private:
public:
	/// @brief �R���X�g���N�^
	ResultScene() { Init(); };
	/// @brief �f�X�g���N�^
	~ResultScene() {};

	/// @brief �I�u�W�F�N�g�̐����֐�
	/// @param _objectID �����������I�u�W�F�N�g�̌^
	/// @return ���������I�u�W�F�N�g��Ԃ��܂�
	Object* CreateObj(const std::string& _objectID) override;

	/// @brief ����������
	void Init() override;
	/// @brief �X�V����
	void Update() override;
	/// @brief �`�揈��
	void Draw() override;
	/// @brief �I������
	void Uninit() override;
};
