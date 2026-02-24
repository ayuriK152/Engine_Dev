#include "Common.hlsl"


float4 ProcessAmbient(float4 ambient, float4 albedo) {
    return ambient * albedo;
}

float4 ProcessDiffuse(float4 diffuse, float4 albedo, float3 lightDir, float3 normal) {
    float4 totalDiffuse;

    float diffuseValue = dot(-lightDir, normal);

    totalDiffuse = diffuse * albedo * diffuseValue * diffuse.a;

    return totalDiffuse;
}

float4 ProcessSpecular(float4 specular, float shiness, float3 lightDir, float3 normal, float3 eyeDir) {
    float4 totalSpecular;

    float3 reflection = reflect(lightDir, normal);
    float specularValue = saturate(dot(reflection, eyeDir));
    specularValue = pow(specularValue, 10);

    totalSpecular = specular * specularValue * shiness;

    return totalSpecular;
}

float4 ProcessEmissive(float4 emissive, float4 albedo) {
    float4 totalEmissive;

    totalEmissive = emissive * albedo;
    totalEmissive.a = 0.0f;

    return totalEmissive;
}

float4 ComputeLight(Material mat, float4 albedo, float3 normal, float3 eyeDir) {
    float4 totalColor = { 0.0f, 0.0f, 0.0f, 0.0f };

    // Global Light Process
    {
        float3 lightDir = normalize(Lights[0].Direction);

        float4 ambient = ProcessAmbient(Lights[0].Ambient * mat.Ambient, albedo);
        float4 diffuse = ProcessDiffuse(Lights[0].Diffuse * mat.Diffuse, albedo, lightDir, normal);
        float4 specular = ProcessSpecular(Lights[0].Specular * mat.Specular, mat.Shiness, lightDir, normal, eyeDir);
        float4 emissive = ProcessEmissive(mat.Emissive, albedo);

        totalColor = ambient + diffuse + specular;
    }

    // // General Light Process
    // {
    //     for (int i = 1; i < gNumLights; i++)
    //     {
    //         float4 diffuse = ProcessDiffuse(Lights[i].Diffuse * mat.Diffuse, albedo, Lights[i].Direction, normal);
    //         float4 specular = ProcessSpecular(Lights[i].Specular * mat.Specular, mat.Shiness, Lights[i].Direction, normal, eyeDir);
        
    //         totalColor += diffuse;
    //     }
    // }

    float4 emissive = ProcessEmissive(mat.Emissive, albedo);
    totalColor += emissive;

    return totalColor;
}
