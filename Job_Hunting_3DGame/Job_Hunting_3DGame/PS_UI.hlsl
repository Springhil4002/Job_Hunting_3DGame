cbuffer Transform : register(b0)
{
    float4x4 world;     // ワールド行列
    float4x4 view;      // ビュー行列
    float4x4 proj;      // 投影行列
    float time;         // 時間
    float3 cameraPos;   // カメラ位置
    float alpha;        // 透明度
}

struct PS_IN
{
    float4 pos   : SV_Position;
    float2 uv    : TEXCOORD;
    float4 color : COLOR;
};

Texture2D tex : register(t0);
SamplerState smp : register(s0);

float4 PS_Main(PS_IN pin) : SV_Target
{
    float4 texColor = tex.Sample(smp, pin.uv);
    texColor.a *= alpha;
    return texColor * pin.color;
}