#include "Goal.h"
#include "System/ImGui/imgui.h"
#include "Debug_New.h"
#include "Debug_Msg.h"

using namespace DirectX;

std::unique_ptr<Object> Goal::clone() const
{
	auto newObj = std::make_unique<Goal>();
	newObj->m_Sphere = m_Sphere;
	newObj->m_Poal_L = m_Poal_L;
	newObj->m_Poal_R = m_Poal_R;
	return newObj;
}

bool Goal::Init(Camera* _camera)
{
	Init_PropGoal(_camera);
	Init_PropSphere(_camera);
	Init_PropPoals(_camera);
	return true;
}

void Goal::Update()
{
	Update_Transform();
	Update_CameraMatrix();
	Update_Sphere();
	Update_Poals();
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
		commandList->SetGraphicsRootConstantBufferView(
			2, m_pLightConstantBuffer[currentIndex]->GetAddress());

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

	if(m_Sphere) m_Sphere->Draw();
	if(m_Poal_L) m_Poal_L->Draw();
	if(m_Poal_R) m_Poal_R->Draw();
}

void Goal::UnInit()
{
	m_camera = nullptr;
	for (auto& cb : m_pConstantBuffer)
		cb.reset();
	for (auto& cb : m_pLightConstantBuffer)
		cb.reset();
	m_pDescriptorHeap.reset();
	m_pRootSignature.reset();
	m_pTexHandle.reset();
	m_Sphere.reset();	
	m_Poal_L.reset();
	m_Poal_R.reset();
}

bool Goal::Init_PropGoal(Camera* _camera)
{
	m_camera = _camera;

	// ModelManager からモデルデータを取得
	auto modelData = ModelManager::GetInstance().GetModel(L"Assets/Goal/GoalGate.fbx");
	if (!modelData)
	{
		DEBUG_LOG_ERROR("Goal:モデル読み込み失敗");
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
			DEBUG_LOG_ERROR(L"Goal:コンスタントバッファ生成失敗");
			return false;
		}

		// カメラの初期化
		auto ptr = m_pConstantBuffer[i]->GetPtr<Matrix>();
		ptr->world = DirectX::XMMatrixIdentity();
		ptr->view = m_camera->GetViewMatrix();
		ptr->proj = m_camera->GetProjMatrix();
	}

	for (size_t i = 0; i < DrawBase::FRAME_BUFFER_COUNT; ++i)
	{
		m_pLightConstantBuffer[i] = std::make_unique<ConstantBuffer>(sizeof(DirectionalLightData));
		if (!m_pLightConstantBuffer[i]->IsValid())
		{
			DEBUG_LOG_ERROR(L"Goal:ライトコンスタントバッファ生成失敗");
			return false;
		}
	}

	// ディスクリプタヒープの生成
	m_pDescriptorHeap = std::make_unique<DescriptorHeap>();

	auto tex = TextureManager::Instance().GetTexture(L"Assets/Texture/Color_Red.png");
	if (!tex)
	{
		DEBUG_LOG_ERROR(L"Goal:テクスチャ読み込み失敗");
		return false;
	}
	m_pTexHandle = m_pDescriptorHeap->Register(tex.get());

	auto& rootManager = RootSignatureManager::GetInstance();
	m_pRootSignature = rootManager.GetRoot(Root_Type::ROOT_TYPE_GOAL);
	if (!m_pRootSignature->IsValid())
	{
		DEBUG_LOG_ERROR(L"Goal:ルートシグネチャの生成に失敗");
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
		m_pPipelineState->SetVS(L"../x64/Debug/VS_Goal.cso");
		m_pPipelineState->SetPS(L"../x64/Debug/PS_Goal.cso");
#else			// Release
		m_pPipelineState->SetVS(L"../x64/Release/VS_Goal.cso");
		m_pPipelineState->SetPS(L"../x64/Release/PS_Goal.cso");
#endif 
		// パイプラインステート作成
		m_pPipelineState->Create();
	}

	if (!m_pPipelineState->IsValid())
	{
		DEBUG_LOG_ERROR(L"Goal:パイプラインステートの生成に失敗");
		return false;
	}

	DEBUG_LOG(L"Goal:初期化処理に成功");
	return true;
}

bool Goal::Init_PropSphere(Camera* _camera)
{
	m_SpherePos		= { 0.0f,5.0f,0.0f };
	m_SphereRota	= { 0.0f,0.0f,0.0f };
	m_SphereScale	= { 1.0f,1.0f,1.0f };

	m_Sphere = std::make_shared<Debug_Sphere>();
	if(!m_Sphere->Init(_camera))
	{
		DEBUG_LOG_ERROR(L"Goal:Sphereの初期化処理に失敗");
		return false;
	}
	m_Sphere->SetAlpha(0.0f);

	DEBUG_LOG(L"Goal:Sphereの初期化処理に成功");
	return true;
}

bool Goal::Init_PropPoals(Camera* _camera)
{
	m_PoleOffset = { 5.5f,6.5f,0.0f };
	m_PoleRota	 = { 0.0f,0.0f,0.0f };
	m_PoleScale	 = { 1.0f,13.5f,1.0f };

	m_Poal_L = std::make_shared<Debug_Box>();
	m_Poal_R = std::make_shared<Debug_Box>();

	if (!m_Poal_L->Init(_camera) ||
		!m_Poal_R->Init(_camera))
	{
		DEBUG_LOG_ERROR(L"Goal:2つのPoalの初期化処理に失敗");		
		return false;
	}
	m_Poal_L->SetAlpha(0.0f);
	m_Poal_R->SetAlpha(0.0f);

	DEBUG_LOG(L"Goal:2つのPoalの初期化処理に成功");
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

	// ライトデータの更新処理
	const auto& lightData = DirectionalLight::Instance().GetLightData();
	auto ptrLight = m_pLightConstantBuffer[currentIndex]->GetPtr<DirectionalLightData>();
	*ptrLight = lightData;
}

void Goal::Update_Sphere()
{
	if (m_Sphere)
	{
		XMMATRIX goalMat = m_worldMatrix;

		// 通過判定用メッシュのローカル座標オフセット
		XMVECTOR SpherePos = XMVectorSet(m_SpherePos.x, m_SpherePos.y, m_SpherePos.z, 0.0f);

		// Goalの行列をかけて座表変換
		m_Sphere->SetPos(XMVector3Transform(SpherePos, goalMat));

		// 回転とスケールの設定
		XMVECTOR sphereScale = XMLoadFloat3(&m_SphereScale);
		m_Sphere->SetScale(sphereScale);
		XMVECTOR sphereRota = XMLoadFloat3(&m_SphereRota);
		m_Sphere->SetRota(sphereRota);

		m_Sphere->Update();
	}
}

void Goal::Update_Poals()
{
	if (m_Poal_L && m_Poal_R)
	{
		XMMATRIX goalMat = m_worldMatrix;

		// 各柱のローカル座標オフセット
		XMVECTOR posL = XMVectorSet(-m_PoleOffset.x, m_PoleOffset.y, m_PoleOffset.z, 0.0f);
		XMVECTOR posR = XMVectorSet( m_PoleOffset.x, m_PoleOffset.y, m_PoleOffset.z, 0.0f);

		// Goalの行列をかけて座表変換
		m_Poal_L->SetPos(XMVector3Transform(posL, goalMat));
		m_Poal_R->SetPos(XMVector3Transform(posR, goalMat));

		// 回転とスケールの設定
		XMVECTOR poleScale = XMLoadFloat3(&m_PoleScale);
		m_Poal_L->SetScale(poleScale);
		m_Poal_R->SetScale(poleScale);

		XMVECTOR poleRota = XMLoadFloat3(&m_PoleRota);
		m_Poal_L->SetRota(poleRota);
		m_Poal_R->SetRota(poleRota);

		m_Poal_L->Update();
		m_Poal_R->Update();
	}
}

void Goal::Draw_ImGui()
{
	ImGui_Goal();
}

void Goal::ImGui_Goal()
{
	ImGui::Begin("Goal Manager");

	char nodeName[64];
	sprintf_s(nodeName, "Goal Object [%p]", this);

	if (ImGui::TreeNode(nodeName))
	{
		ImGui::PushID(this);
		if (m_Sphere)
		{
			if (ImGui::CollapsingHeader("Goal_Sphere", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::DragFloat3("Sphere Pos", &m_SpherePos.x, 0.1f);
				ImGui::DragFloat3("Sphere Scale", &m_SphereScale.x, 0.1f);

				float alpha = m_Sphere->GetAlpha();
				if (ImGui::SliderFloat("Alpha", &alpha, 0.0f, 1.0f))
				{
					m_Sphere->SetAlpha(alpha);
				}
			}
		}

		if (m_Poal_L && m_Poal_R)
		{
			if (ImGui::CollapsingHeader("Goal_Poals", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::DragFloat3("Poles Offset", &m_PoleOffset.x, 0.1f);
				ImGui::DragFloat3("Poles Scale", &m_PoleScale.x, 0.1f);

				float alpha = m_Poal_L->GetAlpha();
				if (ImGui::SliderFloat("Poals Alpha", &alpha, 0.0f, 1.0f))
				{
					m_Poal_L->SetAlpha(alpha);
					m_Poal_R->SetAlpha(alpha);
				}
			}
		}
		ImGui::PopID();
		ImGui::TreePop();
	}
	ImGui::End();
}

std::vector<CollisionOBB> Goal::GetPoalsOBB() const
{
	std::vector<CollisionOBB> obbs;

	// 左右の柱を配列に保存
	const Debug_Box* poals[2] = { m_Poal_L.get(),m_Poal_R.get() };

	for (int i = 0; i < 2; ++i)
	{
		if (!poals[i]) continue;
		CollisionOBB obb;

		// 柱それぞれのワールド行列を取得
		XMMATRIX world = poals[i]->GetWorldMatrix();

		// 当たり判定用メッシュの現在位置を取得
		obb.center = poals[i]->GetPos();
		obb.axis[0] = DirectX::XMVector3Normalize(world.r[0]);	// 右方向(X)
		obb.axis[1] = DirectX::XMVector3Normalize(world.r[1]);	// 上方向(Y)
		obb.axis[2] = DirectX::XMVector3Normalize(world.r[2]);	// 前方向(Z)
	
		obb.radius = XMLoadFloat3(&m_PoleScale) * 0.5f;
		obbs.push_back(obb);
	}
	return obbs;
}