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
	XMFLOAT3 targent;	// �ڋ��
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
};