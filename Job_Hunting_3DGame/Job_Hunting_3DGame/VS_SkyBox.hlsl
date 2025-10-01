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
};

struct VS_OUT
{
    float4 svpos : SV_POSITION;
    float3 dir : TEXCOORD;
};

VS_OUT VS_Main(VS_INPUT vin)
{
    VS_OUT vout;
    
    // ビュー行列の平行移動成分を無視(回転のみ適用)
    float3 viewDir = mul((float3x3)view, vin.pos);
    vout.svpos = mul(proj, float4(viewDir, 1.0f));
    
    // キューブマップ参照用の方向ベクトル
    vout.dir = float3(viewDir.x, -viewDir.y, viewDir.z);    
    return vout;
}