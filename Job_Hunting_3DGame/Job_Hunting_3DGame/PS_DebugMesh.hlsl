struct PS_IN
{
    float4 svpos : SV_POSITION; // ïœä∑Ç≥ÇÍÇΩç¿ïW
    float4 color : COLOR; // ïœä∑Ç≥ÇÍÇΩêF
    float2 uv : TEXCOORD; // UV  
    float alpha : ALPHA;  // ìßñæìx
};

float4 PS_Main(PS_IN pin) : SV_TARGET
{
    float3 color = pin.color.rgb * pin.alpha;
    return float4(color, pin.alpha);
}