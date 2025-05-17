struct VS_Output
{
    float4 svpos : SV_POSITION;     // ���_�V�F�[�_�[���炫�����W
    float4 color : COLOR;           // ���_�V�F�[�_�[���炫���F
    float2 uv : TEXCOORD;           // ���_�V�F�[�_�[���炫��UV
};

float4 pixel(VS_Output input) : SV_TARGET
{
    return float4(input.uv.xy, 1, 1);
}
