#include "PSShadow.hlsli"
struct LightComponent
{
    float3 Diffuse : Diffuse;
    float3 Specular : Specular;
    float3 Ambient : Ambient;
};
struct LightAttri
{
    float3 lightViewPos;
    float3 DiffuseColor;
    float3 AmbientColor;
    float3 DiffuseIntensity;
    float  SpecularPow;
    bool   enableAtt;
    bool   enableSpecMap;
    float  SpecularIntensity;
    float  AttConst;
    float  AttLinear;
    float  AttQuad;
};
#define MAX_LIGHT_NUM (unsigned int)16
cbuffer pointLightCBuf : register(b0)
{    
    uint   cur_light_num;
    float3 pLightViewPos[MAX_LIGHT_NUM];
    float3 pAmbientColor[MAX_LIGHT_NUM];
    float3 pDiffuseColor[MAX_LIGHT_NUM];
    float  pDiffuseIntensity[MAX_LIGHT_NUM];
    float  pAttConst[MAX_LIGHT_NUM];
    float  pAttLinear[MAX_LIGHT_NUM];
    float  pAttQuad[MAX_LIGHT_NUM];
    bool   pEnable[MAX_LIGHT_NUM];    
};


cbuffer GlobalLightCBuf : register(b1)
{
    float3 gLightViewPos;
    float3 gAmbientColor;
    float3 gDiffuseColor;
    float gDiffuseIntensity;
    bool gEnable;
};

LightComponent GetLight(    
    float3 vtxViewPos,
    float3 VertexNormal,
    float3 lightViewPos,
    float3 AmbientColor,
    float3 DiffuseColor,
    float3 DiffuseIntensity,
    float SpecularIntensity,
    float SpecularPow,
    bool enableAtt = false,
    float AttConst = 1.0f,
    float AttLinear = 0.0f,
    float AttQuad = 0.0f
)
{
    LightComponent lc;
    // vector of the obj to the light source
    const float3 vToLight = lightViewPos - vtxViewPos;
	// distance between the obj to the light source
    const float distToLight = length(vToLight);
	// Norm vector of the obj to the light source
    const float3 dirToLight = vToLight / distToLight;
	// the vector take part in the dot product caculation is the unity vector
	// so the result is the cos(theta) between the two vector
    float3 Diffuse = DiffuseColor * DiffuseIntensity * max(0.0f, dot(dirToLight, VertexNormal));
    
    // traditonal phong lighting
    // the reflection vector
    //const float3 r = 2.0f * VertexNormal * dot(vToLight, VertexNormal) - vToLight;
    //float3 Specular = Diffuse * SpecularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(vtxViewPos))), SpecularPow);
    
    // blinn phong lighting
    const float3 h = vToLight - vtxViewPos;
    float3 Specular = Diffuse * SpecularIntensity * pow(max(0.0f, dot(normalize(VertexNormal), normalize(h))), SpecularPow);
    if (enableAtt)
    {
        const float att = 1.0f / (AttConst + AttLinear * distToLight + AttQuad * (distToLight * distToLight));
        Diffuse *= att;
        Specular *= att;
    }
    lc.Diffuse = Diffuse;
    lc.Specular = Specular;
    lc.Ambient = AmbientColor;
    return lc;
};
LightAttri GetLightAttriAt(int index, float SpecularPow, float SpecularIntensity)
{                                         
    LightAttri la;                    
    la.lightViewPos      =   pLightViewPos[index];    
    la.AmbientColor      =   pAmbientColor[index];
    la.DiffuseColor      =   pDiffuseColor[index];
    la.DiffuseIntensity  =   pDiffuseIntensity[index];
    la.SpecularPow       =   SpecularPow;    
    la.SpecularIntensity =   SpecularIntensity;
    la.enableAtt         =   true;
    la.AttConst          =   pAttConst[index];
    la.AttLinear         =   pAttLinear[index];
    la.AttQuad           =   pAttQuad[index];
    return la;
};

LightComponent GetLight(LightAttri la, float3 VertexPos, float3 VertexNormal)
{
    if (la.enableAtt)
        return 
            GetLight(
                VertexPos, VertexNormal,la.lightViewPos, la.AmbientColor, la.DiffuseColor, 
                la.DiffuseIntensity, la.SpecularIntensity, la.SpecularPow,
                la.enableAtt, la.AttConst, la.AttLinear, la.AttQuad);
    else
        return 
            GetLight(
                 VertexPos, VertexNormal, la.lightViewPos, la.AmbientColor,
                 la.DiffuseColor, la.DiffuseIntensity, la.SpecularIntensity, la.SpecularPow);
};
LightComponent SetLightingPixelResult(float SpecularPow, float SpecularIntensity, float3 vtxViewPos, float3 VertexNormal, float4 shadowpos, float alpha = 1.0f)
{
    LightComponent result_in;
    result_in.Diffuse = float3(0.0f, 0.0f, 0.0f);   
    result_in.Specular = float3(0.0f, 0.0f, 0.0f);
    result_in.Ambient = float3(0.3f, 0.3f, 0.3f);
 
    for (uint i = 0; i < cur_light_num; i++)
    {
        if (pEnable[i])
        {
            if (i == 0)
            {
                float shadow_weight = PCFShadowed(shadowpos);
                if (shadow_weight)
                {
                    LightComponent light = GetLight(GetLightAttriAt(i, SpecularPow, SpecularIntensity), vtxViewPos, VertexNormal);
                    result_in.Ambient  += light.Ambient  * shadow_weight;
                    result_in.Diffuse  += light.Diffuse  * shadow_weight;
                    result_in.Specular += light.Specular * shadow_weight;
                }
            }
            else
            {
                LightComponent light = GetLight(GetLightAttriAt(i, SpecularPow, SpecularIntensity), vtxViewPos, VertexNormal);
                result_in.Ambient += light.Ambient;
                result_in.Diffuse += light.Diffuse;
                result_in.Specular += light.Specular;
            }
        }
    }
    return result_in;
};