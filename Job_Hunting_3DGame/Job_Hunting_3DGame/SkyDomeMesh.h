#pragma once
#include <vector>
#include <DirectXMath.h>
#include "System/SharedStruct.h"
#include "DrawBase.h"
#include "Object.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "RootSignature_SkyDomeMesh.h"
#include "PipelineState_SkyDomeMesh.h"
#include "DescriptorHeap.h"
#include "Texture2D.h"
#include "TextureManager.h"
#include "Camera.h"

class SkyDomeMesh : public Object
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
	std::unique_ptr<RootSignature_SkyDomeMesh> m_pRootSignature;
	// パイプラインステート
	std::unique_ptr<PipelineState_SkyDomeMesh> m_pPipelineState;
	// ディスクリプタハンドル
	std::shared_ptr<DescriptorHandle> m_pTexHandle;
	// カメラ
	Camera* m_camera;
public:
	// ワールド行列更新操作用
	DirectX::XMMATRIX m_worldMatrix;
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
	void Uninit() override;

	/// @brief メッシュ生成関数
	/// @param _slices 分割数
	/// @param _stacks 層数
	/// @param _radius 半径
	void CreateMesh(int _slices, int _stacks, float _radius);

	/// @brief ワールド行列の更新
	void Update_Transform();
	/// @brief ビュー・プロジェクションの更新
	void Update_CameraMatrix();
};