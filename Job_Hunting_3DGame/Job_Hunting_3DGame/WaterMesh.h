#pragma once
#include "Object.h"
#include "DrawBase.h"
#include "App.h"
#include <d3dx12.h>
#include "System/SharedStruct.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "RootSignature_WaterMesh.h"
#include "PipelineState.h"
#include "DescriptorHeap.h"
#include "Texture2D.h"
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
	// ディスクリプタヒープ
	DescriptorHeap* m_pDescriptorHeap;
	// ルートシグネチャ
	RootSignature_WaterMesh* m_pRootSignature;
	// パイプラインステート
	PipelineState* m_pPipelineState;
	// カメラ
	Camera* m_camera;
	// 波のための時間
	float g_time = 0.0f;
public:
	/// @brief コンストラクタ
	WaterMesh() = default;
	/// @brief デストラクタ
	~WaterMesh() = default;
	
	// クローンメソッド
	Object* clone() const override;
	/// @brief メッシュ用四角形生成
	Mesh CreateQuad();

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
};