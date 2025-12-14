#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sprite;
uniform float alpha;

void main()
{
    vec4 texColor = texture(sprite, TexCoords);
    // Texture'ın kendi alpha'sı * zamanla azalan alpha
    FragColor = vec4(texColor.rgb, texColor.a * alpha);
    
    // Eğer tamamen şeffafsa çizme (optimizasyon)
    if(FragColor.a < 0.1)
        discard;
}