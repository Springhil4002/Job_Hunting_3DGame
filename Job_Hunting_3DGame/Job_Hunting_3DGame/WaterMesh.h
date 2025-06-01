#pragma once
#include "Object.h"
#include "DrawBase.h"
#include "App.h"
#include <d3dx12.h>
#include "System/SharedStruct.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "RootSignature_WaterMesh.h"
#include "PipelineState_WaterMesh.h"
#include "DescriptorHeap.h"
#include "Texture2D.h"
#include "TextureManager.h"
#include "Camera.h"

class WaterMesh : public Object
{
private:
	// ���_�o�b�t�@
	VertexBuffer* m_pVertexBuffer;
	// �C���f�b�N�X�o�b�t�@
	IndexBuffer* m_pIndexBuffer;
	// �R���X�^���g�o�b�t�@
	ConstantBuffer* m_pConstantBuffer[DrawBase::FRAME_BUFFER_COUNT];
	// �g�p�萔�o�b�t�@�ƃp�����[�^
	ConstantBuffer* m_pWaveBuffer;
	GerstnerParams m_waveParams{};
	// �f�B�X�N���v�^�q�[�v
	DescriptorHeap* m_pDescriptorHeap;
	// ���[�g�V�O�l�`��
	RootSignature_WaterMesh* m_pRootSignature;
	// �p�C�v���C���X�e�[�g
	PipelineState_WaterMesh* m_pPipelineState;
	// �J����
	Camera* m_camera;
	// �f�B�X�N���v�^�n���h��
	DescriptorHandle* m_pTexHandle;
	// �g�̂��߂̎���
	float g_time = 0.0f;
public:
	// ���ʃe�N�X�`���n���h��
	static DescriptorHandle* s_pSharedTexHandle;
	/// @brief �R���X�g���N�^
	WaterMesh() = default;
	/// @brief �f�X�g���N�^
	~WaterMesh() = default;
	
	// �N���[�����\�b�h
	Object* clone() const override;
	/// @brief ���b�V���p�l�p�`����
	Mesh CreateQuad(int _gridX,int _gridY,int _gridSize);

	/// @brief ����������
	/// @return �����������̐��ۂ�Ԃ��܂�
	bool Init() { return true; }
	bool Init(Camera* _camera,int _gridx,int _gridY,int _gridSize);
	/// @brief �X�V����
	void Update()	override;
	/// @brief �`�揈��
	void Draw()		override;
	/// @brief �I������
	void Uninit()	override;
};