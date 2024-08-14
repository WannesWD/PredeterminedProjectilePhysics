//----------------------------------------------------------------------------------------------------------
//  Input/Output Structs
//----------------------------------------------------------------------------------------------------------
SamplerState gSampleState : SampleState;

float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorldMatrix   : WorldMatrix;

Texture2D gDiffuseMap   : DiffuseMap;
Texture2D gNormalMap    : NormalMap;
Texture2D gSpecularMap  : SpecularMap;
Texture2D gGlossMap     : GlossMap;

float3 gLightDirection  : LightDirection;
float3 gLightColor      : LightColor;
float3 gCameraPosition  : CameraPosition;

float gLightIntensity   : LightIntensity;
float gShininess        : Shininess;

const float PI = 3.14159f;

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Color : COLOR;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 Position      : SV_POSITION;
    float4 WorldPosition : TEXCOORD0;
    float2 UV            : TEXCOORD1;
    float3 Normal        : NORMAL;
    float3 Tangent       : TANGENT;
};

//----------------------------------------------------------------------------------------------------------
//  Vertex Shader
//----------------------------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.WorldPosition = mul(float4(input.Position,1.f), gWorldMatrix);
    output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
    output.UV = input.UV;
    output.Normal = mul(input.Normal, (float3x3)gWorldMatrix);
    output.Tangent = mul(input.tangent, (float3x3)gWorldMatrix);
    return output;
}

//----------------------------------------------------------------------------------------------------------
//  Pixel Shader
//----------------------------------------------------------------------------------------------------------
float3 Phong(float ks, float exp, float3 l, float3 v, float3 n)
{
    float3 reflected = reflect(l, n);
    float cosAngle = dot(reflected, v);
    return ks * pow(max(0.f, cosAngle), exp) * gLightColor;
}
float3 Lambert(float kd, float3 cd)
{
    return (kd * cd) / PI;
}
float3 ShadePixel(VS_OUTPUT input)
{
    float3 invViewDirection = normalize(gCameraPosition - input.WorldPosition.xyz);
    float3 invLightDirection = -gLightDirection;
    float3 binomial = cross(input.Normal, input.Tangent);
    float3x3 tangentSpaceAxis = float3x3(input.Tangent, binomial, input.Normal);
    float3 sampledNormal = gNormalMap.Sample(gSampleState, input.UV).rgb;
    float3 mappedNormal = float3(2.f * sampledNormal.x - 1.f,
                                 2.f * sampledNormal.y - 1.f,
                                 2.f * sampledNormal.z - 1.f);
    mappedNormal = mul(mappedNormal, tangentSpaceAxis);
    
    float cosAngle =  dot(invLightDirection, mappedNormal);
    float3 observedArea = float3(cosAngle, cosAngle, cosAngle);
    
    float3 specularSample = gSpecularMap.Sample(gSampleState, input.UV).rgb;
    float sampleSpecular = (specularSample.x + specularSample.y + specularSample.y) / 3.f;
    float sampleGloss = gGlossMap.Sample(gSampleState, input.UV).x;
    
    float3 specular = Phong(sampleSpecular, sampleGloss * gShininess, invLightDirection, invViewDirection, input.Normal);
    float3 diffuse = Lambert(gLightIntensity, gDiffuseMap.Sample(gSampleState, input.UV).rgb);
    
    return saturate((saturate(specular) + saturate(diffuse)) * observedArea);
} 

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    return float4(ShadePixel(input),1.f);
}

float4 PSPC(VS_OUTPUT input) : SV_TARGET
{
    return gDiffuseMap.Sample(gSampleState, input.UV);
}
//----------------------------------------------------------------------------------------------------------
//  BlendStates
//----------------------------------------------------------------------------------------------------------
BlendState gBlendState
{
    BlendEnable[0] = true;
    SrcBlend = src_alpha;
    DestBlend = inv_src_alpha;
    BlendOp = add;
    SrcBlendAlpha = zero;
    DestBlendAlpha = zero;
    BlendOpAlpha = add;
    RenderTargetWriteMask[0] = 0x0F;
};
//----------------------------------------------------------------------------------------------------------
//  BlendStates
//----------------------------------------------------------------------------------------------------------
DepthStencilState gDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = less;
    StencilEnable = false;
    
    StencilReadMask = 0x0F;
    StencilWriteMask = 0x0F;
    
    FrontFaceStencilFunc = always;
    BackFaceStencilFunc = always;
    
    FrontFaceStencilDepthFail = keep;
    BackFaceStencilDepthFail = keep;

    FrontFaceStencilPass = keep;
    BackFaceStencilPass = keep;
    
    FrontFaceStencilFail = keep;
    BackFaceStencilFail = keep;
};

DepthStencilState gDepthStencilStateDefault
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less;
    StencilEnable = false;
    
    StencilReadMask = 0x0F;
    StencilWriteMask = 0x0F;
    
    FrontFaceStencilFunc = always;
    BackFaceStencilFunc = always;
    
    FrontFaceStencilDepthFail = keep;
    BackFaceStencilDepthFail = keep;

    FrontFaceStencilPass = keep;
    BackFaceStencilPass = keep;
    
    FrontFaceStencilFail = keep;
    BackFaceStencilFail = keep;
};
//----------------------------------------------------------------------------------------------------------
//  Techniques
//----------------------------------------------------------------------------------------------------------
technique11 DefaultTechnique
{
    pass P0
    {
        SetDepthStencilState(gDepthStencilStateDefault, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0,PS()));
    }
}

technique11 SimpleTechnique
{
    pass P0
    {
        SetDepthStencilState(gDepthStencilStateDefault, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PSPC()));
    }
}

technique11 PartialCoverageTechnique
{
    pass P0
    {
        SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PSPC()));
    }
}

