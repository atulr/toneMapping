varying vec3 normal, lightDir, eyeVec;

void main()
{	
//	lightDir = normalize(vec3(gl_LightSource[0].position));
//	normal = gl_NormalMatrix * gl_Normal;
    gl_TexCoord[0] = gl_MultiTexCoord0;    
	gl_Position = ftransform();		
}
