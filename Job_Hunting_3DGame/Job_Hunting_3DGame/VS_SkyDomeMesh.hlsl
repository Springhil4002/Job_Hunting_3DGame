cbuffer Transform : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
    float time;
    float3 cameraPos;
    float pad1;
};

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 uv  : TEXCOORD;
};

struct VS_OUT
{
    float4 svpos : SV_POSITION;
    float2 uv    : TEXCOORD;
};

VS_OUT VS_Main(VS_INPUT vin)
{
    VS_OUT vout;
    
    float4x4 identity =
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    // カメラ位置へスカイドームを追従させる(移動部分だけ)
    float4 worldPos = mul(world, float4(vin.pos, 1.0f));
    float4 viewPos = mul(view, worldPos);
    vout.svpos = mul(proj, viewPos);
    
    vout.uv = vin.uv;
    return vout;
}