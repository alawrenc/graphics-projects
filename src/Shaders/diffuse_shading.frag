// Fragment shader for per-pixel diffuse shading. The
// teapot is a good object to test this shader, since
// the .obj file includes surface normals.

// The shader computes the dot product between the unit
// surface normal and light direction, which were
// passed as varying inputs from the vertex shader. The
// result is multiplied with the vertex color, which is
// accessed through a pre-defined varying variable.

varying vec3 normal, eyeDir;

void main()
{
  vec3 normalN, lightDirN, eyeDirN, reflectDirN;

  normalN = normalize(normal);
  eyeDirN = normalize(eyeDir);

  vec4 globalAmbientColor = gl_LightModel.ambient * gl_FrontMaterial.ambient;
  vec4 color = globalAmbientColor;

  for(int i = 0; i < 2; i++)
    {
      lightDirN = normalize(vec3(gl_LightSource[i].position));
      reflectDirN = normalize(reflect(-lightDirN, normalN));
      float phong = dot(reflectDirN, eyeDirN);

      vec4 specColor = (pow(phong, gl_FrontMaterial.shininess)
                        * gl_LightSource[i].specular
                        * gl_FrontMaterial.specular);

      vec4 diffuseColor = (gl_LightSource[i].diffuse * gl_FrontMaterial.diffuse
                           * max(dot(lightDirN, normalN), 0.0));

      vec4 ambientColor = gl_LightSource[i].ambient * gl_FrontMaterial.ambient;

      color = color + specColor + diffuseColor + ambientColor;
    }
  gl_FragColor = color;
}
