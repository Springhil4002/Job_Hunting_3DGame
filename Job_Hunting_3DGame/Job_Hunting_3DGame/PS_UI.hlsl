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
    return texColor * pin.color;
}