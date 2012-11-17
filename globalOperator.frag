uniform sampler2D sampler0;


void main(void)

{
    vec4 sample = texture2D(sampler0, gl_TexCoord[0].st);
    gl_FragColor.rgb = sample.rgb/(sample.rgb + 1.0);
//    gl_FragColor.rgb = vec3(1.0, 0.0, 1.0);
    gl_FragColor.a = 1.0;
//    gl_FragColor = vec4(1.0, 0.0, 1.0, 0.0);
}