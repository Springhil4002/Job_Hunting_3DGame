#pragma once
#include <d3dx12.h>
#include "CommonTypes.h"
#include "ComPtr.h"

// ���_�f�[�^�̒�`
struct Vertex
{
	XMFLOAT3 position;	// �ʒu
	XMFLOAT3 normal;	// �@��
	XMFLOAT2 uv;		// UV���W
	XMFLOAT3 tangent;	// �ڋ��
	XMFLOAT4 color;		// ���_�J���[

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;
private:
	static const int InputElementCount = 5;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

// �ϊ��s��̒�`
struct alignas(256) Matrix
{
	Matrix4x4 world;	// ���[���h�s��
	Matrix4x4 view;		// �r���[�s��
	Matrix4x4 proj;		// ���e�s��

	float time;			// �g�̂��߂̎���
	float padding[3];	// 16�o�C�g�A���C�����g
};

struct GerstnerParams
{
	DirectX::XMFLOAT4 amplitude[4];		// x=�U��
	DirectX::XMFLOAT4 direction[4];		// xy=����
	DirectX::XMFLOAT4 waveLength[4];	// x=�g��
	DirectX::XMFLOAT4 speed[4];			// x=���x
};

struct Mesh
{
	std::vector<Vertex> Vertices;	// ���_�f�[�^�̔z��
	std::vector<uint32_t> Indices;	// �C���f�b�N�X�̔z��
	std::wstring DiffuseMap;		// �e�N�X�`���̃t�@�C���p�X
};