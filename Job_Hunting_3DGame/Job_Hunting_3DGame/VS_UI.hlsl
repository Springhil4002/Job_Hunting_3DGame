cbuffer Transform : register(b0)
{
    float4x4 world;     // ワールド行列
    float4x4 view;      // ビュー行列
    float4x4 proj;      // 投影行列
    float alpha;        // 透明度
    float4 uv;          // UV
};

struct VS_IN
{
    float3 pos   : POSITION;
    float2 uv    : TEXCOORD;
    float4 color : COLOR;
};

struct VS_OUT
{
    float4 pos   : SV_Position;
    float2 uv    : TEXCOORD;
    float4 color : COLOR;
};

VS_OUT VS_Main(VS_IN vin)
{
    VS_OUT vout;
    
    float4 worldPos = mul(world, float4(vin.pos, 1.0f));
    worldPos.z = 0.0f;
    vout.pos = mul(proj, worldPos);
    
    vout.uv.x = lerp(uv.x, uv.z, vin.uv.x);
    vout.uv.y = lerp(uv.y, uv.w, vin.uv.y);
    vout.color = vin.color;
    
    return vout;
}