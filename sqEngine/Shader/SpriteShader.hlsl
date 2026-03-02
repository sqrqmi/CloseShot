cbuffer cbTransform : register(b0)
{
    matrix Transform;
}

// 頂点シェーダーに入力されるデータ
struct VSInput
{
    float3 Pos : POSITION;
    float4 Color : COLOR;
};

// 頂点シェーダーから出力するデータ
struct VSOutput
{
    float4 Pos : SV_POSITION;   // 頂点の座標(射影座標系)
    float4 Color : COLOR;       // 頂点の色
};

// ピクセルシェーダーに入力されるデータ
struct PSInput
{
    float4 Pos : SV_POSITION;   // 頂点の座標(射影座標系)
    float4 Color : COLOR;       // 頂点の色
};
 
//========================================
// 頂点シェーダー
//========================================
VSOutput VS(VSInput input_)
{
    VSOutput Out;
    
    float4 pos = float4(input_.Pos, 1.0f);
    Out.Pos = pos;
    Out.Color = input_.Color;
    
    return Out;
}
 
//========================================
// ピクセルシェーダー
//========================================
float4 PS(PSInput input_) : SV_Target0
{
    return input_.Color;
}