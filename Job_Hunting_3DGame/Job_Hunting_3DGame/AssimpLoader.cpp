#include "AssimpLoader.h"
#include "System/SharedStruct.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <d3dx12.h>
#include <filesystem>

namespace fs = std::filesystem;

/// @brief �t�@�C���p�X���󂯎��A�f�B���N�g��������Ԃ��֐�
/// @param _origin �t�@�C���p�X
/// @return �f�B���N�g������
std::wstring GetDirectoryPath(const std::wstring& _origin)
{
	fs::path p = _origin;
	return p.remove_filename().wstring();
}

/// @brief UTF-16�̕������UTF-8�ɕϊ����鏈��
/// @param _value UTF-16������
/// @return UTF-8��std::string��Ԃ�
std::string ToUTF8(const std::wstring& _value)
{
	// �o�b�t�@�T�C�Y�̎擾
	auto length = WideCharToMultiByte(
		CP_UTF8, 0U, _value.data(), -1, nullptr, 0, nullptr, nullptr);

	if (length == 0) return{};
	
	// ��������������݂��߂̃o�b�t�@�m��
	std::vector<char> buffer(length);

	// ������o�b�t�@�ɕϊ����������������
	WideCharToMultiByte(
		CP_UTF8, 0U, _value.data(), -1, buffer.data(), length, nullptr, nullptr);
	
	return std::string(buffer.data());
}

/// @brief std::string(�}���`�o�C�g������)����std::wstring(���C�h������)�ɕϊ�����֐�
/// @param _str std::string������
/// @return std::wstring�������Ԃ��܂�
std::wstring ToWideString(const std::string& _str)
{
	// �ϊ���̕��������擾
	auto num = MultiByteToWideChar(
		CP_UTF8, 0, _str.c_str(), -1, nullptr, 0);
	if (num == 0) return {};

	// �K�؂ȃT�C�Y�̔z����m��
	std::wstring wstr(num, 0);

	// ������z��ɕϊ���̕����������
	auto conv= MultiByteToWideChar(
		CP_UTF8, 0, _str.c_str(), -1, &wstr[0], num);
	if (conv == 0) return {};
	
	// null�����I�[�����O
	wstr.pop_back();
	return wstr;
}

/// @brief 3D���f����ǂݍ��݁AMesh�\���̂ɕϊ�����֐�
/// @param _settings �ǂݍ��݂ɕK�v�ȍ\����
/// @return �ǂݍ��݂̐��ۂ�Ԃ��܂�
bool AssimpLoader::Load(ImportSettings _settings)
{
	// null�`�F�b�N
	if (_settings.fileName == nullptr)
	{
		return false;
	}

	// �\���̂̊e�����o��ϐ��ɑ�����Ĉ����₷������
	auto& meshes = _settings.meshs;
	auto inverse_U = _settings.inverse_U;
	auto inverse_V = _settings.inverse_V;
	// �t�@�C���p�X��ϊ�����Assimp�ł����Ƃ��悤�ɂ���
	auto path = ToUTF8(_settings.fileName);

	// Assimp�̓ǂݍ��ݐݒ�
	Assimp::Importer importer;
	unsigned int flag = 0;
	// �O�p�`�ɕϊ�
	flag |= aiProcess_Triangulate;
	// ���ׂẴm�[�h�ϊ���K�p
	flag |= aiProcess_PreTransformVertices;
	// �^���W�F���g�E�o�C�^���W�F���g����
	flag |= aiProcess_CalcTangentSpace;
	// �X���[�Y�Ȗ@������
	flag |= aiProcess_GenSmoothNormals;
	// UV���W�𐶐�
	flag |= aiProcess_GenUVCoords;
	// �g���ĂȂ��}�e���A��������
	flag |= aiProcess_RemoveRedundantMaterials;
	// Mesh�𓝍��E����
	flag |= aiProcess_OptimizeMeshes;

	// �t�@�C����ǂݍ���ŃV�[���f�[�^�𓾂�
	auto scene = importer.ReadFile(path, flag);

	// �ǂݍ��ݎ��s���̓G���[�o��
	if (scene == nullptr)
	{
		// �����ǂݍ��݃G���[���ł���\������
		printf(importer.GetErrorString());
		printf("\n");
		return false;
	}

	// Mesh�\���̂�������
	meshes.clear();
	// �ǂݍ��񂾃��b�V���̐��������m��
	meshes.resize(scene->mNumMeshes);
	
	// �ǂݍ��񂾃f�[�^�������Œ�`����Mesh�\���̂ɕϊ�����
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		const auto pMesh = scene->mMeshes[i];
		LoadMesh(meshes[i], pMesh, inverse_U, inverse_V);
		const auto pMaterial = scene->mMaterials[i];
		LoadTexture(_settings.fileName, meshes[i], pMaterial);
	}

	scene = nullptr;

	return true;
}

/// @brief Assimp�œǂݍ��񂾃��b�V���f�[�^������Mesh�N���X�ɕϊ�����֐�
/// @param _dst ���상�b�V���\����
/// @param _src Assimp���b�V���\����
/// @param _inverse_U U���W�̔��]�t���O
/// @param _inverse_V V���W�̔��]�t���O
void AssimpLoader::LoadMesh(Mesh& _dst, const aiMesh* _src, bool _inverse_U, bool _inverse_V)
{
	// ���b�V���ɏ�񂪖����悤�ɑ�֒l��p��
	aiVector3D zero3D(0.0f, 0.0f, 0.0f);
	aiColor4D zeroColor(0.0f, 0.0f, 0.0f, 0.0f);

	// �擾�������_�����A�z����m��
	_dst.Vertices.resize(_src->mNumVertices);

	// ���_�����̊e���_�f�[�^��ϊ�
	for (auto i = 0u; i < _src->mNumVertices; ++i)
	{
		auto position = &(_src->mVertices[i]);
		auto normal	= &(_src->mNormals[i]);
		auto uv = (_src->HasTextureCoords(0)) ? &(_src->mTextureCoords[0][i]) : &zero3D;
		auto tangent = (_src->HasTangentsAndBitangents()) ? &(_src->mTangents[i]) : &zero3D;
		auto color = (_src->HasVertexColors(0)) ? &(_src->mColors[0][i]) : &zeroColor;

		// ���]�t���O����������UV�𔽓]
		if (_inverse_U)
		{
			uv->x = 1 - uv->x;
		}
		if (_inverse_V)
		{
			uv->y = 1 - uv->y;
		}

		// DirectX�`���ɕϊ�
		Vertex vertex = {};
		vertex.position = XMFLOAT3(position->x, position->y, position->z);
		vertex.normal	= XMFLOAT3(normal->x, normal->y, normal->z);
		vertex.uv		= XMFLOAT2(uv->x, uv->y);
		vertex.tangent	= XMFLOAT3(tangent->x, tangent->y, tangent->z);
		vertex.color	= XMFLOAT4(color->r, color->g, color->b, color->a);

		_dst.Vertices[i] = vertex;
	}

	// �|���S���P�ʂ��O�p�`�Ƃ���
	_dst.Indices.resize(_src->mNumFaces * 3);
	// �e�|���S���̒��_�C���f�b�N�X����̘A�������z��ɓ����
	for (auto i = 0u; i < _src->mNumFaces; ++i)
	{
		const auto& face = _src->mFaces[i];

		_dst.Indices[i * 3 + 0] = face.mIndices[0];
		_dst.Indices[i * 3 + 1] = face.mIndices[1];
		_dst.Indices[i * 3 + 2] = face.mIndices[2];
	}
}

/// @brief Assimp�œǂݍ��񂾃}�e���A������e�N�X�`���Ƀp�X������Mesh�\���̂ɐݒ肷��֐�
/// @param _filename ���f���t�@�C���̃p�X
/// @param _dst Mesh�\����
/// @param _src Assimp�œǂݍ��񂾃}�e���A�����
void AssimpLoader::LoadTexture(const wchar_t* _filename, Mesh& _dst, const aiMaterial* _src)
{
	// �e�N�X�`���̃p�X���擾
	aiString path;
	if (_src->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)
	{
		// ���΃p�X�ł���e�N�X�`���p�X���t���p�X�ɕϊ�����
		auto dir = GetDirectoryPath(_filename);
		auto file = std::string(path.C_Str());
		_dst.DiffuseMap = dir + ToWideString(file);
	}
	else
	{
		// �e�N�X�`�����Ȃ��Ȃ�N���A
		_dst.DiffuseMap.clear();
	}
}