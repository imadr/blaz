static float4 o_color;
static float3 v_color;

struct SPIRV_Cross_Input
{
    float3 v_color : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 o_color : SV_Target0;
};

void frag_main()
{
    o_color = float4(v_color, 1.0f);
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    v_color = stage_input.v_color;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.o_color = o_color;
    return stage_output;
}
