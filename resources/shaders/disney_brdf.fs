#version 400

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vTangent;
in vec3 vBinormal;
uniform sampler2D tDiffuse;
uniform vec4 params0;
uniform vec4 params1;

struct BRDFParameter{
    vec4 baseColor;
    vec3 Cspec0;
    vec3 Csheen;
    float metallic;
    float specular;
    float linearRoughness;
    float subsurface;
    float specularTint;
    float anisotropic;
    float sheen;
    float sheenTint;
    float rim;
};

layout( location = 0 ) out vec4 FragColor;
float PI = 3.1416159;
float subsurface = 0.0;
float fuzziness = 0.0;

vec3 mon2lin(vec3 x) {
    vec3 xx = max(x, vec3(0.0));
    return vec3(pow(xx[0], 2.2), pow(xx[1], 2.2), pow(xx[2], 2.2));
}


BRDFParameter extractParameter(sampler2D baseMap, vec2 uv, vec2 repeat, vec2 offset) {
    BRDFParameter param;
    vec4 baseTexel = texture2D(baseMap, uv * repeat + offset);
    param.baseColor = vec4(mon2lin(baseTexel.rgb), baseTexel.a);

    vec4 param0 = params0;
    vec4 param1 = params1;
	float rim = 0.0;
    param.metallic = param0.b;
    param.specular = param0.r;
    param.linearRoughness = 1.0 - param0.g;
    param.subsurface = param0.a;

    param.specularTint = param1.r;
    param.anisotropic = param1.g;
    param.sheen = param1.b;
    param.sheenTint = param1.a;
    param.rim = rim;
    return param;
}

float sqr(float x) 
{ 
	return x*x; 
}

float FSchlick(float f0, float f90, float u) {
    float m = clamp(1.0 - u, 0.0, 1.0);
    float m2 = m * m;
    float m5 = m2 * m2 * m;
    return f0 + f90 * m5;
}

vec3 FSchlick(vec3 f0, vec3 f90, float u) {
    float m = clamp(1.0 - u, 0.0, 1.0);
    float m2 = m * m;
    float m5 = m2*m2 * m ;
    return f0 + f90 * m5;
}

float FSchlick(float u) {
    float m = clamp(1.0-u, 0.0, 1.0);
    float m2 = m*m;
    return m2*m2*m; // pow(m,5)
}

float originDisneyDiffuse(float nDotV, float nDotL, float lDotH, float linearRoughness, float rimAmount)
{
    float fd90 =  2.0 * lDotH * lDotH * linearRoughness;
    float f0 = 1.0;
    float lightScatter = FSchlick(f0, fd90, nDotL);
    float viewScatter = FSchlick(f0, fd90, nDotV);
    return mix(1.0, lightScatter * viewScatter, rimAmount);
}

vec3 disneySheen(vec3 csheen, float sheen,  float lDotH)
{
    vec3 FH = vec3(FSchlick(lDotH));
    return csheen * vec3(sheen) * FH;
}

float Distribution_Velvet(float NdotH, float rn)
{
    float cot_sqr = 1.0 / (1.0 - sqr(NdotH)) - 1.0;
    float sin_quad = sqr(1.0 - sqr(NdotH));
    float roughness_sqr = sqr(rn);
    float distribution_term = 1.0 + 4.0 * exp(-cot_sqr / roughness_sqr) / sin_quad;
    distribution_term /= PI * (1.0 + 4.0 * roughness_sqr);
    return distribution_term;
}

float GTR2_aniso(float nDotH, float hDotX, float hDotY, float ax, float ay)
{
    return 1.0 / (PI * ax * ay * sqr(sqr(hDotX / ax) + sqr(hDotY / ay) + nDotH * nDotH));
}

float smithGGX(float nDotV, float alphaG)
{
    float a = alphaG * alphaG;
    float b = nDotV * nDotV;
    return (2.0 * nDotV) / (nDotV + sqrt(a + b - a * b));
}

float smithGGXUncorrelated(float nDotL, float nDotV, float alphaG)
{
    return smithGGX(nDotL, alphaG) * smithGGX(nDotV, alphaG);
}

vec3 disneySpecular(vec3 f0, float nDotH, float lDotH, float nDotL, float nDotV, float hDotX, float hDotY, float anisotropic, float linearRoughness)
{
    float aspect = sqrt(1.0 - anisotropic * 0.9);
    float ax = max(0.001, sqr(linearRoughness) / aspect);
    float ay = max(0.001, sqr(linearRoughness) * aspect);
    float dFuzz = Distribution_Velvet(nDotH, linearRoughness);
    float D = mix(GTR2_aniso(nDotH, hDotX, hDotY, ax, ay), dFuzz, fuzziness);
    //	float D = mix(GGX(nDotH, sqr(linearRoughness)), dFuzz, fuzziness );
    vec3 F = FSchlick(f0, vec3(1.0), lDotH);
    float roughg = (linearRoughness * 0.5 + 0.5);
    roughg *= roughg;

    float G = smithGGXUncorrelated(nDotL, nDotV, roughg);
    //	float G = smithGGXCorrelated(nDotL, nDotV, roughg);
    return F * vec3(D) * vec3(G);
}

float disneySubsurface(float nDotV, float nDotL, float lDotH, float linearRoughness)
{
    float energyBias = mix(0.0, 0.5, subsurface);
    float energyFactor = mix(1.0, 1.0 / 1.51, linearRoughness);
    float Fss90 = lDotH*lDotH*linearRoughness;
    float fd90 = energyBias + 2.0 * Fss90;
    float f0 = 1.0;
    float lightScatter = FSchlick(f0, fd90, nDotL);
    float viewScatter = FSchlick(f0, fd90, nDotV);
    float Fss = mix(1.0, Fss90, lightScatter) * mix(1.0, Fss90, viewScatter);
    float subsurfaceScatter = 1.25 * (Fss * (1.0 / (nDotL + nDotV) - .5) + .5);
    return subsurfaceScatter;
}

vec3 BRDF( vec3 L, vec3 V, vec3 N, vec3 X, vec3 Y, BRDFParameter param)
{
	vec3 H = normalize(L + V);
    float nDotV = max(dot(N, V), 0.001 );
    float nDotL = max(dot(N, L), 0.0);
    float lDotH = max(dot(L, H), 0.0);
    float nDotH = max(dot(N, H), 0.0);
    float vDotL = max(dot(V, L), 0.0);

    float diffuseAmount = originDisneyDiffuse(nDotV, nDotL, lDotH, param.linearRoughness, param.rim);
    vec3 sheenAmount = disneySheen(param.Csheen, param.sheen, lDotH  );
    float subsurfaceAmount = disneySubsurface(nDotV, nDotL, lDotH, param.linearRoughness);
    
    vec3 specularAmount = disneySpecular(param.Cspec0, nDotH, lDotH, nDotL, nDotV, dot(H, X), dot(H, Y), param.anisotropic, param.linearRoughness );
    return ((param.baseColor.rgb / PI) * mix(diffuseAmount, subsurfaceAmount, param.subsurface) + sheenAmount ) * (1.0 - param.metallic);
}

void main() {
	vec3 normal = normalize(vNormal);
	vec3 tangent = normalize(vTangent);
    vec3 bitangent = normalize(vBinormal);

	BRDFParameter param = extractParameter(tDiffuse, vTexCoord, vec2(1, 1), vec2(0, 0));
    
	vec3 light_term = vec3(0.0);
    vec3 dirEye = normalize(vPosition );

    float nDotV = dot(normal, dirEye);
    nDotV = max(dot(normal, dirEye), 0.0);

	vec3 lightDirection = vec3(0.0, 100.0, 150.0);
	vec3 lightColor = vec3(10.0);

    vec3 dirLight = normalize(lightDirection);
    float nDotL =  max(dot(normal, dirLight), 0.0);
    vec3 brdf = BRDF( dirLight, dirEye,  normal, tangent, bitangent, param );
    light_term += brdf * nDotL * lightColor;

	FragColor = vec4(light_term, 1.0);
}
