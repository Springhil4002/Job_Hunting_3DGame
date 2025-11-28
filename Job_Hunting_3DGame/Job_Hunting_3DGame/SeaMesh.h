#pragma once
#include "Object.h"
#include "DrawBase.h"
#include "App.h"
#include <d3dx12.h>
#include "System/SharedStruct.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "RootSignature_Manager.h"
#include "PipelineState_Manager.h"
#include "DescriptorHeap.h"
#include "TextureManager.h"
#include "Camera.h"
#include <random>

class SeaMesh : public Object
{
private:
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
	PipelineState_General* m_pPipelineState = nullptr;
	// スカイボックス用テクスチャハンドル
	std::shared_ptr<DescriptorHandle> m_pSkyCubeTexHandle;
	// ノーマルマップ用テクスチャハンドル
	std::shared_ptr<DescriptorHandle> m_pNormalTexHandle;
	// ノーマルマップ用テクスチャハンドル2
	std::shared_ptr<DescriptorHandle> m_pNormalTexHandle2;
	// カメラ
	Camera* m_Camera = nullptr;
	// 時間
	float m_Time = 0.0f;
public:
	// ワールド行列
	DirectX::XMMATRIX m_worldMatrix = DirectX::XMMatrixIdentity();
	/// @brief コンストラクタ
	SeaMesh() = default;
	/// @brief デストラクタ
	~SeaMesh() = default;

	// クローン関数
	std::unique_ptr<Object> clone() const override;

	/// @brief 初期化関数
	/// @return 初期化の成否
	bool Init() { return true; }
	bool Init(Camera* _camera);
	/// @brief 更新関数
	void Update()	override;
	/// @brief 描画関数
	void Draw()		override;
	/// @brief 終了関数
	void Uninit()	override;

	Mesh CreateMesh();

	/// @brief 座標更新関数
	void Update_Transform();
	/// @brief カメラ更新関数
	void Update_CameraMatrix();
};