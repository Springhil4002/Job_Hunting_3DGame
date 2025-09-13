#include "UI.h"
#include "Debug_New.h"

using namespace DirectX;

std::unique_ptr<Object> UI::clone() const
{
    auto newObj = std::make_unique<UI>();
    return newObj;
}

bool UI::Init(Camera2D* _cameraUI, float _width, float _height)
{
    m_CameraUI = _cameraUI;
    m_SizeWidth = _width;
    m_SizeHeight = _height;
    if (!m_CameraUI || 
        m_SizeWidth == 0.0f || 
        m_SizeHeight == 0.0f) 
        return false;

    auto mesh = CreateQuad(0, 0, 
        m_SizeWidth, m_SizeHeight, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
    auto vertexSize = sizeof(VertexUI) * std::size(mesh.vertices);
    auto vertexStride = sizeof(VertexUI);

    m_pVertexBuffer = std::make_unique<VertexBuffer>(vertexSize, vertexStride, mesh.vertices.data());
    if (!m_pVertexBuffer->IsValid())
    {
        printf("UI:頂点バッファ生成失敗\n");
        return false;
    }

    auto indexSize = sizeof(uint32_t) * std::size(mesh.indices);
    m_pIndexBuffer = std::make_unique<IndexBuffer>(indexSize, mesh.indices.data());
    if (!m_pIndexBuffer->IsValid())
    {
        printf("UI:インデックスバッファ生成失敗\n");
        return false;
    }

    for (size_t i = 0; i < DrawBase::FRAME_BUFFER_COUNT; ++i)
    {
        m_pConstantBuffer[i] = std::make_unique<ConstantBuffer>(sizeof(Matrix));
        if (!m_pConstantBuffer[i]->IsValid())
        {
            printf("UI:コンスタントバッファ生成失敗\n");
            return false;
        }

        // カメラの初期化
        auto ptr = m_pConstantBuffer[i]->GetPtr<Matrix>();
        ptr->world = XMMatrixIdentity();
        ptr->view = m_CameraUI->GetViewMatrix();
        ptr->proj = m_CameraUI->GetProjMatrix();
    }

    // ディスクリプタヒープ
    m_pDescriptorHeap = std::make_unique<DescriptorHeap>();

    auto tex = TextureManager::Instance().GetTexture(L"Assets/Texture/hogehoge.png");
    if (!tex)
    {
        printf("UI:画像読み込み失敗\n");
        return false;
    }
    m_pTexHandle = m_pDescriptorHeap->Register(tex.get());

    auto& rootManager = RootSignatureManager::GetInstance();
    m_pRootSignature = rootManager.GetRoot(Root_Type::ROOT_TYPE_UI);
    if (!m_pRootSignature->IsValid())
    {
        printf("UI:ルートシグネチャの生成に失敗\n");
        return false;
    }

    // マネージャー経由でパイプラインステートを取得
    auto& psoManager = PipelineState_Manager::GetInstance();
    m_pPipelineState = psoManager.GetPSO_UI(PSO_Type::PSO_TYPE_UI);
    if (!m_pPipelineState->IsValid())
    {
        // 頂点レイアウトの設定
        m_pPipelineState->SetInputLayout(VertexUI::InputLayout);
        // ルートシグネチャの設定
        m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
        // VS/PSの設定
#ifdef _DEBUG	// DEBUG
        m_pPipelineState->SetVS(L"../x64/Debug/VS_UI.cso");
        m_pPipelineState->SetPS(L"../x64/Debug/PS_UI.cso");
#else			// Release
        m_pPipelineState->SetVS(L"../x64/Release/VS_UI.cso");
        m_pPipelineState->SetPS(L"../x64/Release/PS_UI.cso");
#endif 
        // パイプラインステート作成
        m_pPipelineState->Create();
    }

    if (!m_pPipelineState->IsValid())
    {
        printf("UI:パイプラインステートの生成に失敗\n");
        return false;
    }

    printf("UI:初期化処理に成功\n\n");
    return true;
}

void UI::Update()
{
    UpdateTransform();
    UpdateCameraMatrix();
}

void UI::Draw()
{
    // 現在のフレーム番号取得
    auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
    // コマンドリスト
    auto cmdList = g_DrawBase->CommandList();
    // 頂点バッファ
    auto vbView = m_pVertexBuffer->View();
    // インデックスバッファ
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
    // ディスクリプタテーブル
    cmdList->SetGraphicsRootDescriptorTable(1, m_pTexHandle->handleGPU);
    // 三角形
    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmdList->IASetVertexBuffers(0, 1, &vbView);
    cmdList->IASetIndexBuffer(&ibView);
    // 描画
    cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void UI::Uninit()
{
    m_CameraUI = nullptr;
    m_pVertexBuffer.reset();
    m_pIndexBuffer.reset();
    for (auto& cb : m_pConstantBuffer)
        cb.reset();
    m_pDescriptorHeap.reset();
    m_pRootSignature.reset();
    m_pTexHandle.reset();
}

Mesh_UI UI::CreateQuad(float _x, float _y, float _w, float _h, XMFLOAT4 _color)
{
    Mesh_UI mesh;
    
    float halfWidth = _w * 0.5f;
    float halfHeight = _h * 0.5f;

    // 四角形の中心座標が(x,y)になるようにオフセット
    float left   = _x - halfWidth;
    float right  = _x + halfWidth;
    float top    = _y - halfHeight;
    float bottom = _y + halfHeight;

    // 頂点作成
    mesh.vertices.resize(4);
    mesh.vertices[0] = { {left,  top,    0.0f}, {0.0f, 1.0f}, _color };
    mesh.vertices[1] = { {right, top,    0.0f}, {1.0f, 1.0f}, _color };
    mesh.vertices[2] = { {left,  bottom, 0.0f}, {0.0f, 0.0f}, _color };
    mesh.vertices[3] = { {right, bottom, 0.0f}, {1.0f, 0.0f}, _color };

    // インデックス
    mesh.indices = { 0, 1, 2, 1, 3, 2};

    return mesh;
}

void UI::UpdateTransform()
{
    auto pos = GetPos();
    auto rota = GetRota();
    auto scale = GetScale();

    m_worldMatrix =
        DirectX::XMMatrixScalingFromVector(scale) *
        DirectX::XMMatrixRotationRollPitchYawFromVector(rota) *
        DirectX::XMMatrixTranslationFromVector(pos);
}

void UI::UpdateCameraMatrix()
{
    auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
    auto ptr = m_pConstantBuffer[currentIndex]->GetPtr<Matrix>();
    ptr->world = m_worldMatrix;
    ptr->view = m_CameraUI->GetViewMatrix();
    ptr->proj = m_CameraUI->GetProjMatrix();
}