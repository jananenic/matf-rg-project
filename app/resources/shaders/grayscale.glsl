//#shader vertex
#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{

    TexCoords = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}

//#shader fragment
#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screen_texture;
uniform bool grayscale_enabled;

void main() {
    FragColor = texture(screen_texture, TexCoords);
    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    //float average = 0.299 * FragColor.r + 0.587 * FragColor.g + 0.144 * FragColor.b;
    vec4 grayFragColor = vec4(average, average, average, 1.0);
    FragColor = grayscale_enabled ? grayFragColor : FragColor;


}
