SamplerComparisonState ssplr : register(s1);
TextureCube smap : register(t4);
static const float zf = 3000.0f;
static const float zn = 0.5f;
static const float c1 = zf / (zf - zn);
static const float c0 = -zn * zf / (zf - zn);

cbuffer ShadowControl : register(b3)
{
    uint pcfLevel;
    float depthBias;
};

float CalculateShadowDepth(const in float4 shadowPos)
{
    // get magnitudes for each basis component
    const float3 m = abs(shadowPos).xyz;
    // get the length in the dominant axis
    // (this correlates with shadow map face and derives comparison depth)
    const float major = max(m.x, max(m.y, m.z));
    // converting from distance in shadow light space to projected depth
    return (c1 * major + c0) / major;
}

float ShadowLoop(const in float4 spos, uniform int pcf_level)
{
    float4 shadow_pos_bias = spos;
    float shadow_weight = 0.0f;
    [unroll]
    for (int x = -pcf_level; x <= pcf_level; x++)
    {
        shadow_pos_bias.x += x;
        [unroll]
        for (int y = -pcf_level; y <= pcf_level; y++)
        {
            shadow_pos_bias.y += y;
            shadow_weight += smap.SampleCmpLevelZero(ssplr, shadow_pos_bias.xyz, CalculateShadowDepth(spos) - depthBias).r;
        }
    }
    return shadow_weight / pow(pcfLevel * 2 + 1, 2);
}


float PCFShadowed(const in float4 shadow_pos)
{    
    float shadowLevel = 0.0f;
    if (CalculateShadowDepth(shadow_pos) > 1.0f || CalculateShadowDepth(shadow_pos) < 0.0f)
    {
        shadowLevel = 1.0f;
    }  
    else
    {
        [unroll]
        for (uint level = 0; level <= 4; level++)
        {
            if (level == pcfLevel)
            {
                shadowLevel = ShadowLoop(shadow_pos, level);
            }
        }  
    }
    return shadowLevel;
}