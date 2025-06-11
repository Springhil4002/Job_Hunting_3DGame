cbuffer Transform : register(b0)
{
    float4x4 world;     // ワールド行列
    float4x4 view;      // ビュー行列
    float4x4 proj;      // 投影行列
    float time;         // 時間
    float3 cameraPos;   // カメラ位置
    float pad1;         // パディング
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
    float3 worldPos : POSITION1;    // 変換されたワールド空間座標(スペキュラ計算用)
};

VSOutput VS_Main(VSInput vin)
{
    float3 localPos = vin.pos;
    
    // ローカル座標をワールド座標に変換
    float4 worldPos4 = mul(world, float4(localPos, 1.0f));
    float3 worldPos = worldPos4.xyz;
    
    // 波による変位ベクトル(初期値ゼロ)
    float3 waveOffset = float3(0.0f, 0.0f, 0.0f);
    
    // 法線用の偏微分ベクトル
    float3 tangentX = float3(1, 0, 0);  // X方向の単位ベクトル(ワールド空間)
    float3 tangentZ = float3(0, 0, 1);  // Z方向の単位ベクトル(ワールド空間)
    
    float3 dx = float3(0, 0, 0);    // X方向の傾き
    float3 dz = float3(0, 0, 0);    // Z方向の傾き
    
    // Gerstner波の計算(ワールド座標ベース)
    for (int i = 0; i < WAVE_COUNT; ++i)
    {
        // 頂点の位置を計算
        float amp = amplitude[i].x;
        float2 dir = normalize(direction[i].xy);
        float len = waveLength[i].x;
        float spd = speed[i].x;

        float freq = 2.0f * 3.14159f / len;
        float angular = freq * spd;
        float d = dot(dir, worldPos.xz);
        float phase = (freq * d) + (time * angular);
        float cosPhase = cos(phase);
        float sinPhase = sin(phase);
        
        // Gerstner波の変位を加算
        waveOffset.x += dir.x * (amp * cosPhase);
        waveOffset.z += dir.y * (amp * cosPhase);
        waveOffset.y += amp * sinPhase;
        
        // 法線再計算用:偏微分
        float3 D = float3(dir.x, freq * amp * cosPhase, dir.y);
        dx += D * dot(dir, tangentX.xz);
        dz += D * dot(dir, tangentZ.xz);
    }
    
    // ワールド座標に波の変位を加算
    worldPos += waveOffset;
    
    // 接ベクトルの交差で法線を計算(偏微分)
    float3 binormal = float3(0, 1, 0) - dx; // y方向を中心に傾き修正
    float3 tangent = float3(0, 1, 0) - dz;
    float3 normal = normalize(cross(tangent, binormal));

    // 法線をワールド行列で変換
    float3 normal_WS = normalize(mul((float3x3) world, normal));
    
    // ビュー変換とプロジェクション変換
    float4 viewPos = mul(view, float4(worldPos, 1.0f));
    float4 projPos = mul(proj, viewPos);
    
    VSOutput vout;
    vout.svpos = projPos;
    vout.color = vin.color;
    vout.uv = vin.uv;
    vout.normal = normal_WS;
    vout.worldPos = worldPos;
    return vout;
}