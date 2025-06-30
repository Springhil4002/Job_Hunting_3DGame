Texture2D skyTexture : register(t0);
SamplerState samp    : register(s0);

struct PS_IN
{
    float4 svpos : SV_POSITION;
    float2 uv    : TEXCOORD;
};

float4 PS_Main(PS_IN pin) : SV_TARGET
{
    //return float4(pin.uv.x, pin.uv.y, 1.0 - pin.uv.x, 1.0f);
    return skyTexture.Sample(samp, pin.uv);
}