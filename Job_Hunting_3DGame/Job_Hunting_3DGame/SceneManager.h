#pragma once
#include "SceneFactory.h"

class SceneManager
{
private:
	/// @brief ���݂̃V�[��
	static BaseScene* currentScene;
	/// @brief �V�[�������t�@�N�g���[
	static SceneFactory sceneFactory;
	/// @brief �V�[���؂�ւ��t���O
	static bool change;
	/// @brief ���������I�u�W�F�N�g���i�[����z��
	static std::set<Object*> createObjects;
	/// @brief ���������I�u�W�F�N�g���i�[����z��
	static std::set<Object*> deleteObjects;
	
	/// @brief �R���X�g���N�^�A�O������̃C���X�^���X�������֎~
	SceneManager()
	{
		currentScene = sceneFactory.CreateScene(SCENE_ID_TITLE);
	}
	/// @brief �f�X�g���N�^�A�O������̍폜���֎~
	~SceneManager() = default;	
public:
	/// @brief ����������
	void Init();
	/// @brief �X�V����
	void Update();
	/// @brief �`�揈��
	void Draw();
	/// @brief �I������
	void Uninit();

	/// @brief �V�[���J�ڊ֐�
	/// @param _scene_ID �J�ڂ������V�[����ID
	static void ChangeScene(SCENE_ID _scene_ID);

	/// @brief �ǉ����ꂽ�I�u�W�F�N�g�̐���
	void Create();
	/// @brief �ǉ����ꂽ�I�u�W�F�N�g�̍폜
	void Delete();
};
