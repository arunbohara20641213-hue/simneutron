#version 460 core
in vec3 vNormal;
in vec3 vWorldPos;

uniform vec3 lightPos;
uniform vec3 starColor;
uniform float starRadius;
uniform float camRadius;

out vec4 fragColor;

void main()
{
    // Normalize normal
    vec3 n = normalize(vNormal);

    // Light direction (from above/outside)
    vec3 lightDir = normalize(lightPos - vWorldPos);

    // Diffuse lighting
    float diff = max(dot(n, lightDir), 0.0);

    // Attenuation based on distance from center
    float dist = length(vWorldPos);
    float att = smoothstep(starRadius * 0.8, starRadius, dist);

    // Base color with diffuse
    vec3 base = starColor * diff;

    // Add glow effect for points near the surface
    float glow = smoothstep(starRadius, starRadius * 1.2, dist);
    base += starColor * 0.3 * glow;

    // HDR-like output (tonemapped)
    vec3 hdr = base * att;
    fragColor = vec4(hdr, 1.0);
}