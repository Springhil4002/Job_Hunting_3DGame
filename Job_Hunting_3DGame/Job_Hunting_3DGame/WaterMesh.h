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

class WaterMesh : public Object
{
private:
	// 頂点バッファ
	std::unique_ptr<VertexBuffer> m_pVertexBuffer;
	// インデックスバッファ
	std::unique_ptr<IndexBuffer> m_pIndexBuffer;
	// コンスタントバッファ
	std::unique_ptr<ConstantBuffer> m_pConstantBuffer[DrawBase::FRAME_BUFFER_COUNT];
	// 波用コンスタントバッファ
	std::unique_ptr<ConstantBuffer> m_pWaveBuffer;
	GerstnerParams m_waveParams{};
	// ライトコンスタントバッファバッファ
	std::unique_ptr<ConstantBuffer> m_pLightBuffer;
	// WaterParamsコンスタントバッファ
	std::unique_ptr<ConstantBuffer> m_pWaterBuffer;
	WaterParams m_waterParams{};
	// ディスクリプタヒープ
	std::unique_ptr<DescriptorHeap> m_pDescriptorHeap;
	// ルートシグネチャ
	std::shared_ptr<RootSignature> m_pRootSignature;
	// パイプラインステート
	PipelineState_General* m_pPipelineState = nullptr;
	// スカイボックス用テクスチャハンドル
	std::shared_ptr<DescriptorHandle> m_pSkyCubeTexHandle;
	// カメラ
	Camera* m_Camera = nullptr;
	// 時間
	float g_Time = 0.0f;
	// 波形時間
	float m_WaveTime = 0.0f;
	// 頂点数
	UINT m_IndexCount = 0;
	// グリッド線：X軸
	int m_GridX = 256;
	// グリッド線：Z軸
	int m_GridZ = 256;
	// グリッドの大きさ
	float m_GridSize = 512.0f;

	std::vector<float> m_Height;	// 高さ場
	std::vector<float> m_Velocity;	// 速度場
	std::vector<Vertex> m_Vertices;	// 頂点データリスト

	// 波の伝播速度
	float m_WaveSpeed = 10.0f;
	float m_Damping = 0.95f;

	struct DropRequest
	{
		DirectX::XMFLOAT2 uv;
		float strength;
		float radius;
	};
	std::vector<DropRequest> m_PendingDrops;
	size_t Index(int _x, int _z) const
	{
		return static_cast<size_t>(_z) * (m_GridX + 1) + static_cast<size_t>(_x);
	}

public:
	// ワールド行列
	DirectX::XMMATRIX m_worldMatrix = DirectX::XMMatrixIdentity();
	/// @brief コンストラクタ
	WaterMesh() = default;
	/// @brief デストラクタ
	~WaterMesh() = default;

	// クローン関数
	std::unique_ptr<Object> clone() const override;
	/// @brief メッシュ生成関数
	Mesh CreateGridMesh();

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
	/// @brief 水面メッシュサイズ更新関数
	/// @param _newGridSize グリッドサイズの大きさ
	void Update_GridSize(float _newGridSize);
	/// @brief 座標更新関数
	void Update_Transform();
	/// @brief カメラ更新関数
	void Update_CameraMatrix();
	/// @brief ライト更新関数
	void Update_Light();

	// 動的波紋シミュレーション関数
	void StepSimulation(float _deltaTime);

	// 頂点バッファ更新関数
	void Update_VertexBuffer();

	// 波紋生成関数
	void ApplyDrop(const DirectX::XMFLOAT2& _uv, float _strength, float _radius);

	// 処理待ちのドロップを適用する
	void ApplyPendingDrops();

	// 高さ場の初期化
	bool Init_SimulationResources();

	float Sample(int _sx, int _sz);

	/// @brief 波の高さ取得関数
	/// @param _x X軸
	/// @param _z Z軸
	/// @param _time 時間
	/// @return 波の高さ
	float GetWaveHeight(float _x, float _z, float _time);

	/// @brief 高さ場シミュレーションの取得関数
	/// @param _x X軸
	/// @param _z Z軸
	/// @return 高さ場の高さ
	float GetHeightFieldHeight(float _x, float _z);

	// ゲッター
	int GetGridX() const { return m_GridX; }
	int GetGridZ() const { return m_GridZ; }
	float GetGridSize() const { return m_GridSize; }

	void SetGridX(int _gridX) { m_GridX = _gridX; }
	void SetGridZ(int _gridZ) { m_GridZ = _gridZ; }
	void SetGridSize(float _gridSize) { m_GridSize = _gridSize; }
}; 