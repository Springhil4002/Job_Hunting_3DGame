TextureCube skyTexture : register(t0);
SamplerState samp : register(s0);

struct PS_IN
{
    float4 svpos : SV_POSITION;
    float3 dir : TEXCOORD;
};

float4 PS_Main(PS_IN pin) : SV_TARGET
{
    return skyTexture.Sample(samp, normalize(pin.dir));
}