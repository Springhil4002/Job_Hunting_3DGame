#pragma once
#include "Object.h"
#include "DrawBase.h"
#include "App.h"
#include <d3dx12.h>
#include <random>
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
	// ���C�g�p�R���X�^���g�o�b�t�@
	ConstantBuffer* m_pLightBuffer;
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
	// ����
	float g_time = 0.0f;
	// �g�̐؂�ւ��p�ϐ�
	float m_waveTime = 0.0f;
public:
	// ���ʃe�N�X�`���n���h��
	static DescriptorHandle* s_pSharedTexHandle;
	// ���[���h�s��X�V����p
	DirectX::XMMATRIX m_worldMatrix;
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
	/// @brief ���[���h�s��̍X�V
	void Update_Transform();
	/// @brief �r���[�E�v���W�F�N�V�����̍X�V
	void Update_CameraMatrix();
	/// @brief �g�`�X�V�֐�
	void Update_WaterWave(float _waveTime);
	/// @brief �����_���ȐU�����擾����֐�
	/// @param _min �Œ�l
	/// @param _max �ő�l
	/// @return �����_���ȐU���l
	float GetRandomAmplitude(float _min = 0.1f, float _max = 0.5f);
};