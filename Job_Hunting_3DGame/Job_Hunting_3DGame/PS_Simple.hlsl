struct VS_Output
{
    float4 svpos : SV_POSITION;     // 頂点シェーダーからきた座標
    float4 color : COLOR;           // 頂点シェーダーからきた色
    float2 uv : TEXCOORD;           // 頂点シェーダーからきたUV
};

float4 pixel(VS_Output input) : SV_TARGET
{
    return float4(input.uv.xy, 1, 1);
}
