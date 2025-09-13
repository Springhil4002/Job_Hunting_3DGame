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

class WaterMesh : public Object
{
private:
	// 頂点バッファ
	std::unique_ptr<VertexBuffer> m_pVertexBuffer;
	// インデックスバッファ
	std::unique_ptr<IndexBuffer> m_pIndexBuffer;
	// コンスタントバッファ
	std::unique_ptr<ConstantBuffer> m_pConstantBuffer[DrawBase::FRAME_BUFFER_COUNT];
	// 波用定数バッファとパラメータ
	std::unique_ptr<ConstantBuffer> m_pWaveBuffer;
	GerstnerParams m_waveParams{};
	// ライト用コンスタントバッファ
	std::unique_ptr<ConstantBuffer> m_pLightBuffer;
	// ディスクリプタヒープ
	std::unique_ptr<DescriptorHeap> m_pDescriptorHeap;
	// ルートシグネチャ
	std::shared_ptr<RootSignature> m_pRootSignature;
	// パイプラインステート
	PipelineState_General* m_pPipelineState = nullptr;
	// ディスクリプタハンドル
	// (水面メッシュ側では所有しないためdeleteしないこと)
	std::shared_ptr<DescriptorHandle> m_pSkyCubeTexHandle;
	// カメラ
	Camera* m_Camera = nullptr;
	// 時間
	float g_Time = 0.0f;
	// 波の切り替え用変数
	float m_WaveTime = 0.0f;
	// 頂点数
	UINT m_IndexCount = 0;
	// Xマス
	int m_GridX = 256;
	// Zマス
	int m_GridZ = 256;
	// グリッドサイズ
	float m_GridSize = 512.0f;
public:
	// ワールド行列更新操作用
	DirectX::XMMATRIX m_worldMatrix = DirectX::XMMatrixIdentity();
	/// @brief コンストラクタ
	WaterMesh() = default;
	/// @brief デストラクタ
	~WaterMesh() = default;
	
	// クローンメソッド
	std::unique_ptr<Object> clone() const override;
	/// @brief 水面メッシュ生成関数
	Mesh CreateGridMesh();

	/// @brief 初期化処理
	/// @return 初期化処理の成否を返します
	bool Init() { return true; }
	bool Init(Camera* _camera);
	/// @brief 更新処理
	void Update()	override;
	/// @brief 描画処理
	void Draw()		override;
	/// @brief 終了処理
	void Uninit()	override;
	/// @brief グリッドサイズ更新
	/// @param _newGridSize グリッドサイズ
	void Update_GridSize(float _newGridSize);
	/// @brief ワールド行列の更新
	void Update_Transform();
	/// @brief ビュー・プロジェクションの更新
	void Update_CameraMatrix();
	/// @brief ライト更新
	void Update_Light();
	/// @brief 水面の波の高さを取得する関数
	/// @param _x Xマス
	/// @param _z Zマス
	/// @param _time 経過時間
	/// @return 波の高さ
	float GetWaveHeight(float _x, float _z, float _time);

	// 各種ゲッター・セッター
	int GetGridX() const { return m_GridX; }
	int GetGridZ() const { return m_GridZ; }
	float GetGridSize() const { return m_GridSize; }

	void SetGridX(int _gridX) { m_GridX = _gridX; }
	void SetGridZ(int _gridZ) { m_GridZ = _gridZ; }
	void SetGridSize(float _gridSize) { m_GridSize = _gridSize; }
};