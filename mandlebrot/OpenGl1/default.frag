#version 330 core


in highp vec4 gl_FragCoord;
 
out highp vec4 frag_color;
out highp float gl_FragDepth;

#define MAX_ITERATIONS 2048

uniform highp float center_x;
uniform highp float center_y;
uniform highp float zoom;
uniform vec4 color_ranges;

uniform vec4 color_0;
uniform vec4 color_1;
uniform vec4 color_2;
uniform vec4 color_3;

int get_iterations()
{
    highp float real = ((gl_FragCoord.x / 1080.0f - 0.5f) * zoom + center_x) * 4.0f;
    highp float imag = ((gl_FragCoord.y / 1080.0f - 0.5f) * zoom + center_y) * 4.0f;
 
    int iterations = 0;
    highp float const_real = real;
    highp float const_imag = imag;
 
    while (iterations < MAX_ITERATIONS)
    {
        highp float tmp_real = real;

        //mandlebrot alla -1 (z = z^(-1) + c)
        //real = real / (real * real + imag * imag) + const_real;
        //imag = - imag / (tmp_real * tmp_real + imag * imag) + const_imag;

        //mandlebrot (z = z + c)
        //real = (real+ const_real);
        //imag = (imag+ const_imag);

        //mandlebrot al quadrato (z = z^2 + c)
        real = (real * real - imag * imag) + const_real;
        imag = (2.0f * tmp_real * imag) + const_imag;
        
        //mandlebrot al cubo (z = z^3 + c)
        //real = (real * real * real - 3*real*imag*imag ) + const_real;
        //imag = ( 3*tmp_real *tmp_real* imag - imag*imag*imag) + const_imag;
        
        //mandlebrot alla quarta (z = z^4 + c) By Paolo e Alex, brilli all'1 di mattina, 24 Lug 2022
        //real = (real * real * real * real - 6 * real * real * imag * imag + imag * imag * imag * imag + const_real);
        //imag = (4 * tmp_real * tmp_real * tmp_real * imag - 4 * tmp_real * imag * imag * imag + const_imag);


        //burning ship (z = (abs(real)+i*abs(imag))^2 + c)
        //real = (real * real - imag * imag) - const_real;
        //imag = (2.0f * abs(tmp_real * imag)) - const_imag;

        

        highp float dist = real * real + imag * imag;
 
        if (dist > 4.0f)
            break;
 
        ++iterations;
    }
    return iterations;
}
vec4 return_color()
{
    int iter = get_iterations();
    if (iter == MAX_ITERATIONS)
    {
        gl_FragDepth = 0.0f;
        return vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
 
    highp float iterations = float(iter) / MAX_ITERATIONS;
    gl_FragDepth = iterations;
    
    /*
    // Preset di colori fisso
    vec4 color_0 = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    vec4 color_1 = vec4(0.0f, 0.2f, 0.5f, 1.0f);
    vec4 color_2 = vec4(1.0f, 0.8f, 0.0f, 1.0f);
    vec4 color_3 = vec4(1.0f, 0.0f, 0.4f, 1.0f);
    */

    

    float fraction = 0.0f;
    if (iterations < color_ranges[1])
    {
        fraction = (iterations - color_ranges[0]) / (color_ranges[1] - color_ranges[0]);
        return mix(color_0, color_1, fraction);
    }
    else if(iterations < color_ranges[2])
    {
        fraction = (iterations - color_ranges[1]) / (color_ranges[2] - color_ranges[1]);
        return mix(color_1, color_2, fraction);
    }
    else
    {
        fraction = (iterations - color_ranges[2]) / (color_ranges[3] - color_ranges[2]);
        return mix(color_2, color_3, fraction);
    }
}
void main()
{
    frag_color = return_color();
}