#pragma once
#include "ParticleSystem.h"
#include "System/SharedStruct.h"
#include "DrawBase.h"
#include "App.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "RootSignature_Manager.h"
#include "PipelineState_Manager.h"
#include "DescriptorHeap.h"
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
	std::unique_ptr<VertexBuffer> m_pVertexBuffer;
	// インデックスバッファ
	std::unique_ptr<IndexBuffer> m_pIndexBuffer;
	// コンスタントバッファ
	std::unique_ptr<ConstantBuffer> m_pConstantBuffer[DrawBase::FRAME_BUFFER_COUNT];
	// ディスクリプタヒープ
	std::unique_ptr<DescriptorHeap> m_pDescriptorHeap;
	// ルートシグネチャ
	std::shared_ptr<RootSignature> m_pRootSignature;
	// パイプラインステート
	PipelineState_Splash* m_pPipelineState = nullptr;
	// ディスクリプタハンドル
	std::shared_ptr<DescriptorHandle> m_pTexHandle;
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
	~ParticleSystem_Splash();
	bool Init(Camera* _camera) override;
	void Update(float _deltaTime) override;
	void Draw() override;
	void UnInit() override;
};