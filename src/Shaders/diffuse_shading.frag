// Fragment shader for per-pixel diffuse shading. The
// teapot is a good object to test this shader, since
// the .obj file includes surface normals.

// The shader computes the dot product between the unit
// surface normal and light direction, which were
// passed as varying inputs from the vertex shader. The
// result is multiplied with the vertex color, which is
// accessed through a pre-defined varying variable.

varying vec3 normal, lightDir, eyeDir, reflectDir;

void main()
{
  vec3 normalN, lightDirN, eyeDirN, reflectDirN;

  normalN = normalize(normal);
  lightDirN = normalize(lightDir);
  eyeDirN = normalize(eyeDir);
  reflectDirN = normalize(reflectDir);

  float phong = dot(reflectDirN, eyeDirN);
  vec4 specColor = (pow(phong, gl_FrontMaterial.shininess)
                    * gl_LightSource[0].specular
                    * gl_FrontMaterial.specular);

  vec4 diffuseColor = gl_LightSource[0].diffuse *
    dot(lightDirN, normalN) *
    gl_FrontMaterial.diffuse;

  vec4 ambientColor = gl_LightSource[0].ambient * gl_FrontMaterial.ambient;
  vec4 globalAmbientColor = gl_LightModel.ambient * gl_FrontMaterial.ambient;

  gl_FragColor = diffuseColor + specColor + globalAmbientColor + ambientColor;
}
