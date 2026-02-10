#include "UI_ActiveLine.h"
#include "Debug_New.h"
#include "Debug_Msg.h"

using namespace DirectX;

std::unique_ptr<Object> UI_ActiveLine::clone() const
{
    auto newObj = std::make_unique<UI_ActiveLine>();
    return newObj;
}

bool UI_ActiveLine::Init(Camera2D* _cameraUI, float _width, float _height,
    const std::wstring& _filePath,float _speedRatio)
{
    m_SpeedRatio = _speedRatio;
    m_AlphaStart = 0.55f;
    m_AlphaRatio = 0.3f;
    m_Alpha = 0.0f;
    m_AnimSpeed = 0.05f;
    m_TileX = 4;
    m_TileY = 4;

    if (!UI::Init(_cameraUI, _width, _height, _filePath))
    {
        DEBUG_LOG_ERROR("UI_ActiveLine:初期化処理に失敗");
        return false;
    }

    DEBUG_LOG("UI_ActiveLine:初期化処理に成功");
    return true;
}

void UI_ActiveLine::Update(float _deltaTime)
{
    Update_AnimUV(_deltaTime);
}

void UI_ActiveLine::Draw()
{
    if (m_Alpha <= 0.0f)return;
    UI::Draw();
}

void UI_ActiveLine::UnInit()
{
    UI::UnInit();
}

void UI_ActiveLine::Update_AnimUV(float _deltaTime)
{
    m_Timer += _deltaTime;

    // 透明度の制御
    if (m_SpeedRatio > m_AlphaStart)
    {
        float currentAlpha = (m_SpeedRatio - m_AlphaStart) / (1.0f - m_AlphaStart);

        m_Alpha = currentAlpha * m_AlphaRatio;
    }
    else
    {
        m_Alpha = 0.0f;
    }
    // 非表示なら計算スキップ
    if (m_Alpha <= 0.0f) return;

    // UVアニメーション
    if (m_Timer >= m_AnimSpeed)
    {
        m_Timer = 0.0f;
        
        int randomX = rand() % m_TileX;
        int randomY = rand() % m_TileY;
        
        float u = 1.0f / m_TileX;
        float v = 1.0f / m_TileY;

        float left = randomX * u;
        float top = randomY * v;

        m_UV = {
            left,
            top,
            left + u,
            top + v,
        };
    }
    UI::Update();
}