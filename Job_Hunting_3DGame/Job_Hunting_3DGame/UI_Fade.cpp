#include "UI_Fade.h"
#include "Debug_New.h"

using namespace DirectX;

std::unique_ptr<Object> UI_Fade::clone() const
{
    auto newObj = std::make_unique<UI_Fade>();
    return newObj;
}

bool UI_Fade::Init(Camera2D* _cameraUI, float _width, float _height)
{
	m_CameraUI = _cameraUI;
	if (!m_CameraUI) return false;
	m_SizeWidth = _width;
	m_SizeHeight = _height;

	auto mesh = CreateQuad(0, 0, m_SizeWidth, m_SizeHeight, XMFLOAT4(1, 1, 1, m_Alpha));
	auto vertexSize = sizeof(VertexUI) * std::size(mesh.vertices);
	auto vertexStride = sizeof(VertexUI);

    m_pVertexBuffer = std::make_unique<VertexBuffer>(vertexSize, vertexStride, mesh.vertices.data());
    if (!m_pVertexBuffer->IsValid()) 
    {
        printf("UI_Fade:頂点バッファ生成失敗\n");
        return false;
    }
    auto indexSize = sizeof(uint32_t) * std::size(mesh.indices);
    m_pIndexBuffer = std::make_unique<IndexBuffer>(indexSize, mesh.indices.data());
    if (!m_pIndexBuffer->IsValid()) 
    {
        printf("UI_Fade:インデックスバッファ生成失敗\n");
        return false;
    }
    for (size_t i = 0; i < DrawBase::FRAME_BUFFER_COUNT; ++i)
    {
        m_pConstantBuffer[i] = std::make_unique<ConstantBuffer>(sizeof(Matrix));
        if (!m_pConstantBuffer[i]->IsValid()) 
        {
            printf("UI_Fade:コンスタントバッファ生成失敗\n");
            return false;
        }
        auto ptr = m_pConstantBuffer[i]->GetPtr<Matrix>();
        ptr->world = XMMatrixIdentity();
        ptr->view = m_CameraUI->GetViewMatrix();
        ptr->proj = m_CameraUI->GetProjMatrix();
        ptr->alpha = m_Alpha;
    }

    m_pDescriptorHeap = std::make_unique<DescriptorHeap>();

    auto tex = TextureManager::Instance().GetTexture(L"Assets/Texture/Color_Black.png");
    if (!tex)
    {
        printf("UI_Fade:画像読み込み失敗\n");
        return false;
    }

    m_pTexHandle = m_pDescriptorHeap->Register(tex.get());

    auto& rootManager = RootSignatureManager::GetInstance();
    m_pRootSignature = rootManager.GetRoot(Root_Type::ROOT_TYPE_UI);
    if (!m_pRootSignature->IsValid())
    {
        printf("UI_Fade:ルートシグネチャの生成に失敗\n");
        return false;
    }

    auto& psoManager = PipelineState_Manager::GetInstance();
    m_pPipelineState = psoManager.GetPSO_UI(PSO_Type::PSO_TYPE_UI);

    if (!m_pPipelineState->IsValid())
    {
        m_pPipelineState->SetInputLayout(VertexUI::InputLayout);
        m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
#ifdef _DEBUG
        m_pPipelineState->SetVS(L"../x64/Debug/VS_UI.cso");
        m_pPipelineState->SetPS(L"../x64/Debug/PS_UI.cso");
#else
        m_pPipelineState->SetVS(L"../x64/Release/VS_UI.cso");
        m_pPipelineState->SetPS(L"../x64/Release/PS_UI.cso");
#endif
        m_pPipelineState->Create();
    }

    if (!m_pPipelineState->IsValid())
    {
        printf("UI_Fade:パイプラインステートの生成に失敗\n");
        return false;
    }

    printf("UI_Fade:初期化処理に成功\n\n");
    return true;
}

void UI_Fade::Update()
{
	Update_Fade();
    UI::UpdateCameraMatrix();
}

void UI_Fade::Draw()
{
	UI::Draw();
}

void UI_Fade::Uninit()
{
	UI::Uninit();
}

void UI_Fade::Update_Fade()
{
    m_FadeFinished = false;
    if (m_State == FADE_STATE::FADE_STATE_IN)
    {
        m_Alpha += m_FadeSpeed;
        if (m_Alpha >= 1.0f)
        {
            m_Alpha = 1.0f;
            m_State = FADE_STATE::FADE_STATE_NONE;
            printf("UI_Fade:フェードイン完了\n");
            m_FadeFinished = true;
        }
    }
    else if (m_State == FADE_STATE::FADE_STATE_OUT)
    {
        m_Alpha -= m_FadeSpeed;
        if (m_Alpha <= 0.0f)
        {
            m_Alpha = 0.0f;
            m_State = FADE_STATE::FADE_STATE_NONE;
            printf("UI_Fade:フェードアウト完了\n");
            m_FadeFinished = true;
        }
    }

    auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
    auto ptr = m_pConstantBuffer[currentIndex]->GetPtr<Matrix>();
    ptr->world = m_worldMatrix;
    ptr->view = m_CameraUI->GetViewMatrix();
    ptr->proj = m_CameraUI->GetProjMatrix();
    ptr->alpha = m_Alpha;

    UI::UpdateTransform();
}

void UI_Fade::SetFadeIn()
{
    m_State = FADE_STATE::FADE_STATE_IN;
}

void UI_Fade::SetFadeOut()
{
    m_State = FADE_STATE::FADE_STATE_OUT;
}

bool UI_Fade::IsFadeFinished() const 
{
    return m_FadeFinished;
}

FADE_STATE UI_Fade::GetState() const
{
    return m_State;
}