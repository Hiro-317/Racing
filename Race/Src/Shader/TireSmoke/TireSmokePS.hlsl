// Load時に白煙のビルボード画像を生成する。色にはアルファ値を事前乗算しない。
struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 diffuse : COLOR0;
    float2 uv : TEXCOORD0;
    float2 suv : TEXCOORD1;
};

float Hash(float2 p)
{
    return frac(sin(dot(p, float2(127.1f, 311.7f))) * 43758.5453f);
}

float Noise(float2 p)
{
    float2 cell = floor(p);
    float2 f = frac(p);
    f = f * f * (3.0f - 2.0f * f);
    return lerp(lerp(Hash(cell), Hash(cell + float2(1, 0)), f.x),
                lerp(Hash(cell + float2(0, 1)), Hash(cell + float2(1, 1)), f.x), f.y);
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float2 p = input.uv * 2.0f - 1.0f;
    float cloud = Noise(input.uv * 5.0f) * 0.6f
                + Noise(input.uv * 11.0f + 7.0f) * 0.3f
                + Noise(input.uv * 23.0f) * 0.1f;
    float radius = length(p) + (cloud - 0.5f) * 0.18f;
    float alpha = (1.0f - smoothstep(0.15f, 0.95f, radius))
                * lerp(0.4f, 1.0f, cloud);
    float shade = lerp(0.82f, 1.0f, cloud);
    return float4(shade, shade, shade, saturate(alpha));
}
