#define MaxLights 20

struct Light
{
    float4  Ambient;
    float4  Diffuse;
    float4  Specular;
    float4  Emissive;
    float3  Direction;
    int     LightType;
    float2  FalloffInfo;
    float   SpotPower;
    int     padding1;
};

struct Material
{
    float4  Ambient;
    float4  Diffuse;
    float4  Specular;
    float4  Emmissive;
    float   Shiness;
};

cbuffer cbLight : register(b0)
{
    Light GlobalLight;
    Light Lights[MaxLights];
};

float4 ProcessAmbient(float4 ambient, float4 albedo)
{
    return ambient * albedo;
}

float4 ProcessDiffuse(float4 diffuse, float4 albedo, float3 lightDir, float3 normal)
{
    float4 totalDiffuse;

    float diffuseValue = dot(-lightDir, normal);

    // Toon Shading Part
    if (diffuseValue > 0.5f)
        diffuseValue = 1.0f;
    else if (diffuseValue > 0.0f)
        diffuseValue = 0.6f;
    else
        diffuseValue = 0.4f;

    totalDiffuse = diffuse * albedo * diffuseValue;

    return totalDiffuse;
}

float4 ProcessSpecular(float4 specular, float shiness, float3 lightDir, float3 normal, float3 eyeDir)
{
    float4 totalSpecular;

    float3 reflection = reflect(lightDir, normal);
    float specularValue = saturate(dot(reflection, eyeDir));
    specularValue = pow(specularValue, 10);
    
    if (specularValue > 0.8f)
        specularValue = 0.8f;
    else if (specularValue > 0.1f)
        specularValue = 0.5f;
    else
        specularValue = 0.0f;

    totalSpecular = specular * specularValue * shiness;
    //totalSpecular = specular * specularValue;

    return totalSpecular;
}

float4 ComputeLight(Material mat, float4 albedo, float3 normal, float3 eyeDir)
{
    float4 totalColor;

    // Global Light Process
    {
        float3 lightDir = normalize(GlobalLight.Direction);

        float4 ambient = ProcessAmbient(GlobalLight.Ambient * mat.Ambient, albedo);
        float4 diffuse = ProcessDiffuse(GlobalLight.Diffuse * mat.Diffuse, albedo, lightDir, normal);
        float4 specular = ProcessSpecular(GlobalLight.Specular * mat.Specular, mat.Shiness, lightDir, normal, eyeDir);
    
        totalColor = ambient + diffuse + specular;
    }

    // General Light Process
    {
        for (int i = 0; i < MaxLights; i++)
        {
            float4 diffuse = ProcessDiffuse(Lights[i].Diffuse * mat.Diffuse, albedo, Lights[i].Direction, normal);
            float4 specular = ProcessSpecular(Lights[i].Specular * mat.Specular, mat.Shiness, Lights[i].Direction, normal, eyeDir);
        
            totalColor += diffuse;
        }
    }

    return totalColor;
}
