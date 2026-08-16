#version 460 core
in vec3 vNormal;
in vec3 vWorldPos;

uniform vec3 lightPos;
uniform float diskInnerRadius;
uniform float diskOuterRadius;
uniform vec3 diskColor;

out vec4 fragColor;

void main()
{
    vec3 n = normalize(vNormal);

    // Light direction
    vec3 lightDir = normalize(lightPos - vWorldPos);
    float diff = max(dot(n, lightDir), 0.0);

    // Distance from center in disk plane
    float dist = length(vWorldPos.xz);

    // Annular mask - only render between inner and outer radius
    float mask = step(diskInnerRadius, dist) - step(diskOuterRadius, dist);

    // Base color with diffuse
    vec3 color = diskColor * diff;

    // Glow effect near edges
    float glow = smoothstep(diskOuterRadius, diskOuterRadius * 1.1, dist);
    color += diskColor * 0.2 * glow;

    // Fade at center
    float centerFade = smoothstep(0.0, diskInnerRadius * 0.3, dist);

    fragColor = vec4(color * mask * (1.0 - glow) * (1.0 - centerFade), 1.0);
}