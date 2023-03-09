
#include "draw.h"

Vec3f barycentric(Vec2i *pts, Vec2i P)
{

    Vec3f u = Vec3f(pts[2][0] - pts[0][0], pts[1][0] - pts[0][0], pts[0][0] - P[0]) ^ Vec3f(pts[2][1] - pts[0][1], pts[1][1] - pts[0][1], pts[0][1] - P[1]);

    /* `pts` and `P` has integer value as coordinates
           so `abs(u[2])` < 1 means `u[2]` is 0, that means
           triangle is degenerate, in this case return something with negative coordinates */
    if (std::abs(u.z) < 1)
        return Vec3f(-1, 1, 1);
    return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

void Draw::triangle(Vec2i *pts, TGAImage &image, TGAColor color)
{
    Vec2i bboxmin(image.width() - 1, image.height() - 1);
    Vec2i bboxmax(0, 0);
    Vec2i clamp(image.width() - 1, image.height() - 1);
    for (int i = 0; i < 3; i++)
    {
        bboxmin.x = std::max(0, std::min(bboxmin.x, pts[i].x));
        bboxmin.y = std::max(0, std::min(bboxmin.y, pts[i].y));

        bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
        bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
    }
    Vec2i P;
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
    {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
        {
            Vec3f bc_screen = barycentric(pts, P);
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
                continue;
            image.set(P.x, P.y, color);
        }
    }
}

// fill triangle
void Draw::triangle2(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
{
    if (t0.y > t1.y)
        std::swap(t0, t1);
    if (t0.y > t2.y)
        std::swap(t0, t2);
    if (t1.y > t2.y)
        std::swap(t1, t2);

    // bottom-half triangle
    for (int y = t0.y; y <= t1.y; y++)
    {

        float k2 = (float)(y - t0.y) / (t2.y - t0.y);
        int x2 = t0.x + k2 * (t2.x - t0.x);

        float k1 = (float)(y - t0.y) / (t1.y - t0.y + 1); // add 1 to in case divisions by zero
        int x1 = t0.x + k1 * (t1.x - t0.x);

        Vec2i A = Vec2i(x2, y);
        Vec2i B = Vec2i(x1, y);

        color[2] = 100;
        line(A, B, image, color);

        color[2] = 255;
        image.set(A.x, y, color);
        image.set(B.x, y, color);
    }

    // up-half triangle
    for (int y = t1.y; y <= t2.y; y++)
    {

        float k0 = (float)(y - t0.y) / (t2.y - t0.y);
        int x0 = t0.x + k0 * (t2.x - t0.x);

        float k1 = (float)(y - t1.y) / (t2.y - t1.y + 1);
        int x1 = t1.x + k1 * (t2.x - t1.x);

        Vec2i A = Vec2i(x0, y);
        Vec2i B = Vec2i(x1, y);

        color[2] = 255;
        line(A, B, image, color);

        color[2] = 80;
        image.set(x0, y, color);
        image.set(x1, y, color);
    }
}

void Draw::triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
{
    if (t0.y > t1.y)
        std::swap(t0, t1);
    if (t0.y > t2.y)
        std::swap(t0, t2);
    if (t1.y > t2.y)
        std::swap(t1, t2);

    line(t0, t1, image, color);
    line(t1, t2, image, color);
    line(t2, t0, image, color);
}

void Draw::line(Vec2i t0, Vec2i t1, TGAImage &image, TGAColor color)
{

    int x0 = t0.x, y0 = t0.y;
    int x1 = t1.x, y1 = t1.y;

    line(x0, y0, x1, y1, image, color);
}

void Draw::line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    for (float t = 0; t < 1; t += .1)
    {
        int x = x0 + (x1 - x0) * t;
        int y = y0 + (y1 - y0) * t;

        // printf("x: %d y: %d\n", x, y);
        image.set(x, y, color);
    }
    // printf("line end.........\n");
}

void Draw::line2(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    for (int x = x0; x <= x1; x++)
    {
        float t = (x - x0) / (float)(x1 - x0);
        int y = y0 * (1 - t) + y1 * t;

        printf("x: %d y: %d \n", x, y);
        image.set(x, y, color);
    }

    printf("line2 end.........\n");
}

void Draw::line3(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror2 = std::abs(dy) * 2;
    int error2 = 0;
    int y = y0;
    for (int x = x0; x <= x1; x++)
    {
        if (steep)
        {
            image.set(y, x, color);
        }
        else
        {
            image.set(x, y, color);
        }
        error2 += derror2;
        if (error2 > dx)
        {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

Draw::Draw(/* args */)
{
}

Draw::~Draw()
{
}