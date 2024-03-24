static float4 gl_Position;
static float3 v_color;
static float3 a_color;
static float3 a_position;

struct SPIRV_Cross_Input
{
    float3 a_position : POSITION;
    float3 a_color : NORMAL;
};

struct SPIRV_Cross_Output
{
    float3 v_color : TEXCOORD0;
    float4 gl_Position : SV_Position;
};

void vert_main()
{
    v_color = a_color;
    gl_Position = float4(a_position, 1.0f);
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    a_color = stage_input.a_color;
    a_position = stage_input.a_position;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output.v_color = v_color;
    return stage_output;
}
