#pragma once
#include <vector>
#include <DirectXMath.h>
#include "System/SharedStruct.h"
#include "DrawBase.h"
#include "Object.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "RootSignature_Manager.h"
#include "PipelineState_Manager.h"
#include "DescriptorHeap.h"
#include "TextureManager.h"
#include "Camera.h"

class SkyBox : public Object
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
	PipelineState_SkyBox* m_pPipelineState = nullptr;
	// ディスクリプタハンドル
	std::shared_ptr<DescriptorHandle> m_pCubeTexHandle;
	// カメラ
	Camera* m_camera = nullptr;
public:
	// ワールド行列更新操作用
	DirectX::XMMATRIX m_worldMatrix = DirectX::XMMatrixIdentity();
	std::vector<SkyVertex> vertices;
	std::vector<uint32_t> indices;

	/// @brief クローン生成関数
	/// @return クローンを返します
	std::unique_ptr<Object> clone() const override;

	/// @brief 初期化処理
	/// @return 成否を返します
	bool Init() { return true; }
	bool Init(Camera* _camera);

	/// @brief 更新処理
	void Update() override;
	/// @brief 描画処理
	void Draw() override;
	/// @brief 終了処理
	void UnInit() override;

	/// @brief キューブメッシュ作成関数
	void CreateCubeMesh();

	/// @brief ワールド行列の更新
	void Update_Transform();
	/// @brief ビュー・プロジェクションの更新
	void Update_CameraMatrix();
};