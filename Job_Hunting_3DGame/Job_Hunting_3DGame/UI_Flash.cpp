#include "UI_Flash.h"
#include "Debug_New.h"

using namespace DirectX;

std::unique_ptr<Object> UI_Flash::clone() const
{
    auto newObj = std::make_unique<UI_Flash>();
    return newObj;
}

bool UI_Flash::Init(Camera2D* _cameraUI, float _width, float _height,
    const std::wstring& _filePath)
{
    m_FlashSpeed = 0.01f;

    if (!UI::Init(_cameraUI, _width, _height, _filePath))
    {
        printf("UI_Flash:初期化処理に失敗\n\n");
        return false;
    }

    printf("UI_Flash:初期化処理に成功\n\n");
    return true;
}

void UI_Flash::Update()
{
    if (m_IsFlashing)
    {
        Update_Flash(); 
    }
    UI::UpdateCameraMatrix();
}

void UI_Flash::Draw()
{
    UI::Draw();
}

void UI_Flash::Uninit()
{
    UI::Uninit();
}

void UI_Flash::Update_Flash()
{
    if (m_State == FADE_STATE::FADE_STATE_IN)
    {
        m_Alpha += m_FlashSpeed;
        if (m_Alpha >= 1.0f)
        {
            m_Alpha = 1.0f;
            m_State = FADE_STATE::FADE_STATE_OUT;
            //printf("UI_Flash:透明度減少開始\n");
        }
    }
    else if (m_State == FADE_STATE::FADE_STATE_OUT)
    {
        m_Alpha -= m_FlashSpeed;
        if (m_Alpha <= 0.0f)
        {
            m_Alpha = 0.0f;
            m_State = FADE_STATE::FADE_STATE_IN;
            //printf("UI_Fade:透明度上昇開始\n");
        }
    }

    UI::UpdateTransform();
    UI::UpdateCameraMatrix();
}

void UI_Flash::SetFlashIn()
{
    m_State = FADE_STATE::FADE_STATE_IN;
    m_IsFlashing = true;
}

void UI_Flash::SetFlashOut()
{
    m_State = FADE_STATE::FADE_STATE_OUT;
    m_IsFlashing = true;
}

void UI_Flash::SetFlashNone()
{
    m_State = FADE_STATE::FADE_STATE_NONE;
    m_IsFlashing = false;
}

void UI_Flash::SetFlashSpeed(float _speed)
{
    m_FlashSpeed = _speed;
}