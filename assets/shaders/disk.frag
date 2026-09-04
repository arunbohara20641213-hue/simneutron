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

    // Brightest at the inner edge (hot material close to the star),
    // fading smoothly toward the outer edge.
    float innerBoost = smoothstep(diskInnerRadius * 1.25, diskInnerRadius, dist);
    float outerFade = 1.0 - smoothstep(diskOuterRadius * 0.8, diskOuterRadius, dist);

    fragColor = vec4(color * mask * (0.35 + 0.65 * innerBoost) * outerFade, 1.0);
}