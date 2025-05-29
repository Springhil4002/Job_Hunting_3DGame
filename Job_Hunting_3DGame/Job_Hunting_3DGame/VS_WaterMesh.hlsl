cbuffer Transform : register(b0)
{
    float4x4 world;     // ワールド行列
    float4x4 view;      // ビュー行列
    float4x4 proj;      // 投影行列
    
    float time;     // 波のための時間
}

#define WAVE_COUNT (4)

cbuffer GerstnerParams : register(b1)
{
    float4 amplitude[WAVE_COUNT];   // (x = 振幅, yzw = 予備)
    float4 direction[WAVE_COUNT];   // (x,y = 方向ベクトル, zw = 予備)
    float4 waveLength[WAVE_COUNT];  // (x = 波長, yzw = 予備)
    float4 speed[WAVE_COUNT];       // (x = 波の速度, yzw = 予備)
}

struct VSInput
{
    float3 pos      : POSITION;  // 頂点座標
    float3 normal   : NORMAL;    // 法線
    float2 uv       : TEXCOORD;  // UV
    float3 tangent  : TANGENT;   // 接空間
    float4 color    : COLOR;     // 頂点色
};

struct VSOutput
{
    float4 svpos : SV_POSITION; // 変換された座標
    float4 color : COLOR;       // 変換された色
};

VSOutput VS_Main(VSInput vin)
{
    float3 position = vin.pos;
    float3 displaced = position;
    
    for (int i = 0; i < WAVE_COUNT;++i)
    {
        float amp = amplitude[i].x;
        float2 dir = normalize(direction[i].xy);
        float len = waveLength[i].x;
        float s = speed[i].x;
        
        // 波数
        float freq = 2.0f * 3.14159f /len;  
        // 角速度
        float angular = freq * s;
        float d = dot(dir, position.xz);
        float phase = (freq * d) + (time * angular);
        
        displaced.x += dir.x * (amp * cos(phase));
        displaced.z += dir.y * (amp * cos(phase));
        displaced.y += amp * sin(phase);
    }
    
    // ワールド座標に変換
    float4 worldPos = mul(world, float4(displaced,1.0f)); 
    // ビュー座標に変換
    float4 viewPos = mul(view, worldPos); 
    // 投影変換
    float4 projPos = mul(proj, viewPos);        

    VSOutput vout;
    
    vout.svpos = projPos;       // 投影変換された座標をピクセルシェーダーに渡す
    vout.color = vin.color;     // 頂点色をそのままピクセルシェーダーに渡す
    
    return vout;
}