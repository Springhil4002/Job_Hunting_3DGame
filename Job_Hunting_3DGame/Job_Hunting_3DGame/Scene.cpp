#include "Scene.h"
#include "DrawBase.h"
#include "App.h"
#include <d3dx12.h>
#include "SharedStruct.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"

Scene* g_Scene;
VertexBuffer* vertexBuffer;
ConstantBuffer* constantBuffer[DrawBase::FRAME_BUFFER_COUNT];

bool Scene::Init()
{
	// 頂点データ
	Vertex vertices[3] = {};
	vertices[0].position=XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[1].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[2].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	// 頂点バッファ全体のサイズを計算
	auto vertexSize = sizeof(Vertex) * std::size(vertices);
	// 1頂点のサイズを取得
	auto vertexStride = sizeof(Vertex);
	// 頂点バッファのインスタンス生成
	vertexBuffer = new VertexBuffer(vertexSize, vertexStride, vertices);
	
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

	if (!vertexBuffer->IsValid())
	{
		printf("頂点バッファの生成に失敗\n");
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

}
