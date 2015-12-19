#version 150

out vec4 colorOut;
uniform vec2 res;
uniform float t;
uniform float zoom = 1.0;

vec2 u = gl_FragCoord.xy;
float iGlobalTime = t;
vec3 iResolution = vec3(res, 1.0);

void main()
{
    vec4 f = vec4(1.0);

    float s = 1.;
    vec3 r = vec3(s*.5, s*.5,mod(iGlobalTime*s*1.,s)) + s*.5,
         R = iResolution ;

    u-= R.xy*.5;
    float d = length(u/R.y)*2.;
    float a = sin(iGlobalTime*.1);
    u*= mat2(d,a,-a,d);
    u+=R.xy*.5;

    for( float i = .7; i > .1 ; i-=.01 ) {
        r += vec3( (u+u-R.xy)/R.y, 2 ) * .4 * ( f.a = length( mod(r,s) - (s*.5) ) - .3 ) ;
        f.bgr=abs(sin(vec3(i)));
        if( f.a < .001 ) break ;
    }
    f.rgb = sin(iGlobalTime+(f.rgb+vec3(0.0,.33,.66))*6.)*.5+.5;


    colorOut = vec4(f.rgb*r,1.0);
}
