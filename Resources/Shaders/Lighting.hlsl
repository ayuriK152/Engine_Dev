#include "Common.hlsl"


float4 ProcessAmbient(float4 ambient, float4 albedo) {
    return ambient * albedo;
}

float4 ProcessDiffuse(float4 diffuse, float4 albedo, float3 lightDir, float3 normal) {
    float4 totalDiffuse;

    float diffuseValue = dot(lightDir, normal);

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

float DistributionGGX(float NdotH, float roughness) {
    float a2 = pow(roughness, 4.0);
    float NdotH2 = pow(max(NdotH, 0.0), 2.0);
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = acos(-1) * denom * denom;
    return a2 / denom;
}

float GeometrySmith(float NdotV, float NdotL, float roughness) {
    //float k = roughness * roughness / 2.0f;
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;

    float GV = max(NdotV, 0.0) / (NdotV * (1.0 - k) + k);
    float GL = max(NdotL, 0.0) / (NdotL * (1.0 - k) + k);
    
    return GV * GL;
}

float3 FresnelSchlick(float3 F0, float NdotV) {
    return F0 + (1.0 - F0) * pow(1.0 - NdotV, 5.0);
}

// Legacy Blinh Phong
float4 ComputeLight(Material mat, float4 albedo, float3 normal, float3 eyeDir) {
    float4 totalColor = { 0.0, 0.0, 0.0, 0.0 };

    // Global Light Process
    {
        float3 lightDir = normalize(-Lights[0].Direction);

        float4 ambient = ProcessAmbient(Lights[0].Ambient * mat.Ambient, albedo);
        float4 diffuse = ProcessDiffuse(Lights[0].Diffuse * mat.Diffuse, albedo, lightDir, normal);
        float4 specular = ProcessSpecular(Lights[0].Specular * mat.Specular, mat.Metallic, lightDir, normal, eyeDir);
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

float4 BRDFLighting(Material mat, float4 albedo, float3 N, float3 V) {
    float4 color = { 0.0, 0.0, 0.0, albedo.a };
    float3 F0 = float3(0.04, 0.04, 0.04);       // temp

    float NdotV = dot(N, V);

    // Global Light Process, refactoring later
    {
        float3 L = normalize(-Lights[0].Direction);
        float3 H = normalize(V + L);

        float NdotL = dot(N, L);
        float NdotH = dot(N, H);

        float D = DistributionGGX(NdotH, mat.Roughness);
        float G = GeometrySmith(NdotV, NdotL, mat.Roughness);
        float3 F = FresnelSchlick(F0, NdotV);

        float3 nom = D * G * F;
        float denom = 4 * max(dot(L, N), 0.0) * max(dot(V, N), 0.0) + 0.0001;
        float3 specular = nom / max(denom, 0.0001);

        float3 kS = F;
        float3 kD = float3(1.0, 1.0, 1.0) - kS;

        kD *= 1.0 - mat.Metallic;

        float3 diffuse = kD * albedo.rgb / acos(-1);
        color.rgb += (diffuse + specular) * Lights[0].Diffuse.rgb * max(dot(N, L), 0.0);
    }

    return color;
}