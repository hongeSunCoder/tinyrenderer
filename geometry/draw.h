#ifndef __DRAW_H__
#define __DRAW_H__

#include "../tgaimage/tgaimage.h"
#include "../lesson3/geometry.h"

class Draw
{

private:
    /* data */
public:
    void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
    void line2(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
    void line3(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);

    void line(Vec2i t0, Vec2i t1, TGAImage &image, TGAColor color);
    void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);
    void triangle2(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);
    void triangle(Vec2i *pts, TGAImage &image, TGAColor color);
    void triangle(Vec3f *pts, float *zbuffer, TGAImage &image, TGAColor color);

    Draw(/* args */);

    ~Draw();
};

#endif