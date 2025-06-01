struct VSOutput
{
    float4 svpos : SV_POSITION; // ���_�V�F�[�_�[���痈�����W
    float4 color : COLOR;       // ���_�V�F�[�_�[���痈���F
    float2 uv    : TEXCOORD;    // ���_�V�F�[�_�[���炫��UV
};

Texture2D tex : register(t0);       // �e�N�X�`��
SamplerState smp : register(s0);    // �T���v���[

float4 PS_Main(VSOutput pin) : SV_TARGET
{
    float4 texColor = tex.Sample(smp, pin.uv);
    return texColor * pin.color;    // ���_�J���[�ƃe�N�X�`����Z
}