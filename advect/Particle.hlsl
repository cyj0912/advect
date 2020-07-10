struct VSIn
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 instPosition : INSTPOSITION;
};

struct VStoPS
{
    float4 position : SV_Position;
    float3 normal : Normal;
};

float4x4 modelMat;
float4x4 vpMat;

VStoPS vertexShader(VSIn input)
{
    VStoPS output;
    float4 worldPos = mul(modelMat, float4(input.position, 1));
    // worldPos /= worldPos.w;
    worldPos += float4(input.instPosition, 0, 0);
    output.position = mul(vpMat, worldPos);
    output.normal = input.normal;
    return output;
}

float4 pixelShader(VStoPS input) : SV_Target0 { return float4(1, 1, 1, 1); }
