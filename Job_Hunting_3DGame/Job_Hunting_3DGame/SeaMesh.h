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
#include "DirectionalLight.h"
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
	// ライトデータ用コンスタントバッファ
	std::unique_ptr<ConstantBuffer> m_pLightConstantBuffer[DrawBase::FRAME_BUFFER_COUNT];
	
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
	// ワールド行列
	DirectX::XMMATRIX m_worldMatrix = DirectX::XMMatrixIdentity();

	float m_Time = 0.0f;			// 時間
	float m_WaveSpeed = 0.0f;		// 波の速度
	float m_Damping = 0.0f;			// 減衰率
	float m_GridSize = 0.0f;		// メッシュの大きさ
	int m_GridX = 0;				// X軸の分割数
	int m_GridZ = 0;				// Z軸の分割数
	UINT m_IndexCount = 0;			// 頂点数

	// 高さ場シミュレーション
	std::vector<float> m_Height;		// 高さ場
	std::vector<float> m_Velocity;		// 速度場
	std::vector<Vertex> m_Vertices;		// 頂点データリスト

	// 波紋生成構造体
	struct DropRequest
	{
		DirectX::XMFLOAT2 uv;
		float strength;
		float radius;
	};
	std::vector<DropRequest> m_PendingDrops;

	size_t Index(int _x, int _z) const;

	// メッシュ生成関数
	Mesh CreateMesh();
	/// @brief 動的波紋シミュレーション関数
	void StepSimulation(float _deltaTime);
	
	/// @brief 処理待ちの波紋を適用する関数
	void Apply_PendingDrops();

	float Sample(int _sx, int _sz);

	/// @brief 高さ場の初期化
	bool Init_SimulationResources();
	/// @brief 初期化関数
	/// @return 初期化の成否
	bool Init_Settings(Camera* _camera);
	/// @brief 座標更新関数
	void Update_Transform();
	/// @brief カメラ更新関数
	void Update_CameraMatrix();
	/// @brief 頂点バッファ更新関数
	void Update_VertexBuffer();
public:
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
	void UnInit()	override;

	/// @brief 波紋生成関数
	/// @param _uv 座標
	/// @param _strength 強さ 
	/// @param _radius 範囲
	void Apply_Drop(
		const DirectX::XMFLOAT2& _uv,
		float _strength, float _radius);

	/// @brief 高さ場シミュレーション関数
	/// @param _x X軸
	/// @param _z Z軸
	/// @return 高さ場の高さ
	float GetHeightFieldHeight(float _x, float _z);

	int GetGridX() const { return m_GridX; }
	int GetGridZ() const { return m_GridZ; }
	float GetGridSize() const { return m_GridSize; }
};