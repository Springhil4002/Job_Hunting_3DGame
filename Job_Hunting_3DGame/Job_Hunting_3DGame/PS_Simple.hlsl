struct VS_Output
{
    float4 svpos : SV_POSITION;     // 頂点シェーダーからきた座標
    float4 color : COLOR;           // 頂点シェーダーからきた色
    float2 uv    : TEXCOORD;        // 頂点シェーダーからきたUV
};

SamplerState smp : register(s0);    // サンプラー
Texture2D _MainTex : register(t0);  // テクスチャ

float4 pixel(VS_Output input) : SV_TARGET
{
    return _MainTex.Sample(smp, input.uv);
}
