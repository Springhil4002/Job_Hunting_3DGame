#pragma once
#include "Object.h"
#include "DrawBase.h"
#include "App.h"
#include <d3dx12.h>
#include "System/SharedStruct.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "IndexBuffer.h"
#include "AssimpLoader.h"
#include "DescriptorHeap.h"
#include "Texture2D.h"
#include "Camera.h"

#include <filesystem>

class Model3D : public Object
{
private:
	// ���_�o�b�t�@
	VertexBuffer* m_pVertexBuffer;
	// �C���f�b�N�X�o�b�t�@
	IndexBuffer* m_pIndexBuffer;
	// �R���X�^���g�o�b�t�@
	ConstantBuffer* m_pConstantBuffer[DrawBase::FRAME_BUFFER_COUNT];
	// �f�B�X�N���v�^�q�[�v
	DescriptorHeap* m_pDescriptorHeap;
	// ���[�g�V�O�l�`��
	RootSignature* m_pRootSignature;
	// �p�C�v���C���X�e�[�g
	PipelineState* m_pPipelineState;

	Camera* m_camera;

	// 3D���f���t�@�C���p�X
	const wchar_t* m_pModelFile = L"Assets/Alicia/FBX/Alicia_solid_Unity.FBX";
	// ���b�V���z��
	std::vector<Mesh> m_meshes;
	// ���b�V���̐����̒��_�o�b�t�@
	std::vector<VertexBuffer*> m_pVertexBuffers;	
	// ���b�V���̐����̃C���f�b�N�X�o�b�t�@
	std::vector<IndexBuffer*> m_pIndexBuffers;		
	// �e�N�X�`���p�̃n���h��
	std::vector<DescriptorHandle*> m_pMaterialHandles;
public:
	Model3D();
	~Model3D() = default;

	// �N���[�����\�b�h
	Object* clone() const override;

	/// @brief ����������
	/// @return �����������̐��ۂ�Ԃ��܂�
	bool Init() { return true; }
	bool Init(Camera* _camera);
	/// @brief �X�V����
	void Update()	override;
	/// @brief �`�揈��
	void Draw()		override;
	/// @brief �I������
	void Uninit()	override;

	/// @brief �g���q��u��������֐�
	/// @param _origin �t�@�C���p�X
	/// @param _ext �g���q
	/// @return �g���q��u���������t�@�C���p�X
	std::wstring ReplaceExtension(const std::wstring& _origin, const char* _ext);
};
