Texture2D dmap : register(t0);
Texture2D spec : register(t1);
Texture2D nmap : register(t2);
Texture2D amap : register(t3);

SamplerState splr : register(s0);

struct VSOut
{
    float2 tc : Tex2D;
    float3 ViewPos : ViewPos;
    float3 viewNorm : ViewNormal;
    float3 tan : Tangent; 
    float4 shadowCamPos : ShadowPosition;
    float4 pos : SV_Position;
};
cbuffer OBJCBuf : register(b2)
{
    float4 ambient;  
    bool enNormal;
    float normalMapWeight;
    bool enSpec;
    float4 spec_color;
    float specular_intensity;
    float specular_pow;
    bool hasAmbient;
    bool hasGloss;

};


float3 GenNormal(
    float3 SampleNormal,
    float3 unitNormal,
    float3 tangent
)
{
    float3 normalT = normalize(SampleNormal * 2.0f - 1.0f);
    tangent = normalize(unitNormal - dot(tangent, unitNormal) * unitNormal);
    float3 bitangent = cross(unitNormal, tangent);
    float3x3 TBN = float3x3(tangent, bitangent, unitNormal);
    return mul(normalT, TBN);
};
