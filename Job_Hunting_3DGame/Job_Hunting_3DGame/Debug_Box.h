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
#include "Camera.h"

class Debug_Box : public Object
{
private:
	// 頂点バッファ
	std::unique_ptr<VertexBuffer> m_pVertexBuffer;
	// インデックスバッファ
	std::unique_ptr<IndexBuffer> m_pIndexBuffer;
	// 定数バッファ
	std::unique_ptr<ConstantBuffer> m_pConstantBuffer[DrawBase::FRAME_BUFFER_COUNT];
	// ディスクリプタヒープ
	std::unique_ptr<DescriptorHeap> m_pDescriptorHeap;
	// ルートシグネチャ
	std::shared_ptr<RootSignature> m_pRootSignature;
	// パイプラインステート
	PipelineState_DebugMesh* m_pPipelineState = nullptr;
	// カメラ
	Camera* m_camera = nullptr;
	// 透明度
	float m_alpha = 0.0f;

	/// @brief ワールド行列の更新
	void Update_Transform();
	/// @brief ビュー・プロジェクションの更新
	void Update_CameraMatrix();
	/// @brief キューブ作成関数
	void Create_Cube();
public:
	/// @brief 頂点データ配列
	std::vector<Vertex> vertices;
	/// @brief インデックス配列
	std::vector<uint32_t> indices;

	// ワールド行列更新操作用
	DirectX::XMMATRIX m_worldMatrix = DirectX::XMMatrixIdentity();

	/// @brief コンストラクタ
	Debug_Box() = default;
	/// @brief デストラクタ
	~Debug_Box() = default;

	/// @brief クローン関数
	std::unique_ptr<Object> clone() const override;
	/// @brief 初期化関数
	/// @param _camera カメラ 
	/// @return 初期化の成否
	bool Init(Camera* _camera);
	/// @brief 更新関数
	void Update() override;
	/// @brief 描画関数
	void Draw() override;
	/// @brief 終了関数
	void UnInit() override;

	/// @brief 透明度を設定する関数
	/// @param _alpha 透明度の値
	void SetAlpha(float _alpha) { m_alpha = _alpha; }
	/// @brief 透明度を取得する関数
	/// @return 透明度
	float GetAlpha() const { return m_alpha; }

	DirectX::XMMATRIX GetWorldMatrix() const { return m_worldMatrix; }
};