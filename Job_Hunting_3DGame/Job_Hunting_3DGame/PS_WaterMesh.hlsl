cbuffer LightParams : register(b2)
{
    float3 lightDir;    // ライトの方向
    float pad0;         // パディング
    float4 lightColor;  // ライトの色
}

struct VSOutput
{
    float4 svpos    : SV_POSITION;  // 頂点シェーダーから来た座標
    float4 color    : COLOR;        // 頂点シェーダーから来た色
    float2 uv       : TEXCOORD;     // 頂点シェーダーからきたUV
    float3 normal   : NORMAL;       // 頂点シェーダーからきた法線
};

Texture2D tex : register(t0);       // テクスチャ
SamplerState smp : register(s0);    // サンプラー

float4 PS_Main(VSOutput pin) : SV_TARGET
{
    float3 N= normalize(pin.normal);
    float3 L = normalize(-lightDir);    // ライト方向
    
    // ランバート反射
    float NdotL = max(dot(N, L), 0.0f);
    
    float4 texColor = tex.Sample(smp, pin.uv);
    float4 diffuse = texColor * pin.color * lightColor * NdotL;
    
    return diffuse;
}