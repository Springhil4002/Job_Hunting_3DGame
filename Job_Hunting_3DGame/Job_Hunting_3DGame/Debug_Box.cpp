#include "Debug_Box.h"
#include "Debug_New.h"
#include "Debug_Msg.h"

using namespace DirectX;

std::unique_ptr<Object> Debug_Box::clone() const
{
	auto newObj = std::make_unique<Debug_Box>();
	newObj->m_alpha = m_alpha;
	return newObj;
}

bool Debug_Box::Init(Camera* _camera)
{
	m_camera = _camera;

	Create_Cube();

    // 頂点バッファ生成
    auto vbSize = sizeof(Vertex) * vertices.size();
    auto vbStride = sizeof(Vertex);
    m_pVertexBuffer = std::make_unique<VertexBuffer>(vbSize, vbStride, vertices.data());
    if (!m_pVertexBuffer->IsValid())
    {
        DEBUG_LOG_ERROR("Box:頂点バッファ生成失敗");
        return false;
    }

    // インデックスバッファ生成
    auto ibSize = sizeof(uint32_t) * indices.size();
    m_pIndexBuffer = std::make_unique<IndexBuffer>(ibSize, indices.data());
    if (!m_pIndexBuffer->IsValid())
    {
        DEBUG_LOG_ERROR("Box:インデックスバッファ生成失敗");
        return false;
    }

    // コンスタントバッファ生成
    for (size_t i = 0; i < DrawBase::FRAME_BUFFER_COUNT; ++i)
    {
        m_pConstantBuffer[i] = std::make_unique<ConstantBuffer>(sizeof(Matrix));
        if (!m_pConstantBuffer[i]->IsValid())
        {
            DEBUG_LOG_ERROR("Box:コンスタントバッファ生成失敗");
            return false;
        }

        // カメラの初期化
        auto ptr = m_pConstantBuffer[i]->GetPtr<Matrix>();
        ptr->world = DirectX::XMMatrixIdentity();
        ptr->view = m_camera->GetViewMatrix();
        ptr->proj = m_camera->GetProjMatrix();
    }

    // ディスクリプタヒープ
    m_pDescriptorHeap = std::make_unique<DescriptorHeap>();

    // ルートシグネチャ生成
    auto& rootManager = RootSignatureManager::GetInstance();
    m_pRootSignature = rootManager.GetRoot(Root_Type::ROOT_TYPE_DEBUGMESH);
    if (!m_pRootSignature->IsValid())
    {
        DEBUG_LOG_ERROR("Box:ルートシグネチャ生成失敗");
        return false;
    }

    auto& psoManager = PipelineState_Manager::GetInstance();
    m_pPipelineState = psoManager.GetPSO_DebugSphere(PSO_Type::PSO_TYPE_DEBUGMESH);
    if (!m_pPipelineState->IsValid())
    {
        // 頂点レイアウトの設定
        m_pPipelineState->SetInputLayout(Vertex::InputLayout);
        // ルートシグネチャの設定
        m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
        // VS/PSの設定
#ifdef _DEBUG	// DEBUG
        m_pPipelineState->SetVS(L"../x64/Debug/VS_DebugMesh.cso");
        m_pPipelineState->SetPS(L"../x64/Debug/PS_DebugMesh.cso");
#else			// Release
        m_pPipelineState->SetVS(L"../x64/Release/VS_DebugMesh.cso");
        m_pPipelineState->SetPS(L"../x64/Release/PS_DebugMesh.cso");
#endif 
        // パイプラインステート作成
        m_pPipelineState->Create();
    }

    if (!m_pPipelineState->IsValid())
    {
        DEBUG_LOG_ERROR("Box:パイプラインステート生成失敗");
        return false;
    }

    DEBUG_LOG("Box:初期化処理に成功");
    return true;
}

void Debug_Box::Update()
{
    Update_Transform();
    Update_CameraMatrix();
}

void Debug_Box::Draw()
{
    // 現在のフレーム番号取得
    auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
    // コマンドリスト
    auto cmdList = g_DrawBase->CommandList();
    // 頂点バッファビュー
    auto vbView = m_pVertexBuffer->View();
    // インデックスバッファビュー
    auto ibView = m_pIndexBuffer->View();
    // ディスクリプタヒープ
    auto Heap = m_pDescriptorHeap->GetHeap();

    // ルートシグネチャをセット
    cmdList->SetGraphicsRootSignature(m_pRootSignature->Get());
    // パイプラインステートをセット
    cmdList->SetPipelineState(m_pPipelineState->Get());
    // 定数バッファをセット
    cmdList->SetGraphicsRootConstantBufferView(0, m_pConstantBuffer[currentIndex]->GetAddress());
    // ディスクリプタヒープをセット
    cmdList->SetDescriptorHeaps(1, &Heap);

    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmdList->IASetVertexBuffers(0, 1, &vbView);
    cmdList->IASetIndexBuffer(&ibView);

    cmdList->DrawIndexedInstanced(static_cast<UINT>(indices.size()), 1, 0, 0, 0);
}

void Debug_Box::UnInit()
{
    m_camera = nullptr;
    m_pVertexBuffer.reset();
    m_pIndexBuffer.reset();
    for (auto& cb : m_pConstantBuffer)
        cb.reset();
    m_pDescriptorHeap.reset();
}

void Debug_Box::Create_Cube()
{
    vertices.clear();
    indices.clear();

    struct CubeVertex
    {
        XMFLOAT3 pos;
        XMFLOAT3 normal;
    };

    CubeVertex cubeVertices[] = 
    {
        // 前面 (Z+)
        { {-0.5f, -0.5f,  0.5f}, {0, 0, 1} }, { {-0.5f,  0.5f,  0.5f}, {0, 0, 1} },
        { { 0.5f,  0.5f,  0.5f}, {0, 0, 1} }, { { 0.5f, -0.5f,  0.5f}, {0, 0, 1} },
        // 背面 (Z-)
        { { 0.5f, -0.5f, -0.5f}, {0, 0, -1} }, { { 0.5f,  0.5f, -0.5f}, {0, 0, -1} },
        { {-0.5f,  0.5f, -0.5f}, {0, 0, -1} }, { {-0.5f, -0.5f, -0.5f}, {0, 0, -1} },
        // 上面 (Y+)
        { {-0.5f,  0.5f,  0.5f}, {0, 1, 0} }, { {-0.5f,  0.5f, -0.5f}, {0, 1, 0} },
        { { 0.5f,  0.5f, -0.5f}, {0, 1, 0} }, { { 0.5f,  0.5f,  0.5f}, {0, 1, 0} },
        // 下面 (Y-)
        { {-0.5f, -0.5f, -0.5f}, {0, -1, 0} }, { { 0.5f, -0.5f, -0.5f}, {0, -1, 0} },
        { { 0.5f, -0.5f,  0.5f}, {0, -1, 0} }, { {-0.5f, -0.5f,  0.5f}, {0, -1, 0} },
        // 右面 (X+)
        { { 0.5f, -0.5f,  0.5f}, {1, 0, 0} }, { { 0.5f,  0.5f,  0.5f}, {1, 0, 0} },
        { { 0.5f,  0.5f, -0.5f}, {1, 0, 0} }, { { 0.5f, -0.5f, -0.5f}, {1, 0, 0} },
        // 左面 (X-)
        { {-0.5f, -0.5f, -0.5f}, {-1, 0, 0} }, { {-0.5f,  0.5f, -0.5f}, {-1, 0, 0} },
        { {-0.5f,  0.5f,  0.5f}, {-1, 0, 0} }, { {-0.5f, -0.5f,  0.5f}, {-1, 0, 0} },
    };

    for (int i = 0; i < 24; ++i) 
    {
        Vertex v = {};
        v.position = cubeVertices[i].pos;
        v.normal = cubeVertices[i].normal;
        v.uv = { 0.0f, 0.0f }; 
        v.color = { 1.0f, 1.0f, 1.0f, 1.0f };
        vertices.push_back(v);
    }

    // インデックスデータ
    for (int i = 0; i < 6; ++i) {
        uint32_t offset = i * 4;
        indices.push_back(offset + 0);
        indices.push_back(offset + 1);
        indices.push_back(offset + 2);
        indices.push_back(offset + 0);
        indices.push_back(offset + 3);
        indices.push_back(offset + 2);
    }
}

void Debug_Box::Update_Transform()
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

void Debug_Box::Update_CameraMatrix()
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

    // 透明度設定
    ptr->alpha = m_alpha;
}