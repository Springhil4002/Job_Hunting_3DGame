#pragma once
#include "Object.h"
#include "DrawBase.h"
#include "App.h"
#include <d3dx12.h>
#include "System/SharedStruct.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "RootSignature_DebugSphere.h"
#include "PipelineState_DebugSphere.h"
#include "IndexBuffer.h"
#include "DescriptorHeap.h"
#include "Texture2D.h"
#include "TextureManager.h"
#include "Camera.h"

class Debug_Sphere : public Object
{
private:
	// 頂点バッファ
	VertexBuffer* m_pVertexBuffer;
	// インデックスバッファ
	IndexBuffer* m_pIndexBuffer;
	// コンスタントバッファ
	ConstantBuffer* m_pConstantBuffer[DrawBase::FRAME_BUFFER_COUNT];
	// ディスクリプタヒープ
	DescriptorHeap* m_pDescriptorHeap;
	// ルートシグネチャ
	RootSignature_DebugSphere* m_pRootSignature;
	// パイプラインステート
	PipelineState_DebugSphere* m_pPipelineState;
	// ディスクリプタハンドル
	DescriptorHandle* m_pTexHandle;
	// カメラ
	Camera* m_camera;
	// 透明度
	float m_alpha = 0.5f;
	// 球体の半径
	float m_radius = 5.0f;
public:
	// ワールド行列更新操作用
	DirectX::XMMATRIX m_worldMatrix;
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	// コンストラクタ
	Debug_Sphere() = default;
	// デストラクタ
	~Debug_Sphere() = default;

	// クローンメソッド
	Object* clone() const override;

	/// @brief 初期化処理
	/// @return 初期化処理の成否を返します
	bool Init() { return true; }
	bool Init(Camera* _camera);
	/// @brief 更新処理
	void Update() override;
	/// @brief 描画処理
	void Draw() override;
	/// @brief 終了処理
	void Uninit() override;
	/// @brief ワールド行列の更新
	void Update_Transform();
	/// @brief ビュー・プロジェクションの更新
	void Update_CameraMatrix();
	/// @brief 球体メッシュ作成関数
	/// @param _stacks スタック数
	/// @param _slices 分割数
	/// @param _radius 半径
	void Create_Sphere(float _stacks, float _slices, float _radius);
	
	/// @brief 各種ゲッター・セッター
	float GetAlpha() const { return m_alpha; }
	float GetRadius() const { return m_radius; }

	void SetAlpha(float _alpha) { m_alpha = _alpha; }
	void SetRadius(float _radius) { m_radius = _radius; }
};

