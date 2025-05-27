#pragma once
#include <stdio.h>
#include <set>
#include "System/Input.h"
#include "PrototypeManager.h"

#define MAX_OBJECT (1)

class BaseScene
{
protected:
	// �V�[���̃C���X�^���X
	static BaseScene* sceneInstance;	
	// �I�u�W�F�N�g���Ǘ�����C���X�^���X
	std::set<Object*> objectInstance;
public:
	// ���͌n�C���X�^���X
	static Input input;
	
	// �I�u�W�F�N�g�����Ɋւ���C���X�^���X
	PrototypeManager* prototypeManager = new PrototypeManager;

	/// @brief �R���X�g���N�^
	BaseScene() {
		delete sceneInstance;
		sceneInstance = this;
	}

	/// @brief �f�X�g���N�^
	virtual ~BaseScene() {
		sceneInstance = nullptr;
		delete prototypeManager;
	}

	/// @brief ����������
	virtual void Init() = 0;
	/// @brief �X�V����
	virtual void Update() = 0;
	/// @brief �`�揈��
	virtual void Draw() = 0;
	/// @brief �I������
	virtual void Uninit() = 0;

	/// @brief ���͎擾�����֐�
	static void Input();

	/// @brief �I�u�W�F�N�g�̐����֐�
	/// @param _ObjectID �����������I�u�W�F�N�g�̎��ʎq
	/// @return �����������I�u�W�F�N�g��Ԃ��܂�
	virtual Object* CreateObj(const std::string& _ObjectID) = 0;

	/// @brief ���݂̃V�[���C���X�^���X���擾����֐�
	/// @return ���݂̃V�[���C���X�^���X��Ԃ��܂�
	static BaseScene* GetInstance();

	/// @brief �����_�̑��݂��Ă���S�ẴI�u�W�F�N�g���擾����֐�
	/// @return �����_�̑��݂��Ă���S�ẴI�u�W�F�N�g��Ԃ��܂�
	std::set<Object*>* GetObjects();

	/// @brief ����̌^�����I�u�W�F�N�g���擾����֐�
	/// @tparam T �擾�������^
	/// @return �擾�������^�����S�ẴI�u�W�F�N�g
	template <class T>
	std::vector<T*> GetObjects()
	{
		// �擾�������^�����I�u�W�F�N�g���i�[����z��
		std::vector<T*> objects;

		// ���݂��Ă���S�ẴI�u�W�F�N�g����T��
		for (auto& obj : objectInstance)
		{
			// �^��v�Ȃ�z��ɒǉ�
			if (T* castedObj = dynamic_cast<T*>(obj)) {
				objects.push_back(castedObj);
			}
		}
		return objects;
	}
};

