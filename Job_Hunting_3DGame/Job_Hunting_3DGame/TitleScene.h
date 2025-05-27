#pragma once
#include "BaseScene.h"

class TitleScene :public BaseScene
{
private:
	Camera* camera;
public:
	/// @brief �R���X�g���N�^
	TitleScene() = default;
	TitleScene(Camera* _camera) { Init(_camera); }
	/// @brief �f�X�g���N�^
	~TitleScene() = default;

	/// @brief �I�u�W�F�N�g�̐����֐�
	/// @param _objectID �����������I�u�W�F�N�g�̌^
	/// @return ���������I�u�W�F�N�g��Ԃ��܂�
	Object* CreateObj(const std::string& _objectID) override;

	/// @brief ����������
	void Init() {};
	void Init(Camera* _camera);
	/// @brief �X�V����
	void Update() override;
	/// @brief �`�揈��
	void Draw() override;
	/// @brief �I������
	void Uninit() override;
};