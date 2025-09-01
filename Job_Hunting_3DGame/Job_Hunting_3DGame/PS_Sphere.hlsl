struct PS_IN
{
    float4 svpos : SV_POSITION; // ïœä∑Ç≥ÇÍÇΩç¿ïW
    float4 color : COLOR; // ïœä∑Ç≥ÇÍÇΩêF
    float2 uv : TEXCOORD; // UV  
};

float4 PS_Main(PS_IN pin) : SV_TARGET
{
    float alpha = 0.5f;
    float3 color = pin.color.rgb * alpha;
    return float4(color, alpha);
}