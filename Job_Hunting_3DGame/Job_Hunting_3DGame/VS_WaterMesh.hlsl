cbuffer Transform : register(b0)
{
    float4x4 world;     // ワールド行列
    float4x4 view;      // ビュー行列
    float4x4 proj;      // 投影行列
    float time;         // 時間
    float3 cameraPos;   // カメラ位置
    float pad1;         // パディング
};

#define WAVE_COUNT (4)

cbuffer GerstnerParams : register(b1)
{
    float4 amplitude[WAVE_COUNT];   // (x = 振幅, yzw = 予備)
    float4 direction[WAVE_COUNT];   // (x,y = 方向ベクトル, zw = 予備)
    float4 waveLength[WAVE_COUNT];  // (x = 波長, yzw = 予備)
    float4 speed[WAVE_COUNT];       // (x = 波の速度, yzw = 予備)
};

struct VS_Input
{
    float3 pos : POSITION;      // 頂点座標
    float3 normal : NORMAL;     // 法線
    float2 uv : TEXCOORD;       // UV
    float3 tangent : TANGENT;   // 接空間
    float4 color : COLOR;       // 頂点色
};

struct VS_Out
{
    float4 svpos : SV_POSITION;     // 変換された座標
    float4 color : COLOR;           // 変換された色
    float2 uv : TEXCOORD;           // 変換されたUV
    float3 normal : NORMAL;         // 変換された法線
    float3 worldPos : POSITION1;    // 変換されたワールド空間座標(スペキュラ計算用)
};

VS_Out VS_Main(VS_Input vin)
{
    // ローカル座標の取得
    float3 localPos = vin.pos;
    
    // ローカル座標をワールド座標に変換
    float4 worldPos4 = mul(world, float4(localPos, 1.0f));
    float3 worldPos = worldPos4.xyz;
    
    // Gerstner波による変位ベクトルを初期化
    float3 waveOffset = float3(0.0f, 0.0f, 0.0f);
    
    // Gerstner波の法線計算用の接線ベクトルの初期化
    float3 tangentX = float3(1, 0, 0);
    float3 tangentZ = float3(0, 0, 1);
    
    // x方向の接線ベクトルの傾き成分
    float3 dx = float3(0, 0, 0); 
    // z方向の接線ベクトルの傾き成分
    float3 dz = float3(0, 0, 0); 
    
    // 複数のGerstner波の計算
    [unroll]
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
        
        // ゲルストナー波の接線計算
        float3 D = float3(dir.x, freq * amp * cosPhase, dir.y);
        
        // 接線方向への波の傾き成分を累積
        dx += D * dot(dir, tangentX.xz);
        dz += D * dot(dir, tangentZ.xz);
    }
    
    // ワールド座標に波の変位を加算
    worldPos += waveOffset;
    
    // ゲルストナー波による接線と従法線を計算
    float3 tangent = float3(1, 0, 0) + dx;
    float3 binormal = float3(0, 0, 1) + dz;
    float3 normal_WS = normalize(cross(tangent,binormal));
    
    // 高さ場の法線をワールド行列で変換
    float3 normal_HF = normalize(mul((float3x3) world, normal_WS));
    
    // ゲルストナー波の法線に高さ場の法線を合成
    float3 unionNormal_WS_HF = normalize(mul((float3x3) world, vin.normal));
    
    // 最終法線
    const float HF_WEIGHT = 0.46f;
    float3 finalNormal = normalize(
        unionNormal_WS_HF * (1.0f - HF_WEIGHT) +
        normal_HF * HF_WEIGHT
    );
    
    // ビュー変換とプロジェクション変換
    float4 viewPos = mul(view, float4(worldPos, 1.0f));
    float4 projPos = mul(proj, viewPos);
    
    VS_Out vout;
    vout.svpos = projPos;
    vout.color = vin.color;
    vout.uv = vin.uv;
    vout.normal = finalNormal;
    vout.worldPos = worldPos;
    return vout;
};