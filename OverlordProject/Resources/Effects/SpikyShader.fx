//************
// VARIABLES *
//************
float4x4 m_MatrixWorldViewProj : WORLDVIEWPROJECTION;
float4x4 m_MatrixWorld : WORLD;
float3 m_LightDirection : DIRECTION
<
	string Object="TargetLight";
> = float3(0.577f, -0.577f, 0.577f);

float3 gColorDiffuse : COLOR = float3(1,1,1);

float gSpikeLength
<
	string UIWidget="Slider";
	float UIMin=0.0f;
	float UIMax=1.f;
	float UIStep=0.0001f;
> = 0.2f;

RasterizerState FrontCulling 
{ 
	CullMode = FRONT; 
};

//**********
// STRUCTS *
//**********
struct VS_DATA
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
};

struct GS_DATA
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
};

//****************
// VERTEX SHADER *
//****************
VS_DATA MainVS(VS_DATA vsData)
{
    return vsData;
}

//******************
// GEOMETRY SHADER *
//******************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float3 normal)
{
	//Step 1. Create a GS_DATA object
	GS_DATA vertex = (GS_DATA)0;
	//Step 2. Transform the position using the WVP Matrix and assign it to (GS_DATA object).Position (Keep in mind: float3 -> float4)
	vertex.Position = mul(float4(pos, 1.0f), m_MatrixWorldViewProj);
	//Step 3. Transform the normal using the World Matrix and assign it to (GS_DATA object).Normal (Only Rotation, No translation!)
	vertex.Normal = mul(normal,(float3x3) m_MatrixWorld);
	//Step 4. Append (GS_DATA object) to the TriangleStream parameter (TriangleStream::Append(...))
	triStream.Append(vertex);
}

[maxvertexcount(9)]
void SpikeGenerator(triangle VS_DATA v[3], inout TriangleStream<GS_DATA> triStream)
{
	//Use these variable names
    float3 CENTER_POINT, topN, leftN, rightN, spikeNormal;

	//Step 1. Calculate CENTER_POINT
	CENTER_POINT = (v[0].Position + v[1].Position + v[2].Position) / 3;
	//Step 2. Calculate Face Normal (Original Triangle)
	
	spikeNormal = (v[0].Normal + v[1].Normal + v[2].Normal) / 3.0f;
	
	//Step 3. Offset CENTER_POINT (use gSpikeLength)
	
	CENTER_POINT += spikeNormal * gSpikeLength;
	
	//Step 4 + 5. Calculate Individual Face Normals (Cross Product of Face Edges) & Create Vertices for every face

	//Face 1
	float3 faceNormal1 = normalize(cross(v[1].Position - v[0].Position, CENTER_POINT - v[0].Position));
	
	CreateVertex(triStream, v[0].Position, faceNormal1);
	CreateVertex(triStream, v[1].Position, faceNormal1);
	CreateVertex(triStream, CENTER_POINT, faceNormal1);
	
	triStream.RestartStrip();

	//Face 2
	float3 faceNormal2 = normalize(cross(v[2].Position - v[1].Position, CENTER_POINT - v[1].Position));
	
	CreateVertex(triStream, v[1].Position, faceNormal2);
	CreateVertex(triStream, v[2].Position, faceNormal2);
	CreateVertex(triStream, CENTER_POINT, faceNormal2);
	
	triStream.RestartStrip();

	//Face 3
	float3 faceNormal3 = normalize(cross(v[0].Position - v[2].Position, CENTER_POINT - v[2].Position));
	
	CreateVertex(triStream, v[2].Position, faceNormal3);
	CreateVertex(triStream, v[0].Position, faceNormal3);
	CreateVertex(triStream, CENTER_POINT, faceNormal3);
	
	triStream.RestartStrip();
}

//***************
// PIXEL SHADER *
//***************
float4 MainPS(GS_DATA input) : SV_TARGET
{
    input.Normal=normalize(input.Normal);
	float diffuseStrength = max(dot(normalize(m_LightDirection),-input.Normal),0.2f); 
	return float4(gColorDiffuse*diffuseStrength,1.0f);
}

//*************
// TECHNIQUES *
//*************
technique10 Default //FXComposer >> Rename to "technique10 Default"
{
    pass p0
    {
        SetRasterizerState(FrontCulling);
        SetVertexShader(CompileShader(vs_4_0, MainVS()));
        SetGeometryShader(CompileShader(gs_4_0, SpikeGenerator())); 
        SetPixelShader(CompileShader(ps_4_0, MainPS()));
    }
}