float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float4x4 gWorldViewProj_Light;
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float gShadowMapBias = 0.01f;
float4x4 gBones[70];

Texture2D gDiffuseMap;
Texture2D gShadowMap;

SamplerComparisonState cmpSampler
{
	// sampler state
	Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
	AddressU = MIRROR;
	AddressV = MIRROR;

	// sampler comparison state
	ComparisonFunc = LESS_EQUAL;
};

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 BoneIndices : BLENDINDICES;
	float4 BoneWeights : BLENDWEIGHTS;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 lPos : TEXCOORD1;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState NoCulling
{
	CullMode = NONE;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	//TODO: complete Vertex Shader 
	
	float4 transformedPosition = (float4)0;
	float3 transformedNormal = (float3)0;

	for (int i = 0; i < 4; ++i)
	{
		int boneIndex = (int)input.BoneIndices[i];

		if (boneIndex >= 0)
		{
			float boneWeight = input.BoneWeights[i];

			// Calculate BoneSpace Position & Normal (gBones Matrix)
			float4x4 boneMatrix = gBones[boneIndex];

			float4 bonePos = mul(float4(input.pos, 1.0f), boneMatrix);
			float3 boneNormal = mul(input.normal, (float3x3)boneMatrix);

			//append position and rotation
			transformedPosition += bonePos * boneWeight;
			transformedNormal += boneNormal * boneWeight;
		}
	}

	transformedPosition.w = 1.f;

	output.pos = mul(transformedPosition, gWorldViewProj);

	// Transform the final normal to world space and normalize it
	output.normal = normalize(mul(transformedNormal, (float3x3)gWorld));

	output.texCoord = input.texCoord;

	output.lPos = mul( transformedPosition, gWorldViewProj_Light);
	
	return output;
}

float2 texOffset(int u, int v)
{
	//TODO: return offseted value (our shadow map has the following dimensions: 1280 * 720)
	return float2(u / 1280.f , v / 720.f);
}

float EvaluateShadowMap(float4 lpos)
{
	lpos.xyz /= lpos.w;

	const bool isOutside = lpos.x < -1.f || lpos.x > 1.f || lpos.y < -1.f || lpos.y > 1.f || lpos.z < 0.f || lpos.y > 1.f;

	if (isOutside)
	{
		return 1.f;
	}

	lpos.x = lpos.x / 2.f + 0.5f;
	lpos.y = lpos.y / -2.f + 0.5f;

	lpos.z -= gShadowMapBias;

	float sum = 0;
	float x, y;

	for (y = -1.5; y <= 1.5; y += 1.0)
	{
		for (x = -1.5; x <= 1.5; x += 1.0)
		{
			sum += gShadowMap.SampleCmpLevelZero(cmpSampler, lpos.xy + texOffset(x, y), lpos.z);
		}
	}

	float shadowFactor = sum / 16.0;

	if (shadowFactor < lpos.z) return 0.4f;

	return 1.f;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float shadowValue = EvaluateShadowMap(input.lPos);

	float4 diffuseColor = gDiffuseMap.Sample( samLinear,input.texCoord );
	float3 color_rgb= diffuseColor.rgb;
	float color_a = diffuseColor.a;
	
	//HalfLambert Diffuse :)
	float diffuseStrength = dot(input.normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	color_rgb = color_rgb * diffuseStrength;

	return float4( color_rgb * shadowValue, color_a );
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
    pass P0
    {
		SetRasterizerState(NoCulling);
		SetDepthStencilState(EnableDepth, 0);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

