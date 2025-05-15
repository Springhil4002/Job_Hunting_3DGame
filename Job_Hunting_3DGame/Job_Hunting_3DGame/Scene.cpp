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

Scene* g_Scene;
VertexBuffer* vertexBuffer;
ConstantBuffer* constantBuffer[DrawBase::FRAME_BUFFER_COUNT];
RootSignature* rootSignature;
PipelineState* pipelineState;
IndexBuffer* indexBuffer;

bool Scene::Init()
{
	// 頂点データ
	Vertex vertices[4] = {};
	vertices[0].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertices[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[3].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[3].color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);

	// 頂点バッファ全体のサイズを計算
	auto vertexSize = sizeof(Vertex) * std::size(vertices);
	// 1頂点のサイズを取得
	auto vertexStride = sizeof(Vertex);
	// 頂点バッファのインスタンス生成
	vertexBuffer = new VertexBuffer(vertexSize, vertexStride, vertices);
	
	if (!vertexBuffer->IsValid())
	{
		printf("頂点バッファの生成に失敗\n");
		return false;
	}

	// これに書かれてる順番で描画する
	uint32_t indices[] = { 0,1,2,0,2,3 };

	// インデックスバッファの生成
	auto size = sizeof(uint32_t) * std::size(indices);
	indexBuffer = new IndexBuffer(size, indices);
	if (!indexBuffer->IsValid())
	{
		printf("インデックスバッファの生成に失敗\n");
		return false;
	}

	// 視点の位置
	auto eyePos= DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f); 
	// 注視点の座標
	auto targetPos = DirectX::XMVectorZero(); 
	// 上方向を表すベクトル
	auto upward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); 
	// 視野角
	auto fov = DirectX::XMConvertToRadians(37.5); 
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
	//// 毎フレーム回転させる
	//rotateY += 0.02f;
	//// 現在のフレーム番号を取得
	//auto cuurentIndex = g_DrawBase->CurrentBackBufferIndex();
	//// 現在のフレーム番号に対応する定数バッファを取得
	//auto cuurentTransform = constantBuffer[cuurentIndex]->GetPtr<Matrix>();

	//cuurentTransform->world = DirectX::XMMatrixRotationY(rotateY);
}

void Scene::Draw()
{
	// 現在のフレーム番号を取得
	auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
	// コマンドリスト
	auto commandList = g_DrawBase->CommandList();
	// 頂点バッファビュー
	auto vbView = vertexBuffer->View();
	// インデックスバッファビュー
	auto ibView = indexBuffer->View();

	// ルートシグネチャをセット
	commandList->SetGraphicsRootSignature(rootSignature->Get());
	// パイプラインステートをセット
	commandList->SetPipelineState(pipelineState->Get());
	// 定数バッファをセット
	commandList->SetGraphicsRootConstantBufferView(0, constantBuffer[currentIndex]->GetAddress());
	
	// 三角形を描画する設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	
	// 頂点バッファをスロット0番を使って一個だけ設定する
	commandList->IASetVertexBuffers(0, 1, &vbView);
	// インデックスバッファをセットする
	commandList->IASetIndexBuffer(&ibView);
	
	// 三角形ポリゴン二枚で四角形を描画
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}
