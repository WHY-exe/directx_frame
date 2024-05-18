Texture2D tex;
SamplerState splr;

cbuffer Kernel
{
    uint nTabs;
    float cofficients[15];
};

cbuffer Control
{
    bool horizontal;
};


float4 main(float2 uv : Texcoord) : SV_Target
{
    float width, height;
    tex.GetDimensions(width, height);
    float accAlpha = 0.0f;
    float3 maxColor = float3(0.0f, 0.0f, 0.0f);
    const float dx = horizontal ? 1.0f / width : 0.0f,
                dy = horizontal ? 0.0f : 1.0f / height;
    const int r = nTabs / 2;
    [unroll(77)]
    for (int i = -r; i <= r; i++)
    {
        const float2 tc = uv + float2(dx * i, dy * i);
        const float4 s = tex.Sample(splr, tc).rgba;
        maxColor = max(s.rgb, maxColor);
        accAlpha += s.a * cofficients[i + r];
    }
    return float4(maxColor, accAlpha);
}