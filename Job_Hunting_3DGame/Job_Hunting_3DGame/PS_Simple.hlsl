struct VS_Output
{
    float4 svpos : SV_POSITION;     // ���_�V�F�[�_�[���炫�����W
    float4 color : COLOR;           // ���_�V�F�[�_�[���炫���F
    float2 uv    : TEXCOORD;        // ���_�V�F�[�_�[���炫��UV
};

SamplerState smp : register(s0);    // �T���v���[
Texture2D _MainTex : register(t0);  // �e�N�X�`��

float4 pixel(VS_Output input) : SV_TARGET
{
    return _MainTex.Sample(smp, input.uv);
}
