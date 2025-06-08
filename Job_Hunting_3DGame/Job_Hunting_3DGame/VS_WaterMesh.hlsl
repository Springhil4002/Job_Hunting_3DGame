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
    float4 svpos    : SV_POSITION;  // 変換された座標
    float4 color    : COLOR;        // 変換された色
    float2 uv       : TEXCOORD;     // 変換されたUV
    float3 normal   : NORMAL;       // 変換された法線
};

VSOutput VS_Main(VSInput vin)
{
    float3 localPos = vin.pos;
    float3 normal_WS = normalize(mul((float3x3) world, vin.normal));
    
    // ローカル座標をワールド座標に変換
    float4 worldPos4 = mul(world, float4(localPos, 1.0f));
    float3 worldPos = worldPos4.xyz;
    
    // 波による変位ベクトル(初期値ゼロ)
    float3 waveOffset = float3(0.0f, 0.0f, 0.0f);
    
    // Gerstner波の計算(ワールド座標ベース)
    for (int i = 0; i < WAVE_COUNT;++i)
    {
        float amp = amplitude[i].x;
        float2 dir = normalize(direction[i].xy);
        float len = waveLength[i].x;
        float spd = speed[i].x;

        float freq = 2.0f * 3.14159f / len;
        float angular = freq * spd;
        
        float d = dot(dir, worldPos.xz);
        float phase = (freq * d) + (time * angular);
        
        // Gerstner波の変位を加算
        waveOffset.x += dir.x * (amp * cos(phase));
        waveOffset.z += dir.y * (amp * cos(phase));
        waveOffset.y += amp * sin(phase);
    }
    
    // ワールド座標に波の変位を加算
    worldPos += waveOffset;
    // ビュー変換
    float4 viewPos = mul(view, float4(worldPos, 1.0f));
    // プロジェクション変換
    float4 projPos = mul(proj, viewPos);
    
    VSOutput vout;
    vout.svpos = projPos;
    vout.color = vin.color;
    vout.uv = vin.uv;
    vout.normal = normal_WS;
    return vout;
}