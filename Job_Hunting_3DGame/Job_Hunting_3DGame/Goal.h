#pragma once
#include "Object.h"
#include "DrawBase.h"
#include "App.h"
#include <d3dx12.h>
#include "System/SharedStruct.h"
#include "Camera.h"
#include "ModelManager.h"
#include "ConstantBuffer.h"
#include "DescriptorHeap.h"
#include "TextureManager.h"
#include "RootSignature_Manager.h"
#include "PipelineState_Manager.h"
#include "Debug_Sphere.h"
#include "DirectionalLight.h"

class Goal : public Object
{
private:
	// 頂点バッファ
	std::unique_ptr<VertexBuffer> m_pVertexBuffer;
	// インデックスバッファ
	std::unique_ptr<IndexBuffer> m_pIndexBuffer;
	// コンスタントバッファ
	std::unique_ptr<ConstantBuffer> m_pConstantBuffer[DrawBase::FRAME_BUFFER_COUNT];
	// ライトデータ用コンスタントバッファ
	std::unique_ptr<ConstantBuffer> m_pLightConstantBuffer[DrawBase::FRAME_BUFFER_COUNT];
	// ディスクリプタヒープ
	std::unique_ptr<DescriptorHeap> m_pDescriptorHeap;
	// ルートシグネチャ
	std::shared_ptr<RootSignature> m_pRootSignature;
	// パイプラインステート
	PipelineState_General* m_pPipelineState = nullptr;
	// ディスクリプタハンドル
	std::shared_ptr<DescriptorHandle> m_pTexHandle;
	// カメラ
	Camera* m_camera = nullptr;
	// モデルデータ配列
	std::vector<Mesh> m_meshes;
	std::vector<std::shared_ptr<VertexBuffer>> m_pVertexBuffers;
	std::vector<std::shared_ptr<IndexBuffer>> m_pIndexBuffers;
	// 当たり判定用デバッグ球体
	std::shared_ptr<Debug_Sphere> m_Sphere;
public:
	// ワールド行列更新操作用
	DirectX::XMMATRIX m_worldMatrix = DirectX::XMMatrixIdentity();

	// コンストラクタ
	Goal() = default;
	// デストラクタ
	~Goal() { UnInit(); }

	// クローンメソッド
	std::unique_ptr<Object> clone() const override;

	/// @brief 初期化処理
	/// @return 初期化処理の成否を返します
	bool Init() { return true; }
	bool Init(Camera* _camera);
	/// @brief 更新処理
	void Update() override;
	/// @brief 描画処理
	void Draw() override;
	/// @brief 終了処理
	void UnInit() override;
	/// @brief ゴールオブジェクトの初期化処理
	/// @param _camera カメラ
	/// @return 初期化成功の成否
	bool Init_PropGoal(Camera* _camera);
	/// @brief 球体オブジェクトの初期化処理
	/// @param _camera カメラ
	/// @return 初期化成功の成否
	bool Init_PropSphere(Camera* _camera);
	/// @brief ワールド行列の更新
	void Update_Transform();
	/// @brief ビュー・プロジェクションの更新
	void Update_CameraMatrix();

	// デバッグ用:ImGui描画関数
	void Draw_ImGui();
	void ImGui_Sphere();

	/// @brief 当たり判定用の球体の取得
	/// @return 当たり判定用の球体のポインタ
	Debug_Sphere* GetSphere() const { return m_Sphere.get(); }
};