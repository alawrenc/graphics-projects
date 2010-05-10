/* // Vertex shader for per-pixel diffuse shading.The */
/* // teapot is a good object to test this shader, since */
/* // the .obj file includes surface normals. */

/* // The shader computes the unit light direction and unit */
/* // surface normal, which are passed to the fragment */
/* // shader as varying variables. */

varying vec3 normal, eyeDir, LDirect, reflectDirDirect, LPoint, reflectDirPoint, reflectDirSpot, LSpot;

varying vec4 globalAmbientColor, ambientColorDirect, clPointSpec, clPointDiff, ambientColorPoint, ambientColorSpot, clSpotSpec, clSpotDiff;

void main()
{
        globalAmbientColor = gl_LightModel.ambient * gl_FrontMaterial.ambient;

        /* first transform the normal into eye space and normalize the result */
        normal = normalize(gl_NormalMatrix * gl_Normal);
        eyeDir = -normalize(vec3(gl_ModelViewMatrix * gl_Vertex));

        //Directional
        LDirect = -normalize(vec3(gl_LightSource[0].position));
        reflectDirDirect = normalize(reflect(LDirect, normal));
        ambientColorDirect = gl_LightSource[0].ambient * gl_FrontMaterial.ambient;

        //Point
        vec3 lightSrcPoint = normalize(vec3(gl_LightSource[1].position));
        vec3 p_vPoint = lightSrcPoint - eyeDir; //minus a negative = +
        LPoint = normalize(p_vPoint);
        reflectDirPoint = normalize(reflect(LPoint, normal));

        float dist = length(p_vPoint);
        float denom = pow(dist, 2.0);
        clPointSpec = gl_LightSource[1].specular / denom;
        clPointDiff = gl_LightSource[1].diffuse / denom;

        ambientColorPoint = gl_LightSource[1].ambient * gl_FrontMaterial.ambient;

        //spot
        vec3 lightSrcSpot = normalize(vec3(gl_LightSource[2].position));
        vec3 p_vSpot = lightSrcSpot - eyeDir; //minus a negative = +
        LSpot = normalize(p_vSpot);
        reflectDirSpot = normalize(reflect(LSpot, normal));
        ambientColorSpot = gl_LightSource[2].ambient * gl_FrontMaterial.ambient;

        float angle = dot(-LSpot, lightSrcSpot);

        if (angle <= gl_LightSource[2].spotCosCutoff)
          {
            clSpotSpec = vec4(0.0, 0.0, 0.0, 0.0);
            clSpotDiff = vec4(0.0, 0.0, 0.0, 0.0);
          }
        else
          {
            float powD = pow(angle, gl_LightSource[2].spotExponent);
            clSpotSpec = gl_LightSource[2].specular * powD;
            clSpotDiff = gl_LightSource[2].diffuse * powD;
          }
        gl_TexCoord[0] = gl_MultiTexCoord0;
        gl_Position = ftransform();
}
