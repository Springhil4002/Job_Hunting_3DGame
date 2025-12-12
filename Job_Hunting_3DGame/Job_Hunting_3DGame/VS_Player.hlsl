cbuffer Transform : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
    float3 cameraPos;
    float pad;
};

cbuffer LightData : register(b1)
{
    float3 lightDir; // ライト方向 
    float envStrength; // 環境光の強さ
    float4 lightColor; // ライトカラー
};

struct VS_IN
{
    float3 pos : POSITION; // 頂点座標
    float3 normal : NORMAL; // 法線
    float2 uv : TEXCOORD; // UV
    float3 tangent : TANGENT; // 接空間
    float4 color : COLOR; // 頂点カラー
};

struct VS_OUT
{
    float4 svpos : SV_POSITION; // 変換された座標
    float4 color : COLOR; // 変換された色
    float2 uv : TEXCOORD; // UV
    float3 worldPos : POSITION; // 位置
    float3 worldNormal : NORMAL; // 法線
};

VS_OUT VS_Main(VS_IN vin)
{
    VS_OUT vout;
    
    float4 localPos = float4(vin.pos, 1.0f); // 頂点座標
    float4 worldPos = mul(world, localPos); // ワールド座標に変換
    float4 viewPos = mul(view, worldPos); // ビュー座標に変換
    float4 projPos = mul(proj, viewPos); // 投影変換
    
    vout.svpos = projPos;
    vout.color = vin.color;
    vout.uv = vin.uv;
    
    vout.worldPos = worldPos.xyz;
    vout.worldNormal = normalize(mul((float3x3) world, vin.normal));
    
    return vout;
}