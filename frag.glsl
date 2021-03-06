#version 150

out vec4 colorOut;
uniform vec2 res;
uniform vec2 offset;
uniform float t;
uniform float zoom = 1.0;

 #define MARCHLIMIT 70

vec3 camPos = vec3(0.0, 0.0, -1.0);
vec3 ld = vec3(0.0, 0.0, 1.0);

// Smooth HSV to RGB conversion
vec3 hsv2rgb_smooth( in vec3 c )
{
    vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );

	rgb = rgb*rgb*(3.0-2.0*rgb); // cubic smoothing

	return c.z * mix( vec3(1.0), rgb, c.y);
}

vec4 range(vec3 p)
{
    // Sphere with Radius
    vec3 spherepos = vec3(0.0, 0.0, 0.0);
    float radius = log(sin(t*0.1)*0.05+1.0)+0.1;

    // Sinewave effect
    //										  V this cosine with floor acts as a toggle
    float xampl = sin(t*1.3)*0.4;//*floor(cos(t*0.4)+1.0);
    float yampl = sin(t*1.3)*0.4;//-(floor(-cos(t*0.4)+1.0)*0.3);

    p.x += cos((max(-2.0+p.z-camPos.z,0.)))*xampl-xampl;
    p.y += sin((max(-2.0+p.z-camPos.z,0.)))*yampl;

    // Pulsating effect
    p.x *= -min(+2.0+p.z-camPos.z,0.)*sin(t*3.0)*0.1 + 1.0;
    p.y *= -min(+2.0+p.z-camPos.z,0.)*sin(t*3.0)*0.1 + 1.0;

    p = mod(p + vec3(0.5,0.5,0.5), vec3(1.0,1.0,1.0)) - vec3(0.5,0.5,0.5);
    spherepos = mod(spherepos + vec3(0.5,0.5,0.5), vec3(1.0,1.0,1.0)) - vec3(0.5,0.5,0.5);

    vec3 diff = p - spherepos;

    vec3 normal = normalize(diff);


    return vec4(normal, length(diff)-radius);
}

// Basic linear interpolation (Only used in the fresnel effect
vec3 lerp(vec3 a, vec3 b, float p)
{
    p = clamp(p,0.,1.);
 	return a*(1.0-p)+b*p;
}


vec4 march(vec3 cam, vec3 n)
{

    float len = 1.0;
    vec4 ret;

    for(int i = 0; i < MARCHLIMIT; i++)
    {
        ret = range(camPos + len*n)*0.5;
		len += ret.w;
    }

	return vec4(ret.xyz, len);
}

void main()
{
    vec2 uv = gl_FragCoord.xy/res.xy - vec2(0.5, 0.5) + offset;
    uv.x *= res.x / res.y;

    float rotangle = t*0.08;
    vec2 newuv;
    newuv.x = uv.x*cos(rotangle)-uv.y*sin(rotangle);
    newuv.y = uv.x*sin(rotangle)+uv.y*cos(rotangle);
    uv = newuv;

    camPos = vec3(0.5, 0.5, t*1.0);

    ld = normalize(vec3(0.0, 0.0, 1.0));

    // This is the raymarching vector. It is calculated by interpreting the uv coordinates as angles, and thus rotating
    // the ld (lookdirection) vector by the given angle. It is then used as the direction for the ray to march in.
    // With this projection you can see the full 360° around you. Try changing the zoom to something like 1.5
    float zoom = pow(2,zoom);
    vec3 n = normalize(vec3(sin(uv.x*3.1415*zoom),sin(uv.y*3.1415*zoom) ,ld.z*cos(uv.x*3.1415*zoom)*cos(uv.y*3.1415*zoom)));

    vec4 rangeret = march(camPos, n); // March rays from the camera in the direction of n

    float d = log(rangeret.w / 1.0 + 1.0);	// Take logarithm of distance to make transition more smooth for further away objects
    vec3 normal = rangeret.xyz;	// Extract normal from return vector

    // Calculate angle between the raymarching ray and normal (I think this is broken, but it looks good)
    vec3 p = camPos + n*d;
    float angle = acos(dot(normal, n)/length(normal)*length(n));

    colorOut = vec4(hsv2rgb_smooth(lerp(vec3(length(vec2(uv.x*3.1415*zoom, uv.y*3.1415*zoom))+d*0.1 + t*0.01, 2.0, max(1.0 - log(d),0.0)),vec3(d*0.01 + (t+120.0)*0.01 , 2.0, max(1.0 - log(d),0.0)),cos(angle/10.0))),1.0);
}