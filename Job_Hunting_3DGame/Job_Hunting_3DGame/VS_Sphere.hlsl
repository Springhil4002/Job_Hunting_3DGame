cbuffer Transform : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
    float time;
    float3 cameraPos;
    float pad1;
};

struct VS_IN
{
    float3 pos : POSITION;      // 頂点座標
    float3 normal : NORMAL;     // 法線
    float2 uv : TEXCOORD;       // UV
    float3 tangent : TANGENT;   // 接空間
    float4 color : COLOR;       // 頂点カラー
};

struct VS_OUT
{
    float4 svpos : SV_POSITION; // 変換された座標
    float4 color : COLOR; // 変換された色
    float2 uv : TEXCOORD; // UV  
};

VS_OUT VS_Main(VS_IN vin)
{
    VS_OUT vout;
    
    float4 localPos = float4(vin.pos, 1.0f); // 頂点座標
    float4 worldPos = mul(world, localPos); // ワールド座標に変換
    float4 viewPos = mul(view, worldPos); // ビュー座標に変換
    float4 projPos = mul(proj, viewPos); // 投影変換
    
    vout.svpos = projPos; // 投影変換された座標をピクセルシェーダーに渡す
    vout.color = vin.color; // 頂点カラーをそのままピクセルシェーダーに渡す
    vout.uv = vin.uv; // UVを入力から渡す
    
    return vout;
}