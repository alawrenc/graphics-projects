/* // Fragment shader for per-pixel diffuse shading. The */
/* // teapot is a good object to test this shader, since */
/* // the .obj file includes surface normals. */

/* // The shader computes the dot product between the unit */
/* // surface normal and light direction, which were */
/* // passed as varying inputs from the vertex shader. The */
/* // result is multiplied with the vertex color, which is */
/* // accessed through a pre-defined varying variable. */

varying vec3 normal, eyeDir, LDirect, reflectDirDirect, LPoint, reflectDirPoint, reflectDirSpot, LSpot;
uniform sampler2D tex;
varying vec4 globalAmbientColor, ambientColorDirect, clPointSpec, clPointDiff, ambientColorPoint, ambientColorSpot, clSpotSpec, clSpotDiff;

void main()
{
        vec3 normalN = normalize(normal);
        vec3 eyeDirN = normalize(eyeDir);

        //directional light. phong model
        vec3 LDirectN = normalize(LDirect);
        vec3 reflectDirDirectN = normalize(reflectDirDirect);

        float phongDirect = max(dot(reflectDirDirectN, eyeDirN), 0.0);

        vec4 directSpec = gl_LightSource[0].specular * gl_FrontMaterial.specular;
        vec4 specColorDirect = directSpec * pow(phongDirect, gl_FrontMaterial.shininess);

        vec4 directDiffuse = gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse;
        vec4 diffuseColorDirect = directDiffuse * max(dot(LDirectN, normalN), 0.0);

        //point light. phong model
        vec3 LPointN = normalize(LPoint);
        vec3 reflectDirPointN = normalize(reflectDirPoint);

        float phongPoint = max(dot(reflectDirPointN, eyeDirN), 0.0);

        //cSpec = ks.spec * cl.spec * pow((dot(ReflectDir, eyeDir),phong)
        vec4 pointSpec = clPointSpec * gl_FrontMaterial.specular;
        vec4 specColorPoint = pointSpec * pow(phongPoint,
                                              gl_FrontMaterial.shininess);
        //
        vec4 pointDiffuse = clPointDiff * gl_FrontMaterial.diffuse;
        vec4 diffuseColorPoint = pointDiffuse * max(dot(LPointN, normalN), 0.0);

        //spot light. phong model
        vec3 LSpotN = normalize(LSpot);
        vec3 reflectDirSpotN = normalize(reflectDirSpot);

        float phongSpot = max(dot(reflectDirSpotN, eyeDirN), 0.0);

        /* //cSpec = ks.spec * cl.spec * pow((dot(ReflectDir, eyeDir),phong) */
        vec4 spotSpec = clSpotSpec * gl_FrontMaterial.specular;
        vec4 specColorSpot = spotSpec * pow(phongSpot, gl_FrontMaterial.shininess);

        vec4 spotDiffuse = clSpotDiff * gl_FrontMaterial.diffuse ;
        vec4 diffuseColorSpot = spotDiffuse * max(dot(LSpotN, normalN), 0.0);

        gl_FragColor = gl_FragColor = (texture2D(tex, gl_TexCoord[0].st) +
                                       specColorDirect + diffuseColorDirect + ambientColorDirect +
                                       specColorPoint + diffuseColorPoint + ambientColorPoint +
                                       specColorSpot + diffuseColorSpot + ambientColorSpot +
                                       globalAmbientColor);
}
