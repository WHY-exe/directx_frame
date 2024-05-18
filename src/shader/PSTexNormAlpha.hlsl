#include "lighting.hlsli"
#include "PSBase.hlsli"
float4 main(VSOut vso) : SV_Target
{
    float3 ViewNormal = vso.viewNorm;
     [flatten]
    if (enNormal)
    {
        const float3 mappedNorm = GenNormal(nmap.Sample(splr, vso.tc).xyz, normalize(vso.viewNorm), vso.tan);
        ViewNormal = lerp(ViewNormal, mappedNorm, normalMapWeight);
    }
    if (dot(ViewNormal, vso.ViewPos.xyz) > 0.0f)
        ViewNormal = -ViewNormal;
    LightComponent result = { { 0, 0, 0 }, { 0, 0, 0 }, { 0.3, 0.3, 0.3 }};
    float4 matDiffuse = dmap.Sample(splr, vso.tc);
    float4 matAmbient = matDiffuse;
    if (hasAmbient)
        matAmbient = amap.Sample(splr, vso.tc);
    float4 matSpec = spec_color;
    result = SetLightingPixelResult(specular_pow, specular_intensity, vso.ViewPos, ViewNormal,vso.shadowCamPos);
    return float4(
        saturate(result.Diffuse) * matDiffuse.rgb +
        saturate(result.Ambient) * matAmbient.rgb +
        saturate(result.Specular) * matSpec.rgb, 
        max(matDiffuse.a, matAmbient.a));
}