Texture2D tex;
SamplerState splr;

cbuffer Kernel
{
    int nTabs;
    float4 cofficients[15];
};

cbuffer Control
{
    bool horizontal;
};


float4 main(float2 uv : Texcoord) : SV_Target
{
    float width, height;
    tex.GetDimensions(width, height);
    float4 acc = { 0.0f, 0.0f, 0.0f, 0.0f };
    const float dx = horizontal ? 1.0f / width : 0.0f, 
                dy = horizontal ? 0.0f : 1.0f / height;
    const int r = nTabs / 2;
    [unroll(77)]
    for (int i = -r; i <= r; i++)
    {
        const float2 tc = uv + float2(dx * i, dy * i);
        const float4 s = tex.Sample(splr, tc).rgba;
        acc += s * cofficients[i + r];
    }
    return acc;
}