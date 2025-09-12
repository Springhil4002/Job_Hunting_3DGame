#include "Goal.h"
#include "System/ImGui/imgui.h"
#include "Debug_New.h"

using namespace DirectX;

std::unique_ptr<Object> Goal::clone() const
{
	auto newObj = std::make_unique<Goal>();
	newObj->m_Sphere = m_Sphere;
	return newObj;
}

bool Goal::Init(Camera* _camera)
{
	Init_PropGoal(_camera);
	Init_PropSphere(_camera);
	return true;
}

void Goal::Update()
{
	Update_Transform();
	Update_CameraMatrix();
	if (m_Sphere)
	{
		XMVECTOR posGoal = GetPos();

		XMVECTOR offset=XMVectorSet(0.0f,3.0f,0.0f,0.0f);
		m_Sphere->SetPos(posGoal + offset);
		m_Sphere->Update();
	}
}

void Goal::Draw()
{
	// 現在のフレーム番号を取得
	auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
	// コマンドリスト
	auto commandList = g_DrawBase->CommandList();
	// ディスクリプタヒープ
	auto materialHeap = m_pDescriptorHeap->GetHeap();

	//　メッシュの数だけインデックス分の描画を行う
	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		// メッシュに対応する頂点バッファ
		auto vbView = m_pVertexBuffers[i]->View();
		// メッシュに対応する頂点の順番バッファ
		auto ibView = m_pIndexBuffers[i]->View();

		commandList->SetGraphicsRootSignature(m_pRootSignature->Get());
		commandList->SetPipelineState(m_pPipelineState->Get());
		commandList->SetGraphicsRootConstantBufferView(
			0, m_pConstantBuffer[currentIndex]->GetAddress());

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vbView);
		commandList->IASetIndexBuffer(&ibView);

		// 使用するディスクリプタヒープをセット
		commandList->SetDescriptorHeaps(1, &materialHeap);
		
		// テクスチャハンドルのnullチェック
		if(m_pTexHandle)
			// テクスチャをセット
			commandList->SetGraphicsRootDescriptorTable(1, m_pTexHandle->handleGPU);
		// インデックスの数分描画
		commandList->DrawIndexedInstanced(static_cast<UINT>(m_meshes[i].Indices.size()),
			1, 0, 0, 0);
	}

	if(m_Sphere)
	{
		m_Sphere->Draw();
	}

	//Draw_ImGui();
}

void Goal::Uninit()
{
	m_camera = nullptr;
	for (auto& cb : m_pConstantBuffer)
		cb.reset();
	m_pDescriptorHeap.reset();
	m_pRootSignature.reset();
	m_pTexHandle.reset();
	m_Sphere.reset();	
}

bool Goal::Init_PropGoal(Camera* _camera)
{
	m_camera = _camera;

	// ModelManager からモデルデータを取得
	auto modelData = ModelManager::GetInstance().GetModel(L"Assets/Goal/GoalGate.fbx");
	if (!modelData)
	{
		printf("Goal:モデル読み込み失敗\n");
		return false;
	}
	
	m_meshes = modelData->meshes;
	m_pVertexBuffers = modelData->vertexBuffers;
	m_pIndexBuffers = modelData->indexBuffers;

	for (size_t i = 0; i < DrawBase::FRAME_BUFFER_COUNT; ++i)
	{
		m_pConstantBuffer[i] = std::make_unique<ConstantBuffer>(sizeof(Matrix));
		if (!m_pConstantBuffer[i]->IsValid())
		{
			printf("Goal:コンスタントバッファ生成失敗\n");
			return false;
		}

		// カメラの初期化
		auto ptr = m_pConstantBuffer[i]->GetPtr<Matrix>();
		ptr->world = DirectX::XMMatrixIdentity();
		ptr->view = m_camera->GetViewMatrix();
		ptr->proj = m_camera->GetProjMatrix();
	}

	// ディスクリプタヒープの生成
	m_pDescriptorHeap = std::make_unique<DescriptorHeap>();

	auto tex = TextureManager::Instance().GetTexture(L"Assets/Texture/Color_Red.png");
	if (!tex)
	{
		printf("Goal:テクスチャ読み込み失敗\n");
		return false;
	}
	m_pTexHandle = m_pDescriptorHeap->Register(tex.get());

	auto& rootManager = RootSignatureManager::GetInstance();
	m_pRootSignature = rootManager.GetRoot(Root_Type::ROOT_TYPE_GOAL);
	if (!m_pRootSignature->IsValid())
	{
		printf("Goal:ルートシグネチャの生成に失敗\n");
		return false;
	}

	// マネージャー経由でパイプラインステートを取得
	auto& psoManager = PipelineState_Manager::GetInstance();
	m_pPipelineState = psoManager.GetPSO_General(PSO_Type::PSO_TYPE_GOAL);
	if (!m_pPipelineState->IsValid())
	{
		// 頂点レイアウトの設定
		m_pPipelineState->SetInputLayout(Vertex::InputLayout);
		// ルートシグネチャの設定
		m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
		// VS/PSの設定
#ifdef _DEBUG	// DEBUG
		m_pPipelineState->SetVS(L"../x64/Debug/VS_Simple.cso");
		m_pPipelineState->SetPS(L"../x64/Debug/PS_Simple.cso");
#else			// Release
		m_pPipelineState->SetVS(L"../x64/Release/VS_Simple.cso");
		m_pPipelineState->SetPS(L"../x64/Release/PS_Simple.cso");
#endif 
		// パイプラインステート作成
		m_pPipelineState->Create();
	}

	if (!m_pPipelineState->IsValid())
	{
		printf("Goal:パイプラインステートの生成に失敗\n");
		return false;
	}


	printf("Goal:初期化処理に成功\n\n");
	return true;
}

bool Goal::Init_PropSphere(Camera* _camera)
{
	m_Sphere = std::make_shared<Debug_Sphere>();
	if(!m_Sphere->Init(_camera))
	{
		return false;
	}
	m_Sphere->SetPos(XMVectorSet(0.0f, 25.0f, 50.0f, 0.0f));
	m_Sphere->SetRota(XMVectorZero());
	m_Sphere->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	m_tags.AddTag("Debug_Sphere");
	return true;
}

void Goal::Update_Transform()
{
	// カメラの更新処理
	auto pos = GetPos();
	auto rota = GetRota();
	auto scale = GetScale();

	m_worldMatrix =
		DirectX::XMMatrixScalingFromVector(scale) *
		DirectX::XMMatrixRotationRollPitchYawFromVector(rota) *
		DirectX::XMMatrixTranslationFromVector(pos);
}

void Goal::Update_CameraMatrix()
{
	auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
	auto ptr = m_pConstantBuffer[currentIndex]->GetPtr<Matrix>();
	ptr->world = m_worldMatrix;
	ptr->view = m_camera->GetViewMatrix();
	ptr->proj = m_camera->GetProjMatrix();

	// カメラ位置を毎フレーム更新
	XMVECTOR camPosVec = m_camera->GetPos();
	XMFLOAT3 camPos;
	XMStoreFloat3(&camPos, camPosVec);
	ptr->cameraPos = camPos;
}

void Goal::Draw_ImGui()
{
	ImGui_Sphere();
}

void Goal::ImGui_Sphere()
{
	if (m_Sphere)
	{
		ImGui::Begin("Goal_Sphere");
		if (ImGui::CollapsingHeader("Goal_Sphere", ImGuiTreeNodeFlags_DefaultOpen))
		{
			float alpha = m_Sphere->GetAlpha();
			if (ImGui::SliderFloat("Alpha", &alpha, 0.0f, 1.0f))
			{
				m_Sphere->SetAlpha(alpha);	
			}
		}
		ImGui::End();
	}
}