#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <complex.h>

typedef struct {
    unsigned char level;
} pixel_t;

typedef struct {
    pixel_t *pixels;
    unsigned short width;
    unsigned short height;
} bitmap_t;

typedef double complex complex_t;

void write_tga(bitmap_t *bitmap, char *fname) {
    int datalen = 19 + bitmap->width*bitmap->height*3;
    unsigned char *data = (unsigned char*) calloc(1, datalen);
    data[2] = 2;
    memcpy(data+12, &bitmap->width, 2);
    memcpy(data+14, &bitmap->height, 2);
    data[16] = 24;
    unsigned char *offset = data+19;
    int i=0;
    for(int y=0;y<bitmap->height;y+=1){
        for(int x=0;x<bitmap->width;x+=1){
            offset[i] = bitmap->pixels[y*bitmap->width + bitmap->width-x-1].level;
            offset[i+1] = bitmap->pixels[y*bitmap->width + bitmap->width-x-1].level;
            offset[i+2] = bitmap->pixels[y*bitmap->width + bitmap->width-x-1].level;
            i += 3;
        }
    }
    FILE *file = fopen(fname,"wb");
    fwrite(data, 1, datalen, file);
    fclose(file);
    free(data);
}

void set_pixel(bitmap_t* bitmap, int x, int y, unsigned char level){
    bitmap->pixels[bitmap->width*y + x].level = level;
}

int main(int argc, char **argv){
    if( argc < 3 ){
        printf("usage: %s width height a b scale\n", argv[0]);
        printf("a b are constants in f(z) = z^2 + a + bi\n");
        printf("scale adjusts the zoom of the fractal. start with 0.25.\n");
        printf("outputs to fractal.tga\n");
        return 1;
    }

    // prepare TGA for writing
    bitmap_t output;
    output.width = atof(argv[1]);
    output.height = atof(argv[2]);
    output.pixels = (pixel_t*) calloc(sizeof(pixel_t), output.width*output.height);

    // declare variables
    int centerX = output.width/2;
    int centerY = output.height/2;
    int iterations = 128;

    double scale = 0.4;
    if( argc == 6 ) scale = atof(argv[5]);

    complex_t complex_constant = -0.221 - 0.713*I;
    if( argc >= 5 ) complex_constant = atof(argv[3]) + atof(argv[4]) * I;

    printf("Using function: f(z) = z^2 + %f + %fi\n", creal(complex_constant), cimag(complex_constant));


    // loop through pixels and generate intensity map
    for( int y=0; y < output.height; y+=1 ){
        for( int x=0; x < output.width; x+=1 ){

            complex_t z = (x-centerX)/(output.width*scale) +
                (y - centerY) * I / (output.width*scale);

            int count;
            for( count=0; count<iterations; count+=1 ){
                z = z*z + complex_constant;
                if( abs(creal(z)+1) > 2 || abs(cimag(z)+1) > 2 ){
                    set_pixel(&output, x, y, 0);
                    break;
                }
            }
            set_pixel(&output, x, y, (unsigned char) floor(((double)count/iterations)*255));

        }
    }

    write_tga(&output, "fractal.tga");
    free(output.pixels);
    return 0;
}
