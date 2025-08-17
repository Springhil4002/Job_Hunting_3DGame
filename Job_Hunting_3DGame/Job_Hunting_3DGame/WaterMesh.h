#pragma once
#include "Object.h"
#include "DrawBase.h"
#include "App.h"
#include <d3dx12.h>
#include <random>
#include "System/SharedStruct.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "RootSignature_WaterMesh.h"
#include "PipelineState_WaterMesh.h"
#include "DescriptorHeap.h"
#include "Texture2D.h"
#include "TextureManager.h"
#include "Camera.h"

class WaterMesh : public Object
{
private:
	// 頂点バッファ
	VertexBuffer* m_pVertexBuffer;
	// インデックスバッファ
	IndexBuffer* m_pIndexBuffer;
	// コンスタントバッファ
	ConstantBuffer* m_pConstantBuffer[DrawBase::FRAME_BUFFER_COUNT];
	// 波用定数バッファとパラメータ
	ConstantBuffer* m_pWaveBuffer;
	GerstnerParams m_waveParams{};
	// ライト用コンスタントバッファ
	ConstantBuffer* m_pLightBuffer;
	// ディスクリプタヒープ
	DescriptorHeap* m_pDescriptorHeap;
	// ルートシグネチャ
	RootSignature_WaterMesh* m_pRootSignature;
	// パイプラインステート
	PipelineState_WaterMesh* m_pPipelineState;
	// ディスクリプタハンドル
	DescriptorHandle* m_pSkyCubeTexHandle;
	// カメラ
	Camera* m_Camera;
	// 時間
	float g_Time = 0.0f;
	// 波の切り替え用変数
	float m_WaveTime = 0.0f;
	// 頂点数
	UINT m_IndexCount;
	// Xマス
	int m_GridX = 256;
	// Zマス
	int m_GridZ = 256;
	// グリッドサイズ
	int m_GridSize = 512;
public:
	// 共通テクスチャハンドル
	static DescriptorHandle* s_pSharedTexHandle;
	// ワールド行列更新操作用
	DirectX::XMMATRIX m_worldMatrix;
	/// @brief コンストラクタ
	WaterMesh() = default;
	/// @brief デストラクタ
	~WaterMesh() = default;
	
	// クローンメソッド
	Object* clone() const override;
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
	void Update_GridSize(int _newGridSize);
	/// @brief ワールド行列の更新
	void Update_Transform();
	/// @brief ビュー・プロジェクションの更新
	void Update_CameraMatrix();
	/// @brief 波形更新関数
	void Update_WaterWave(float _waveTime);
	/// @brief ライト更新
	void Update_Light();
	/// @brief ランダムな振幅を取得する関数
	/// @param _min 最低値
	/// @param _max 最大値
	/// @return ランダムな振幅値
	float GetRandomAmplitude(float _min, float _max);
	/// @brief 水面の波の高さを取得する関数
	/// @param _x Xマス
	/// @param _z Zマス
	/// @param _time 経過時間
	/// @return 波の高さ
	float GetWaveHeight(float _x, float _z, float _time);

	// 各種ゲッター・セッター
	int GetGridX() { return m_GridX; }
	int GetGridZ() { return m_GridZ; }
	int GetGridSize() { return m_GridSize; }

	void SetGridX(int _gridX) { m_GridX = _gridX; }
	void SetGridZ(int _gridZ) { m_GridZ = _gridZ; }
	void SetGridSize(int _gridSize) { m_GridSize = _gridSize; }
};