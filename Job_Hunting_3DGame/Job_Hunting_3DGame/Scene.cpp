#include "Scene.h"
#include "DrawBase.h"
#include "App.h"
#include <d3dx12.h>
#include "SharedStruct.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "IndexBuffer.h"
#include "AssimpLoader.h"

Scene* g_Scene;
VertexBuffer* vertexBuffer;
ConstantBuffer* constantBuffer[DrawBase::FRAME_BUFFER_COUNT];
RootSignature* rootSignature;
PipelineState* pipelineState;
IndexBuffer* indexBuffer;
const wchar_t* modelFile = L"Assets/Alicia/FBX/Alicia_solid_Unity.FBX";
std::vector<Mesh> meshes;
std::vector<VertexBuffer*> vertexBuffers;	// メッシュの数分の頂点バッファ
std::vector<IndexBuffer*> indexBuffers;		// メッシュの数分のインデックスバッファ

bool Scene::Init()
{
	// インポートに必要なパラメータ構造体
	ImportSettings importSetting = 
	{
		modelFile,
		meshes,
		false,
		true,	// アリシアはテクスチャV座標が反転してるためtrue
	};

	AssimpLoader loader;
	if (!loader.Load(importSetting))
	{
		return false;
	}
	
	// メッシュの数だけ頂点バッファを用意する
	vertexBuffers.reserve(meshes.size());
	for (size_t i = 0; i < meshes.size(); i++)
	{
		auto size = sizeof(Vertex) * meshes[i].Vertices.size();
		auto stride = sizeof(Vertex);
		auto vertices = meshes[i].Vertices.data();
		auto pVB = new VertexBuffer(size, stride, vertices);
		if (!pVB->IsValid())
		{
			printf("頂点バッファの生成に失敗\n");
			return false;
		}
		vertexBuffers.push_back(pVB);
	}

	// メッシュの数だけインデックスバッファを用意する
	indexBuffers.reserve(meshes.size());
	for (size_t i = 0; i < meshes.size(); i++)
	{
		auto size = sizeof(uint32_t) * meshes[i].Indices.size();
		auto indices = meshes[i].Indices.data();
		auto pIB = new IndexBuffer(size, indices);
		
		if (!pIB->IsValid())
		{
			printf("インデックスバッファの生成に失敗\n");
			return false;
		}
		indexBuffers.push_back(pIB);
	}

	// 視点の位置
	auto eyePos = DirectX::XMVectorSet(0.0f, 120.0f, 75.0f, 0.0f); 
	// 注視点の座標
	auto targetPos = DirectX::XMVectorSet(0.0f, 120.0f, 0.0f, 0.0f);
	// 上方向を表すベクトル
	auto upward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); 
	// 視野角
	auto fov = DirectX::XMConvertToRadians(60.0); 
	// アスペクト比
	auto aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT); 
	// 定数バッファの生成と変換行列の登録
	for (size_t i = 0; i < DrawBase::FRAME_BUFFER_COUNT; i++)
	{
		// 定数バッファの生成
		constantBuffer[i] = new ConstantBuffer(sizeof(Matrix));
		if (!constantBuffer[i]->IsValid())
		{
			printf("変換行列用定数バッファの生成に失敗\n");
			return false;
		}

		// 変換行列の登録
		auto ptr = constantBuffer[i]->GetPtr<Matrix>();
		ptr->world = DirectX::XMMatrixIdentity();
		ptr->view = DirectX::XMMatrixLookAtRH(eyePos, targetPos, upward);
		ptr->proj = DirectX::XMMatrixPerspectiveFovRH(fov, aspect, 0.3f, 1000.0f);
	}

	rootSignature = new RootSignature();
	if (!rootSignature->IsValid())
	{
		printf("ルートシグネチャの生成に失敗\n");
		return false;
	}

	// パイプラインステートのインスタンス生成
	pipelineState = new PipelineState();
	// 頂点レイアウトの設定
	pipelineState->SetInputLayout(Vertex::InputLayout);
	// ルートシグネチャの設定
	pipelineState->SetRootSignature(rootSignature->Get());

#ifdef _DEBUG	// DEBUG
	// VSを設定
	pipelineState->SetVS(L"../x64/Debug/VS_Simple.cso");
	// PSを設定
	pipelineState->SetPS(L"../x64/Debug/PS_Simple.cso");
#else			// Release
	// VSを設定
	pipelineState->SetVS(L"../x64/Release/VS_Simple.cso");
	// PSを設定
	pipelineState->SetPS(L"../x64/Release/PS_Simple.cso");
#endif 

	// パイプラインステート作成
	pipelineState->Create();

	if (!pipelineState->IsValid())
	{
		printf("パイプラインステートの生成に失敗\n");
		return false;
	}

	printf("シーンの初期化処理に成功\n");
	return true;
}

void Scene::Update()
{
	
}

void Scene::Draw()
{
	// 現在のフレーム番号を取得
	auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
	// コマンドリスト
	auto commandList = g_DrawBase->CommandList();

	//　メッシュの数だけインデックス分の描画を行う
	for (size_t i = 0; i < meshes.size(); i++)
	{
		// メッシュに対応する頂点バッファ
		auto vbView = vertexBuffers[i]->View();	
		// メッシュに対応する頂点の順番バッファ
		auto ibView = indexBuffers[i]->View();

		commandList->SetGraphicsRootSignature(rootSignature->Get());
		commandList->SetPipelineState(pipelineState->Get());
		commandList->SetGraphicsRootConstantBufferView(
			0, constantBuffer[currentIndex]->GetAddress());

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vbView);
		commandList->IASetIndexBuffer(&ibView);
		
		// インデックスの数分描画
		commandList->DrawIndexedInstanced(meshes[i].Indices.size(), 1, 0, 0, 0);
	}
}
