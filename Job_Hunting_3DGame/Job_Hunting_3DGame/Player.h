#pragma once
#include "Object.h"
#include "DrawBase.h"
#include "App.h"
#include <d3dx12.h>
#include "System/SharedStruct.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "RootSignature_Player.h"
#include "PipelineState_Player.h"
#include "IndexBuffer.h"
#include "AssimpLoader.h"
#include "DescriptorHeap.h"
#include "Texture2D.h"
#include "TextureManager.h"
#include "Camera.h"

class Player : public Object
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
	RootSignature_Player* m_pRootSignature;
	// パイプラインステート
	PipelineState_Player* m_pPipelineState;
	// ディスクリプタハンドル
	DescriptorHandle* m_pTexHandle;
	// カメラ
	Camera* m_camera;
	// 3Dモデルファイルパス
	const wchar_t* m_pModelFile = L"";
	// メッシュ配列
	std::vector<Mesh> m_meshes;
	// メッシュの数分の頂点バッファ
	std::vector<VertexBuffer*> m_pVertexBuffers;
	// メッシュの数分のインデックスバッファ
	std::vector<IndexBuffer*> m_pIndexBuffers;
	// テクスチャ用のハンドル
	std::vector<DescriptorHandle*> m_pMaterialHandles;
public:
	// ワールド行列更新操作用
	DirectX::XMMATRIX m_worldMatrix;

	// コンストラクタ
	Player() = default;
	// デストラクタ
	~Player() = default;

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
};

