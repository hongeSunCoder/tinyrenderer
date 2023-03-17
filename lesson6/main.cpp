#include "our_gl.h"
#include "../lesson1/model.h"

Model *model = NULL;
Vec3f light_dir(1, 1, 1);
Vec3f eye(1, 1, 3);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);
const int width = 800;
const int height = 800;

struct GouraudShader : public IShader
{
    Vec3f varying_intensity;
    mat<2, 3, float> varying_uv;

    mat<4, 4, float> uniform_M;   // Projection*ModelView
    mat<4, 4, float> uniform_MIT; // Projection*ModelView*invert_transpose()

    virtual Vec4f vertex(int iface, int nthvert)
    {
        varying_intensity[nthvert] = std::max(0.f, model->normal(iface, nthvert) * light_dir);
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));

        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));

        return Viewport * Projection * ModelView * gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor &color)
    {

        // float intensity = varying_intensity * bar;

        Vec2f uv = varying_uv * bar;

        Vec3f n = proj<3>(uniform_MIT * embed<4>(model->normal(uv))).normalize();

        Vec3f l = proj<3>(uniform_M * embed<4>(light_dir)).normalize();

        // reflected light
        Vec3f r = (n * (n * l * 2.f) - l).normalize();

        float spec = pow(std::max(r.z, 0.0f), model->specular(uv));

        float diff = std::max(0.f, n * l);

        color = model->diffuse(uv);
        for (int i = 0; i < 3; i++)
            color[i] = std::min<float>(5 + color[i] * (1 * diff + .0 * spec), 255);
        return false;
    }
};

int main(int argc, char **argv)
{

    if (2 == argc)
    {
        model = new Model(argv[1]);
    }
    else
    {
        model = new Model("../obj/african_head/african_head.obj");
    }

    lookat(eye, center, up);
    viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
    projection(-1.f / (eye - center).norm());
    light_dir.normalize();

    TGAImage image(width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

    GouraudShader shader;
    shader.uniform_M = Projection * ModelView;
    shader.uniform_MIT = (Projection * ModelView).invert_transpose();
    for (int i = 0; i < model->nfaces(); i++)
    {
        Vec4f screen_coords[3];
        for (int j = 0; j < 3; j++)
        {
            screen_coords[j] = shader.vertex(i, j);
        }
        triangle(screen_coords, shader, image, zbuffer);
    }

    // image.flip_vertically(); // to place the origin in the bottom left corner of the image
    // zbuffer.flip_vertically();
    image.write_tga_file("output.tga");
    zbuffer.write_tga_file("zbuffer.tga");

    delete model;
    return 0;
}