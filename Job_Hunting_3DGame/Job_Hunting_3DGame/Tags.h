#pragma once
#include<string>
#include<set>
#include<vector>
// ������t�^�A�폜�A�����ł���
class Tags
{
private:
	// �^�O���i�[����z��
	std::set<std::string> m_tags;
public:	
	/// @brief �R���X�g���N�^
	Tags() = default;
	/// @brief �f�X�g���N�^
	~Tags() = default;

	std::set<std::string>::const_iterator begin() const;
	std::set<std::string>::const_iterator end() const;

	/// @brief �^�O�̒ǉ��֐�
	/// @param _tag �ǉ��������^�O��
	void AddTag(const std::string _tag);
	/// @brief �^�O�̎w��폜�֐�
	/// @param _tag ���������^�O��
	void RemoveTag(const std::string _tag);		
	/// @brief �^�O��T���֐�
	/// @param _tag �T�������^�O��
	/// @return ���v����^�O�̃I�u�W�F�N�g�̐���Ԃ��܂�
	bool SearchTag(const std::string _tag) const;	
	/// @brief �^�O�̑S�폜
	void ClearTags();								
};

