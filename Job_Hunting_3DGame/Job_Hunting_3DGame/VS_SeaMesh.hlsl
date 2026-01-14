cbuffer Transform : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
    float time;
    float3 cameraPos;
};

struct VS_IN
{
    float3 pos      : POSITION; 
    float3 normal   : NORMAL;   
    float2 uv       : TEXCOORD; 
    float3 tangent  : TANGENT;  
    float4 color    : COLOR;    
};

struct VS_OUT
{
    float4 svpos : SV_POSITION; 
    float4 color : COLOR;
    float2 uv    : TEXCOORD;
    
    float3 worldPos    : POSITION1; 
    float3 worldNormal : NORMAL1;
    
    float3 worldTangent  : TANGENT1;
    float3 worldBinormal : BINORMAL1;
};

SamplerState smp : register(s0);
Texture2D normalTex : register(t1);
Texture2D normalTex2 : register(t2);

VS_OUT VS_Main(VS_IN vin)
{
    VS_OUT vout;

    float2 scrollUV1 = vin.uv;
    float2 scrollUV2 = vin.uv;
    scrollUV1.x += time * 0.005f;
    scrollUV1.y += time * 0.005f;
    scrollUV2.x += time * 0.005f;
    scrollUV2.y -= time * 0.005f;
    scrollUV2 *= 0.5f;
    
    // ノーマルマップのサンプリング
    float3 normalMap1 = normalTex.SampleLevel(smp, scrollUV1, 0).xyz * 2.0f - 1.0f;
    float3 normalMap2 = normalTex2.SampleLevel(smp, scrollUV2, 0).xyz * 2.0f - 1.0f;
    
    // 合成ノーマルマップ（変位量として使用）
    float3 normalMap = normalize(normalMap1 + normalMap2);
    float displacement = normalMap.y * 5.0f;
    
    // 頂点位置の変更
    float3 pos = vin.pos;
    pos.y += displacement; // 高さを変位させる
    
    float4 localPos = float4(pos, 1.0f); // 変更後の位置を使用
    float4 worldPos = mul(world, localPos);
    
    // 法線のワールド変換
    float3 normal = normalize(mul((float3x3) world, vin.normal));
    // 接線のワールド変換
    float3 tangent = normalize(mul((float3x3) world, vin.tangent));
    // バイノーマルの計算(法線と接線の外積)
    float3 binormal = normalize(cross(normal, tangent));
    
    float4 viewPos = mul(view, worldPos);
    float4 projPos = mul(proj, viewPos); 

    vout.svpos = projPos; 
    vout.color = vin.color;
    vout.uv = vin.uv;
    
    vout.worldPos = worldPos.xyz;
    vout.worldNormal = normal;
    
    vout.worldTangent = tangent;
    vout.worldBinormal = binormal;
    
    return vout;
}