#pragma once
#include "ParticleSystem.h"
#include "System/SharedStruct.h"
#include "DrawBase.h"
#include "App.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "RootSignature_Splash.h"
#include "PipelineState_Splash.h"
#include "DescriptorHeap.h"
#include "Texture2D.h"
#include "TextureManager.h"
#include <d3dx12.h>

struct InstanceData
{
	DirectX::XMFLOAT3 worldPos;	// パーティクル位置
	float scale;				// スケール
	DirectX::XMFLOAT4 color;	// 色
};

class ParticleSystem_Splash : public ParticleSystem
{
private:
	Camera* m_Camera;					// カメラ

	std::vector<InstanceData> m_InstanceData;	// インスタンスデータ
	ComPtr<ID3D12Resource> m_InstanceBuffer;	// インスタンスバッファ
	D3D12_VERTEX_BUFFER_VIEW m_InstanceBufferView;	// インスタンスバッファビュー
	int m_ActiveInstanceCount;					// ワンフレームで描画するインスタンス数

	// 頂点バッファ
	VertexBuffer* m_pVertexBuffer;
	// インデックスバッファ
	IndexBuffer* m_pIndexBuffer;
	// コンスタントバッファ
	ConstantBuffer* m_pConstantBuffer[DrawBase::FRAME_BUFFER_COUNT];
	// ディスクリプタヒープ
	DescriptorHeap* m_pDescriptorHeap;
	// ルートシグネチャ
	RootSignature_Splash* m_pRootSignature;
	// パイプラインステート
	PipelineState_Splash* m_pPipelineState;
	// ディスクリプタハンドル
	DescriptorHandle* m_pTexHandle;
	/// @brief 四角形メッシュ生成関数
	/// @return 四角形メッシュ。
	Mesh CreateQuad();
	/// @brief インスタンスバッファ生成関数
	/// @return 生成成否
	bool CreateInstanceBuffer();
	/// @brief 描画関連の初期化処理
	/// @param _camera カメラ
	/// @return 初期化の成否
	bool Init_Prop(Camera* _camera);
	/// @brief パラメータの初期化処理
	bool Init_Param();
public:
	ParticleSystem_Splash(Camera* _camera);
	bool Init(Camera* _camera) override;
	void Update(float _deltaTime) override;
	void Draw() override;
};