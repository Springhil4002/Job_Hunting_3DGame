#pragma once
#include "Object.h"
#include "Model3D.h"
#include "WaterMesh.h"

class PrototypeManager
{
private:
	std::unordered_map<std::string, Object*> prototypes;
public:
	/// @brief �V�K�v���g�^�C�v�ǉ��֐�
	/// @param _key �v���g�^�C�v�̎��ʗp������
	/// @param _prototype �ǉ�����v���g�^�C�v�̌^
	void AddPrototype(const std::string& _key, Object* _prototype)
	{
		prototypes[_key] = _prototype;
	}

	/// @brief ����̃v���g�^�C�v�̃N���[���쐬����
	/// @param _objectID �쐬�������N���[���̌^
	/// @return �쐬�����N���[���̌^��Ԃ��܂�
	Object* Create(const std::string& _objectID)
	{
		// �Y������v���g�^�C�v�̓o�^���`�F�b�N
		if (prototypes.find(_objectID) != prototypes.end())
		{
			// �N���[������
			return prototypes[_objectID]->clone();
		}
		// �Y������v���g�^�C�v���Ȃ�
		return nullptr;
	}
};