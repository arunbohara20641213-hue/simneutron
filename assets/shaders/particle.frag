#version 460 core
in vec3 vWorldPos;

out vec4 fragColor;

uniform vec3 cameraPos;
uniform float pointSize;
uniform vec3 color;

void main()
{
    // Simple point sprite - just output color
    // In a real app, we'd use glPointSize and GL_POINT_SPRITE
    float size = pointSize;

    // Fade based on distance
    float dist = length(vWorldPos - cameraPos);
    float alpha = smoothstep(50.0, 0.0, dist);
    
    fragColor = vec4(color * alpha, alpha);
}