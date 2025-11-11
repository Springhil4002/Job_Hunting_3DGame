cbuffer Transform : register(b0)
{
    float4x4 world;
    float4x4 view; 
    float4x4 proj; 
    float time; 
    float3 cameraPos; 
    float pad1;
};

cbuffer LightParams : register(b2)
{
    float3 lightDir;    
    float envStrength; 
    float4 lightColor; 
};

cbuffer WaterParams : register(b3)
{
    float2 fbmScale;
    float fbmGain;
    float fbmLacunarity;
    int fbmOctaves;

    float normalPerturb;
    float foamThreshold;
    float foamIntensity;
    float4 shallowColor;

    float4 deepColor;
};

struct PS_IN
{
    float4 svpos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL; 
    float3 worldPos : POSITION1; 
};

Texture2D tex : register(t0); 
SamplerState smp : register(s0); 
TextureCube skyCube : register(t3); 

float hash(float2 p)
{
    p = frac(p * float2(123.34, 456.21));
    p += dot(p, p + 78.233);
    return frac(p.x * p.y);
}

float noise(float2 p)
{
    float2 i = floor(p);
    float2 f = frac(p);
    float a = hash(i);
    float b = hash(i + float2(1, 0));
    float c = hash(i + float2(0, 1));
    float d = hash(i + float2(1, 1));
    float2 u = f * f * (3.0 - 2.0 * f);
    return lerp(lerp(a, b, u.x), lerp(c, d, u.x), u.y);
}

float fbm(float2 p, int octaves, float lacunarity, float gain)
{
    float value = 0.0;
    float amp = 0.5;
    float2 q = p;
    [unroll]
    for (int i = 0; i < 10; ++i)
    {
        if (i >= octaves)
            break;
        value += amp * noise(q);
        q = q * lacunarity + float2(37.1, 17.7);
        amp *= gain;
    }
    return value;
}

float4 PS_Main(PS_IN pin) : SV_TARGET
{
    float3 N = normalize(pin.normal);
    float3 L = normalize(-lightDir);
    float3 V = normalize(cameraPos - pin.worldPos);
    
    float NdotL = saturate(dot(N, L));

    float2 st = pin.worldPos.xz * fbmScale + time * 0.05;
    int oct = max(1, fbmOctaves);
    float n0 = fbm(st, oct, fbmLacunarity, fbmGain);
    float n1 = fbm(st * 2.31 + 11.7, oct, fbmLacunarity, fbmGain);
    float nH = fbm(st * 4.71 + 31.1, oct, fbmLacunarity, fbmGain); 

    float3 up = float3(0, 1, 0);
    float3 T = normalize(cross(up, N));
    T = (all(T == 0)) ? float3(1, 0, 0) : T;
    float3 B = normalize(cross(N, T));
    
    float3 perturb = normalize(T * (n0 - 0.5) + B * (n1 - 0.5));
    N = normalize(N + perturb * normalPerturb);
    
    float NdotV = saturate(dot(N, V));
    const float F0 = 0.02;     
    float fresnel = F0 + (1.0 - F0) * pow(1.0 - NdotV, 5.0);
    fresnel = max(fresnel, 0.04);
     
    float3 Rl = reflect(-L, N);
    float3 Rv = reflect(-V, N);
        
    float spec = pow(saturate(dot(Rl, V)), 240.0);
    float4 specular = fresnel * spec * lightColor;
            
    float slope = 1.0 - dot(N, float3(0, 1, 0));
    float depthFactor = saturate(pow(1.0 - NdotV, 1.5));
    float4 waterBase = lerp(deepColor, shallowColor, depthFactor);
    float3 absorbCoeff = float3(2.0, 1.2, 0.4);
    waterBase.rgb *= exp(-absorbCoeff * depthFactor);
            
    float4 ambient = float4(0.08, 0.10, 0.12, 1.0);
    float4 diffuse = waterBase * (NdotL * 0.6);
           
    float4 envColor = skyCube.Sample(smp, Rv);
    float envMix = saturate(envStrength + fresnel * 0.8);
    envMix = max(envMix, 0.15); 

    float4 lighting = ambient + diffuse + specular;
    float4 baseMix = lerp(lighting, envColor, envMix);
           
    float foam = saturate((slope * 0.6 + (nH - foamThreshold)) * (1.0 / max(1e-3, 1.0 - foamThreshold)));
    foam *= foamIntensity;
    float4 withFoam = lerp(baseMix, float4(1, 1, 1, 1), foam);
      
    float dist = length(cameraPos - pin.worldPos);
    float fogT = saturate((dist - 150.0) / 800.0);
    float4 fogColor = deepColor;
    float4 finalColor = lerp(withFoam, fogColor, fogT);
    
    return finalColor;
};