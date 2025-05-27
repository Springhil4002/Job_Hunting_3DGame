#pragma once
#include "BaseScene.h"

class ResultScene : public BaseScene
{
private:
	Camera* camera;
public:
	/// @brief �R���X�g���N�^
	ResultScene() = default;
	ResultScene(Camera* _camera) { Init(_camera); }
	/// @brief �f�X�g���N�^
	~ResultScene() = default;

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
