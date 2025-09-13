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
public:
	// ワールド行列更新操作用
	DirectX::XMMATRIX m_worldMatrix = DirectX::XMMatrixIdentity();
	/// @brief コンストラクタ
	UI() = default;
	/// @brief デストラクタ
	~UI() = default;

	// クローンメソッド
	std::unique_ptr<Object> clone() const override;
	
	Mesh_UI CreateQuad(
		float _x, float _y, float _w, float _h, XMFLOAT4 _color);

	/// @brief 初期化処理
	/// @return 初期化処理の成否を返します
	bool Init() { return true; }
	bool Init(Camera2D* _cameraUI, float _width, float _height);
	/// @brief 更新処理
	void Update()	override;
	/// @brief 描画処理
	void Draw()		override;
	/// @brief 終了処理
	void Uninit()	override;

	void UpdateTransform();
	void UpdateCameraMatrix();
};