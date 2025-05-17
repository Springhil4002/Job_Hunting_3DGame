#pragma once
#define NOMINMAX
#include <d3d12.h>
#include <DirectXMath.h>
#include <string>
#include <vector>

struct Mesh;
struct Vertex;

// Assimp�̍\����
struct aiMesh;
struct aiMaterial;

// �C���|�[�g����Ƃ��̃p�����[�^
struct ImportSettings
{
	// �ǂݍ��ރ��f���̃t�@�C���p�X
	const wchar_t* fileName = nullptr;	
	// �ǂݍ��񂾃��b�V��������z��
	std::vector<Mesh>& meshs;
	// U���W�𔽓]�����邩
	bool inverse_U = false;
	// V���W�𔽓]�����邩
	bool inverse_V = false;
};

class AssimpLoader
{
private:
	/// @brief ���b�V���ǂݍ��݊֐�
	/// @param _dst ���b�V���f�[�^�̔z��
	/// @param _src Assimp�̃��b�V���\����
	/// @param _inverse_U U���W�̔��]�t���O
	/// @param _inverse_V V���W�̔��]�t���O
	void LoadMesh(Mesh& _dst, const aiMesh* _src, bool _inverse_U, bool _inverse_V);
	/// @brief �e�N�X�`���ǂݍ��݊֐�
	/// @param _fileName ���f���t�@�C���p�X��
	/// @param _dst ���b�V���\����
	/// @param _src Assimp�̃}�e���A���\����
	void LoadTexture(const wchar_t* _fileName, Mesh& _dst, const aiMaterial* _src);
public:
	/// @brief ���f����ǂݍ��ފ֐�
	/// @param _Setting ���f����ǂݍ��ނ̂ɕK�v�ȃp�����[�^�Q
	/// @return �ǂݍ��݂̐��ۂ�Ԃ��܂�
	bool Load(ImportSettings _Setting);
};

