#pragma once
#include "Object.h"
#include "DrawBase.h"
#include "App.h"
#include <d3dx12.h>
#include "System/SharedStruct.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "IndexBuffer.h"
#include "AssimpLoader.h"
#include "DescriptorHeap.h"
#include "Texture2D.h"
#include "Camera.h"

#include <filesystem>

class Model3D : public Object
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
	RootSignature* m_pRootSignature;
	// パイプラインステート
	PipelineState* m_pPipelineState;

	Camera* m_camera;

	// 3Dモデルファイルパス
	const wchar_t* m_pModelFile = L"Assets/Alicia/FBX/Alicia_solid_Unity.FBX";
	// メッシュ配列
	std::vector<Mesh> m_meshes;
	// メッシュの数分の頂点バッファ
	std::vector<VertexBuffer*> m_pVertexBuffers;	
	// メッシュの数分のインデックスバッファ
	std::vector<IndexBuffer*> m_pIndexBuffers;		
	// テクスチャ用のハンドル
	std::vector<DescriptorHandle*> m_pMaterialHandles;
public:
	Model3D();
	~Model3D() = default;

	// クローンメソッド
	Object* clone() const override;

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

	/// @brief 拡張子を置き換える関数
	/// @param _origin ファイルパス
	/// @param _ext 拡張子
	/// @return 拡張子を置き換えたファイルパス
	std::wstring ReplaceExtension(const std::wstring& _origin, const char* _ext);
};
