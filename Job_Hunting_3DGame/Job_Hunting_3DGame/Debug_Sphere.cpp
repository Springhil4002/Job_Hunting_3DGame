#include "Debug_Sphere.h"
#include "Debug_New.h"

using namespace DirectX;

std::unique_ptr<Object> Debug_Sphere::clone() const
{
    auto newObj = std::make_unique<Debug_Sphere>();
	newObj->m_alpha = m_alpha;
    newObj->m_radius = m_radius;
    return newObj;
}

bool Debug_Sphere::Init(Camera* _camera)
{
	m_camera = _camera;

    Create_Sphere(32, 64, m_radius);

    // 頂点バッファ生成
    auto vbSize = sizeof(Vertex) * vertices.size();
    auto vbStride = sizeof(Vertex);
    m_pVertexBuffer = std::make_unique<VertexBuffer>(vbSize, vbStride, vertices.data());
    if (!m_pVertexBuffer->IsValid())
    {
        printf("Sphere:頂点バッファ生成失敗\n");
        return false;
    }

	// インデックスバッファ生成
    auto ibSize = sizeof(uint32_t) * indices.size();
    m_pIndexBuffer = std::make_unique<IndexBuffer>(ibSize, indices.data());
    if (!m_pIndexBuffer->IsValid())
    {
        printf("Sphere:インデックスバッファ生成失敗\n");
        return false;
    }

	// コンスタントバッファ生成
    for (size_t i = 0; i < DrawBase::FRAME_BUFFER_COUNT; ++i)
    {
        m_pConstantBuffer[i] = std::make_unique<ConstantBuffer>(sizeof(Matrix));
        if (!m_pConstantBuffer[i]->IsValid())
        {
            printf("Sphere:コンスタントバッファ生成失敗\n");
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
    m_pRootSignature = std::make_unique<RootSignature_DebugSphere>();
    if (!m_pRootSignature->IsValid())
    {
        printf("Sphere:ルートシグネチャ生成失敗\n");
        return false;
    }

	m_pPipelineState = std::make_unique<PipelineState_DebugSphere>();
	m_pPipelineState->SetInputLayout(Vertex::InputLayout);
	m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
#ifdef _DEBUG
	m_pPipelineState->SetVS(L"../x64/Debug/VS_Sphere.cso");
	m_pPipelineState->SetPS(L"../x64/Debug/PS_Sphere.cso");
#else
	m_pPipelineState->SetVS(L"../x64/Release/VS_Sphere.cso");
	m_pPipelineState->SetPS(L"../x64/Release/PS_Sphere.cso");
#endif
	m_pPipelineState->Create();
    if (!m_pPipelineState->IsValid())
    {
        printf("Sphere:パイプラインステート生成失敗\n");
        return false;
	}

    printf("Sphere:初期化処理に成功\n");
    return true;
}

void Debug_Sphere::Update()
{
    Update_Transform();
	Update_CameraMatrix();
}

void Debug_Sphere::Draw()
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

void Debug_Sphere::Uninit()
{
    m_camera = nullptr;
    m_pVertexBuffer.reset();
    m_pIndexBuffer.reset();
    for (auto& cb : m_pConstantBuffer)
        cb.reset();
    m_pDescriptorHeap.reset();
    m_pRootSignature.reset();
    m_pPipelineState.reset();
    m_pTexHandle.reset();
}

void Debug_Sphere::Create_Sphere(float _stacks, float _slices, float _radius)
{
    vertices.clear();
    indices.clear();

    for (int i = 0; i <= _stacks; i++) {
        float phi = XM_PI * i / _stacks;
        float y = cosf(phi);
        float r = sinf(phi);

        for (int j = 0; j <= _slices; j++) {
            float theta = XM_2PI * j / _slices;
            float x = r * cosf(theta);
            float z = r * sinf(theta);

            XMFLOAT3 pos = { x * _radius, y * _radius, z * _radius };
            XMFLOAT3 normal = { x, y, z };
            XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&normal));
            XMStoreFloat3(&normal, n);

            XMFLOAT3 tangent;
            {
                XMVECTOR t = XMVector3Normalize(XMVectorSet(-sinf(theta), 0.0f, cosf(theta), 0.0f));
                XMStoreFloat3(&tangent, t);
            }

            float u = (float)j / _slices; // [0,1]
            float v = (float)i / _stacks; // [0,1]

            XMFLOAT4 color = { 1, 1, 1, 1 };

            Vertex vertex = {};
			vertex.position = pos;
			vertex.normal = normal;
			vertex.uv = XMFLOAT2(u, v);
			vertex.tangent = tangent;
			vertex.color = color;

            vertices.push_back(vertex);
        }
    }

    // インデックス生成
    for (int i = 0; i < _stacks; i++) {
        for (int j = 0; j < _slices; j++) {
            int first = i * (_slices + 1) + j;
            int second = first + _slices + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
}

void Debug_Sphere::Update_Transform()
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

void Debug_Sphere::Update_CameraMatrix()
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