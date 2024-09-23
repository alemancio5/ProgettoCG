#version 450

layout(location = 0) in vec2 fragUV;
layout(location = 1) in vec3 fragPos;
layout(location = 2) in vec3 fragNorm;

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform LightsDiffusion {
    vec3 viewPos;
    vec3 lightPos;
    vec4 lightColor;
    vec4 lightStatus;
    vec3 winSpotLightPos;

    float ambientStrength;
    float specularStrength;
    float shininess;

    float cosIn;
    float cosOut;

    float isWin;
} LDubo;

layout(location = 0) out vec4 outColor;

void main() {
    // INIZIALIZZAZIONI UTILI
    // Normalizzazione della normale
    vec3 norm = normalize(fragNorm);
    // Eye direction
    vec3 viewDir = normalize(LDubo.viewPos - fragPos);
    // Sample the texture using UV coordinates
    vec3 texColor = texture(texSampler, fragUV).rgb;
    // Calcolo del vettore luce
    vec3 lightDir = normalize(LDubo.lightPos - fragPos);

    // COMPONENTI DI LUCE
    // Componente diffusa
    float diffuse = max(dot(norm, lightDir), 0.0);
    // Calcolo del riflesso speculare
    float specular = LDubo.specularStrength * pow(max(dot(viewDir, reflect(-lightDir, norm)), 0.0), LDubo.shininess);
    // Componente ambiente
    float ambient = LDubo.ambientStrength;

    // LUCE DIRETTA
    vec3 result = (ambient + diffuse + specular) * LDubo.lightColor.rgb * texColor * LDubo.lightStatus.x;

    // LUCE A PUNTO
    vec3 pointLight = LDubo.lightColor.rgb * pow((LDubo.lightColor.a / length(LDubo.lightPos - fragPos)), 2.0);
    result += (ambient + diffuse + specular) * pointLight * texColor * LDubo.lightStatus.y;

    // LUCE SPOT
    vec3 spotLight = LDubo.lightColor.rgb *
                        pow((LDubo.lightColor.a / length(LDubo.lightPos - fragPos)), 2.0) *
                        clamp( ( dot(normalize(LDubo.lightPos - fragPos), lightDir) - LDubo.cosOut ) / (LDubo.cosIn - LDubo.cosOut), 0.0, 1.0 );
    result += (ambient + diffuse + specular) * spotLight * texColor * LDubo.lightStatus.z;

    // LUCE SPOT NEW
    vec3 ligthWin = LDubo.isWin == 1.0 ? vec3(1.0 - LDubo.lightColor.r, 1.0 - LDubo.lightColor.g, 1.0 - LDubo.lightColor.b) : vec3(LDubo.lightColor);
    vec3 spotLightNew = ligthWin *
        pow((0.25 / length(LDubo.winSpotLightPos - fragPos)), 2.0) *
        clamp( ( dot(normalize(LDubo.winSpotLightPos - fragPos), normalize(LDubo.winSpotLightPos - fragPos)) - 0.3 ) / (0.5 - 0.3), 0.0, 1.0 );
    result += (ambient + diffuse + specular) * spotLightNew * ligthWin * LDubo.lightStatus.w;

    outColor = vec4(result, 1.0);
}
