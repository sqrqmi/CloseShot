
// 頂点シェーダーから出力するデータ
struct VSOutput
{
    float4 Pos : SV_Position;   // 頂点の座標(射影座標系)
};
 
//========================================
// 頂点シェーダー
//========================================
VSOutput VS(float4 pos : POSITION)
{
    VSOutput Out;
    // 頂点座標を、何も加工せずそのまま出力
    Out.Pos = pos;
    return Out;
}
 
//========================================
// ピクセルシェーダー
//========================================
float4 PS(VSOutput In) : SV_Target0
{
    // 緑色にする
    return float4(0, 1, 0, 1);
}