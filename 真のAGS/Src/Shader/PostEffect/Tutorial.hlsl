#include "../Common/Pixel/PixelShader2DHeader.hlsli"

// 定数バッファ：スロット4番目(b4)
cbuffer cbParam : register(b4)
{
    float4 g_color; // x:intensity, y:time, z:chroma, w:seed
}

// 単純な疑似乱数（UVベース）
float random(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}

float4 main(PS_INPUT PSInput) : SV_TARGET
{
    float2 uv = PSInput.uv;
    float intensity = saturate(g_color.x);
    float time = g_color.y;
    float chroma = g_color.z;
    float seed = g_color.w;

    // 元画像（ヘッダで定義されたテクスチャ/サンプラを使用）
    float4 base = tex.Sample(texSampler, uv);

    // 横バンドごとの小さなシフト
    const float BAND_COUNT = 24.0;
    float bandIdx = floor(uv.y * BAND_COUNT);
    float bandRand = random(float2(bandIdx, floor(time * 5.0 + seed)));
    float bandShift = (bandRand - 0.5) * 0.2 * intensity;
    float2 uv_band = uv + float2(bandShift, 0.0);

    // 時折のバースト
    float burstProb = random(float2(floor(time * 2.0), 0.0));
    float burst = step(0.92, burstProb);
    float burstShift = (random(float2(time * 12.34, bandIdx)) - 0.5) * 0.4 * intensity * burst;
    uv_band.x += burstShift;

    // 小さな波
    float wave = (sin(uv.y * 60.0 + time * 1.2) * 0.5) * 0.03 * intensity;
    uv_band.x += wave;

    // クロマ分離
    float2 chromaOff = float2(chroma * 0.0025, 0.0) * intensity;
    float r = tex.Sample(texSampler, uv_band + chromaOff).r;
    float g = tex.Sample(texSampler, uv_band).g;
    float b = tex.Sample(texSampler, uv_band - chromaOff).b;
    float3 col = float3(r, g, b);

    // 走査線
    float scan = sin(uv.y * 800.0 + time * 40.0) * 0.5 + 0.5;
    float scanDark = lerp(0.0, 0.12, intensity) * (1.0 - scan);
    col *= (1.0 - scanDark);

    // 粒子ノイズ
    float grain = (random(uv * 1000.0 + time * 10.0) - 0.5) * 0.06 * intensity;
    col += grain;

    return float4(col, base.a);
}