#pragma once
#include <d3dx12.h>
#include "Object.h"
#include "DrawBase.h"
#include "App.h"
#include "System/SharedStruct.h"
#include "Camera2D.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "DescriptorHeap.h"
#include "RootSignature_Manager.h"
#include "PipelineState_Manager.h"
#include "TextureManager.h"

class UI : public Object
{
protected:
	// 頂点バッファ
	std::unique_ptr<VertexBuffer> m_pVertexBuffer;
	// インデックスバッファ
	std::unique_ptr<IndexBuffer> m_pIndexBuffer;
	// コンスタントバッファ
	std::unique_ptr<ConstantBuffer> m_pConstantBuffer[DrawBase::FRAME_BUFFER_COUNT];
	// ディスクリプタヒープ
	std::unique_ptr<DescriptorHeap> m_pDescriptorHeap;
	// ルートシグネチャ
	std::shared_ptr<RootSignature> m_pRootSignature;
	// パイプラインステート
	PipelineState_UI* m_pPipelineState = nullptr;
	// ディスクリプタハンドル
	std::shared_ptr<DescriptorHandle> m_pTexHandle;
	// カメラ
	Camera2D* m_CameraUI = nullptr;
	// UIの横幅
	float m_SizeWidth = 0.0f;
	// UIの縦幅
	float m_SizeHeight = 0.0f;
	// 透明度
	float m_Alpha = 1.0f;
	// UV座表
	DirectX::XMFLOAT4 m_UV = { 0.0f,0.0f,1.0f,1.0f };
public:
	// ワールド行列更新操作用
	DirectX::XMMATRIX m_worldMatrix = DirectX::XMMatrixIdentity();
	/// @brief コンストラクタ
	UI() = default;
	/// @brief デストラクタ
	virtual ~UI() = default;

	// クローンメソッド
	virtual std::unique_ptr<Object> clone() const override;
	
	Mesh_UI CreateQuad(
		float _x, float _y, float _w, float _h, XMFLOAT4 _color);

	/// @brief 初期化処理
	/// @return 初期化処理の成否を返します
	bool Init() { return true; }
	bool Init(Camera2D* _cameraUI, float _width, float _height, 
		const std::wstring& _filePath);
	/// @brief 更新処理
	virtual void Update()	override;
	/// @brief 描画処理
	virtual void Draw()		override;
	/// @brief 終了処理
	virtual void Uninit()	override;

	void SetTransform(
		DirectX::XMVECTOR _pos,
		DirectX::XMVECTOR _rota,
		DirectX::XMVECTOR _scale,
		float alpha = 1.0f,
		XMFLOAT4 _uvRect = { 0.0f,0.0f,1.0f,1.0f });
	void SetUV(const DirectX::XMFLOAT4& _uv);
	void UpdateTransform();
	void UpdateCameraMatrix();
	float GetAlpha() const { return m_Alpha; }
	void SetAlpha(float _alpha) { m_Alpha = _alpha; }
};