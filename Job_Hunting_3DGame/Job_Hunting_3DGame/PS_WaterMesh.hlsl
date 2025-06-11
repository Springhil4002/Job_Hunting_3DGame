cbuffer Transform : register(b0)
{
    float4x4 world;     // ���[���h�s��
    float4x4 view;      // �r���[�s��
    float4x4 proj;      // ���e�s��
    float time;         // ����
    float3 cameraPos;   // �J�����ʒu
    float pad1;         // �p�f�B���O
}

cbuffer LightParams : register(b2)
{
    float3 lightDir;    // ���C�g�̕���
    float pad0;         // �p�f�B���O
    float4 lightColor;  // ���C�g�̐F
}

struct VSOutput
{
    float4 svpos    : SV_POSITION;  // ���_�V�F�[�_�[���痈�����W
    float4 color    : COLOR;        // ���_�V�F�[�_�[���痈���F
    float2 uv       : TEXCOORD;     // ���_�V�F�[�_�[���炫��UV
    float3 normal   : NORMAL;       // ���_�V�F�[�_�[���炫���@��
    float3 worldPos : POSITION1;    // ���_�V�F�[�_�[���痈�����[���h��ԍ��W(�X�y�L�����v�Z�p)
};

Texture2D tex : register(t0);       // �e�N�X�`��
SamplerState smp : register(s0);    // �T���v���[

float4 PS_Main(VSOutput pin) : SV_TARGET
{
    float3 N = normalize(pin.normal); // �@��(���[���h���)
    float3 L = normalize(-lightDir); // ���C�g����(���[���h���)
    float3 V = normalize(cameraPos - pin.worldPos); // �����x�N�g��
    float3 R = reflect(-L, N); // ���˃x�N�g��
    
    // �n�[�t�����o�[�g
    float NdotL = dot(N, L);
    NdotL = saturate(NdotL * 0.5f + 0.5f); // ���邳�̍Œ�l��0.5
    
    // ���ʔ���
    float specPower = 64.0f;
    float specIntensity = 0.8f;
    float spec = pow(max(dot(R, V), 0.0f), specPower);
    
    // Fresnel���ʁiSchlick�ߎ��j
    float fresnelPower = 5.0f;
    float baseReflectivity = 0.02f; // ���̕\���̂���
    float fresnel = baseReflectivity + 
                    (1.0 - baseReflectivity) * 
                    pow(1.0 - saturate(dot(N, V)), 
                    fresnelPower);
    // �e�N�X�`���J���[
    float4 texColor = tex.Sample(smp, pin.uv);
    
    // ���C�e�B���O
    float4 ambient = 0.1f * texColor;
    float4 diffuse = texColor * pin.color * lightColor * NdotL;
    float4 specular = fresnel * specIntensity * spec * lightColor;
    
    return ambient + diffuse + specular;
}