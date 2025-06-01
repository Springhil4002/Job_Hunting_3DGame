struct VSOutput
{
    float4 svpos : SV_POSITION; // 頂点シェーダーから来た座標
    float4 color : COLOR;       // 頂点シェーダーから来た色
    float2 uv    : TEXCOORD;    // 頂点シェーダーからきたUV
};

Texture2D tex : register(t0);       // テクスチャ
SamplerState smp : register(s0);    // サンプラー

float4 PS_Main(VSOutput pin) : SV_TARGET
{
    float4 texColor = tex.Sample(smp, pin.uv);
    return texColor * pin.color;    // 頂点カラーとテクスチャ乗算
}