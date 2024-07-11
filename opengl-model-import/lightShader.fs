#version 330 core

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_emission;
	float shininess;
};

struct DirLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 viewPosition;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct FlashLight
{
	vec3 viewPosition;
	vec3 viewDirection;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float cutOff;
	float outerCutOff;
};

uniform DirLight dirLight;
vec3 ComputeDirLight(DirLight light, vec3 normal, vec3 viewDir);

#define N_POINT_LIGHTS 4
uniform PointLight pointLights[N_POINT_LIGHTS];
vec3 ComputePointLight(PointLight light, vec3 modelViewPos, vec3 normal, vec3 viewDir);

uniform FlashLight flashLight;
vec3 ComputeFlashLight(FlashLight light, vec3 modelViewPos, vec3 normal, vec3 viewDir);

uniform Material material;
uniform float time;

in vec3 ModelViewPos;
in vec3 Normal;
in vec2 TexCoord;
out vec4 FragColor;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = -normalize(ModelViewPos);

	vec3 result = ComputeDirLight(dirLight, norm, viewDir);

	for (int i = 0; i < N_POINT_LIGHTS; i++)
	{
		result += ComputePointLight(pointLights[i], ModelViewPos, norm, viewDir);
	}

	result += ComputeFlashLight(flashLight, ModelViewPos, norm, viewDir);

	vec3 emission = vec3(0.0);

	if (texture(material.texture_specular1, TexCoord).rgb == vec3(0.0))
	{
		emission = texture(material.texture_emission, TexCoord + vec2(0.0, time)).rgb;

		emission *= (sin(time) + 1) / 2;
	}

	result += emission;

	FragColor = vec4(result, 1.0);
}

vec3 ComputeDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(light.direction);

	float diff = max(dot(normal, -lightDir), 0.0);

	vec3 reflectDir = lightDir - 2 * dot(lightDir, normal) * normal;
	float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);

	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoord));
	vec3 diffuse = diff * light.diffuse * vec3(texture(material.texture_diffuse1, TexCoord));
	vec3 specular = spec * light.specular * vec3(texture(material.texture_specular1, TexCoord));
	return (ambient + diffuse + specular);
}

vec3 ComputePointLight(PointLight light, vec3 modelViewPos, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(modelViewPos - light.viewPosition);

	float distance = length(light.viewPosition - modelViewPos);
	float diff = max(dot(normal, -lightDir), 0.0);
	float attenuation = 1.0 / (light.constant + distance * light.linear + (distance * distance) * light.quadratic);

	vec3 reflectDir = lightDir - 2 * dot(lightDir, normal) * normal;
	float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);

	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoord));
	vec3 diffuse = diff * light.diffuse * vec3(texture(material.texture_diffuse1, TexCoord));
	vec3 specular = spec * light.specular * vec3(texture(material.texture_specular1, TexCoord));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 ComputeFlashLight(FlashLight light, vec3 modelViewPos, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(modelViewPos - light.viewPosition);

	float theta = dot(lightDir, normalize(light.viewDirection));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	float diff = max(dot(normal, -lightDir), 0.0);
	float distance = length(light.viewPosition - modelViewPos);
	float attenuation = 1.0 / (light.constant + distance * light.linear + (distance * distance) * light.quadratic);

	vec3 reflectDir = lightDir - 2 * dot(lightDir, normal) * normal;
	float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);

	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoord));
	vec3 diffuse = diff * light.diffuse * vec3(texture(material.texture_diffuse1, TexCoord));
	vec3 specular = spec * light.specular * vec3(texture(material.texture_specular1, TexCoord));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	diffuse *= intensity;
	specular *= intensity;

	return (ambient + diffuse + specular);
}