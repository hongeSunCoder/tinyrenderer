#include <limits>
#include <cmath>
#include <vector>
#include "geometry/draw.h"

#include "tgaimage/tgaimage.h"
// #include "lesson1/geometry.h"
#include "lesson1/model.h"
// #include "our_gl.h"

constexpr int width = 800; // output image size
constexpr int height = 800;
constexpr int depth = 255;
// constexpr vec3 light_dir{1, 1, 1}; // light source
// constexpr vec3 eye{1, 1, 3};       // camera position
// constexpr vec3 center{0, 0, 0};    // camera direction
// constexpr vec3 up{0, 1, 0};        // camera up vector

// extern mat<4, 4> ModelView; // "OpenGL" state matrices
// extern mat<4, 4> Projection;

// struct Shader : IShader
// {
//     const Model &model;
//     vec3 uniform_l;        // light direction in view coordinates
//     mat<2, 3> varying_uv;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
//     mat<3, 3> varying_nrm; // normal per vertex to be interpolated by FS
//     mat<3, 3> view_tri;    // triangle in view coordinates

//     Shader(const Model &m) : model(m)
//     {
//         uniform_l = proj<3>((ModelView * embed<4>(light_dir, 0.))).normalized(); // transform the light vector to view coordinates
//     }

//     virtual void vertex(const int iface, const int nthvert, vec4 &gl_Position)
//     {
//         varying_uv.set_col(nthvert, model.uv(iface, nthvert));
//         varying_nrm.set_col(nthvert, proj<3>((ModelView).invert_transpose() * embed<4>(model.normal(iface, nthvert), 0.)));
//         gl_Position = ModelView * embed<4>(model.vert(iface, nthvert));
//         view_tri.set_col(nthvert, proj<3>(gl_Position));
//         gl_Position = Projection * gl_Position;
//     }

//     virtual bool fragment(const vec3 bar, TGAColor &gl_FragColor)
//     {
//         vec3 bn = (varying_nrm * bar).normalized(); // per-vertex normal interpolation
//         vec2 uv = varying_uv * bar;                 // tex coord interpolation

//         // for the math refer to the tangent space normal mapping lecture
//         // https://github.com/ssloy/tinyrenderer/wiki/Lesson-6bis-tangent-space-normal-mapping
//         mat<3, 3> AI = mat<3, 3>{{view_tri.col(1) - view_tri.col(0), view_tri.col(2) - view_tri.col(0), bn}}.invert();
//         vec3 i = AI * vec3{varying_uv[0][1] - varying_uv[0][0], varying_uv[0][2] - varying_uv[0][0], 0};
//         vec3 j = AI * vec3{varying_uv[1][1] - varying_uv[1][0], varying_uv[1][2] - varying_uv[1][0], 0};
//         mat<3, 3> B = mat<3, 3>{{i.normalized(), j.normalized(), bn}}.transpose();

//         vec3 n = (B * model.normal(uv)).normalized();                                      // transform the normal from the texture to the tangent space
//         double diff = std::max(0., n * uniform_l);                                         // diffuse light intensity
//         vec3 r = (n * (n * uniform_l) * 2 - uniform_l).normalized();                       // reflected light direction, specular mapping is described here: https://github.com/ssloy/tinyrenderer/wiki/Lesson-6-Shaders-for-the-software-renderer
//         double spec = std::pow(std::max(-r.z, 0.), 5 + sample2D(model.specular(), uv)[0]); // specular intensity, note that the camera lies on the z-axis (in view), therefore simple -r.z

//         TGAColor c = sample2D(model.diffuse(), uv);
//         for (int i : {0, 1, 2})
//             gl_FragColor[i] = std::min<int>(10 + c[i] * (diff + spec), 255); // (a bit of ambient light, diff + spec), clamp the result

//         return false; // the pixel is not discarded
//     }
// };

// int main(int argc, char** argv) {
//     if (2>argc) {
//         std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
//         return 1;
//     }
//     TGAImage framebuffer(width, height, TGAImage::RGB); // the output image
//     lookat(eye, center, up);                            // build the ModelView matrix
//     viewport(width/8, height/8, width*3/4, height*3/4); // build the Viewport matrix
//     projection((eye-center).norm());                    // build the Projection matrix
//     std::vector<double> zbuffer(width*height, std::numeric_limits<double>::max());

//     for (int m=1; m<argc; m++) { // iterate through all input objects
//         Model model(argv[m]);
//         Shader shader(model);
//         for (int i=0; i<model.nfaces(); i++) { // for every triangle
//             vec4 clip_vert[3]; // triangle coordinates (clip coordinates), written by VS, read by FS
//             for (int j : {0,1,2})
//                 shader.vertex(i, j, clip_vert[j]); // call the vertex shader for each triangle vertex
//             triangle(clip_vert, shader, framebuffer, zbuffer); // actual rasterization routine call
//         }
//     }
//     framebuffer.write_tga_file("framebuffer.tga");

//     return 0;
// }

/*
lesson 4: Perspective Projection

Translation in 2D is not linear because
[x y] + [e f]

in 3D is linear:
[0 0 e]
[0 0 f] [x y 1] = [e f 1]
[0 0 1]

*/

Matrix viewport(int x, int y, int w, int h)
{
    Matrix m = Matrix::identity();
    m[0][3] = x + w / 2.f;
    m[1][3] = y + h / 2.f;
    m[2][3] = depth / 2.f;

    m[0][0] = w / 2.f;
    m[1][1] = h / 2.f;
    m[2][2] = depth / 2.f;
    return m;
}

Vec3f m2v(Matrix41 m)
{
    return Vec3f(int(m[0][0] / m[3][0]), int(m[1][0] / m[3][0]), int(m[2][0] / m[3][0]));
}

Matrix41 v2m(Vec3f v)
{
    Matrix41 m;

    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = 1.f;
    return m;
}

int main(int argc, char **argv)
{
    int width = 1024;
    int height = 1024;
    TGAImage framebuffer(width, height, TGAImage::RGB);
    TGAColor grayColor;
    grayColor[0] = 255;
    grayColor[1] = 255;
    grayColor[2] = 255;

    TGAColor redColor;
    redColor[0] = 0;
    redColor[1] = 0;
    redColor[2] = 255;
    redColor[3] = 100;

    TGAColor randColor = {(std::uint8_t)(rand() % 255), (std::uint8_t)(rand() % 255), (std::uint8_t)(rand() % 255), 255};

    // line(10, 0, 50, 50, framebuffer, color);

    // line(13, 20, 80, 40, framebuffer, redColor);

    Model model(argv[1]);
    // for (int i = 0; i < model.nfaces(); i++)
    // {
    //     std::vector<int> face = model.face(i);
    //     for (int j = 0; j < 3; j++)
    //     {
    //         Vec3f v0 = model.vert(face[j]);
    //         Vec3f v1 = model.vert(face[(j + 1) % 3]);

    //         int x0 = (v0.x + 1.) * width / 2.;
    //         int y0 = (v0.y + 1.) * height / 2.;
    //         int x1 = (v1.x + 1.) * width / 2.;
    //         int y1 = (v1.y + 1.) * height / 2.;
    //         line3(x0, y0, x1, y1, framebuffer, grayColor);
    //     }
    // }

    Draw drawman;

    // drawman.line(10, 0, 50, 50, framebuffer, grayColor);

    Vec2i t0 = Vec2i(10, 90);
    Vec2i t1 = Vec2i(70, 10);
    Vec2i t2 = Vec2i(80, 40);

    // drawman.triangle2(t0, t1, t2, framebuffer, redColor);

    // Vec2i pts[3] = {t0, t1, t2};
    // drawman.triangle(pts, framebuffer, redColor);
    // drawman.triangle(t0, t1, t2, framebuffer, grayColor);

    Vec3f light_dir(0, 0, -1);
    Vec3f camera(0, 0, 10);

    TGAImage texture;
    texture.read_tga_file(argv[2]);
    texture.flip_vertically();

    float zbuffer[width * height];
    for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max())
        ;

    Matrix Projection = Matrix::identity();
    Matrix ViewPort = viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
    Projection[3][2] = -1.f / camera.z;

    for (int i = 0; i < model.nfaces(); i++)
    {
        std::vector<std::vector<int>> face = model.face(i);
        Vec3f world_coords[3];
        Vec3f screen_coords[3];
        Vec2f world_texture_coords[3];
        Vec2f screen_texture_coords[3];

        for (int j = 0; j < 3; j++)
        {
            world_coords[j] = model.vert(face[0][j]);
            world_texture_coords[j] = model.texture(face[1][j]);

            screen_coords[j] = m2v(ViewPort * Projection * v2m(world_coords[j]));
            // screen_coords[j] = Vec3f(int((world_coords[j].x + 1) * width / 2), int((world_coords[j].y + 1) * height / 2), world_coords[j].z);

            screen_texture_coords[j] = Vec2f(int(texture.width() * world_texture_coords[j].x), int(texture.height() * world_texture_coords[j].y));
        }

        Vec3f n = cross((world_coords[2] - world_coords[0]), (world_coords[1] - world_coords[0]));

        n.normalize();

        float intensity = n * light_dir;
        if (intensity > 0)
        {

            TGAColor randColor = {(std::uint8_t)(255 * intensity), (std::uint8_t)(255 * intensity), (std::uint8_t)(255 * intensity), 255};

            // std::vector<TGAColor> colors;

            // for (int j = 0; j < 3; j++)
            // {
            //     colors.push_back(texture.get(int(texture.width() * world_texture_coords[j].x), int(texture.height() * world_texture_coords[j].y)));
            // }

            // drawman.triangle(screen_coords, zbuffer, framebuffer, grayColor);
            drawman.triangle(screen_coords, zbuffer, framebuffer, texture, screen_texture_coords);
        };
    }

    bool success = framebuffer.write_tga_file("line.tga");

    printf("write........ %d", success);

    return 0;
}
