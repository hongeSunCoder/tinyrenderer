#include "our_gl.h"
#include "../lesson1/model.h"

Model *model = NULL;
float *shadowbuffer = NULL;
Vec3f light_dir(1, 1, 0);
Vec3f eye(1, 1, 3);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);
const int width = 800;
const int height = 800;

struct GouraudShader : public IShader
{

    mat<2, 3, float> varying_uv;
    mat<3, 3, float> varying_tri;

    mat<4, 4, float> uniform_M;       // Projection*ModelView
    mat<4, 4, float> uniform_MIT;     // Projection*ModelView*invert_transpose()
    mat<4, 4, float> uniform_Mshadow; //

    GouraudShader(Matrix M, Matrix MIT, Matrix MS) : uniform_M(M), uniform_MIT(MIT), uniform_Mshadow(MS), varying_uv(), varying_tri() {}

    virtual Vec4f vertex(int iface, int nthvert)
    {

        varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        Vec4f gl_Vertex = Viewport * Projection * ModelView * embed<4>(model->vert(iface, nthvert));
        varying_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor &color)
    {

        Vec4f sb_p = uniform_Mshadow * embed<4>(varying_tri * bar);
        sb_p = sb_p / sb_p[3];

        int idx = int(sb_p[0]) + int(sb_p[1]) * width; // index in the shadowbuffer array
        float shadow = .3 + .7 * (shadowbuffer[idx] < sb_p[2] + 43.34);

        Vec2f uv = varying_uv * bar;
        Vec3f n = proj<3>(uniform_MIT * embed<4>(model->normal(uv))).normalize();
        Vec3f l = proj<3>(uniform_M * embed<4>(light_dir)).normalize();
        // reflected light
        Vec3f r = (n * (n * l * 2.f) - l).normalize();

        float spec = pow(std::max(r.z, 0.0f), model->specular(uv));

        float diff = std::max(0.f, n * l);

        color = model->diffuse(uv);
        for (int i = 0; i < 3; i++)
            color[i] = std::min<float>(5 + color[i] * shadow * (1.2 * diff + .6 * spec), 255);
        return false;
    }
};

struct DepthShader : public IShader
{
    mat<3, 3, float> varying_tri;

    DepthShader() : varying_tri() {}

    virtual Vec4f vertex(int iface, int nthvert)
    {
        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));
        gl_Vertex = Viewport * Projection * ModelView * gl_Vertex;

        varying_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor &color)
    {
        Vec3f p = varying_tri * bar;
        color = TGAColor(255, 255, 255) * (p.z / depth);
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
    float *zbuffer = new float[width * height];
    shadowbuffer = new float[width * height];
    for (int i = width * height; --i;)
    {
        zbuffer[i] = shadowbuffer[i] = -std::numeric_limits<float>::max();
    }

    light_dir.normalize();

    {
        TGAImage depth(width, height, TGAImage::RGB);
        lookat(light_dir, center, up);
        viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
        projection(0);

        DepthShader depthShader;
        Vec4f screen_coords[3];
        for (int i = 0; i < model->nfaces(); i++)
        {
            for (int j = 0; j < 3; j++)
            {
                screen_coords[j] = depthShader.vertex(i, j);
            }

            triangle(screen_coords, depthShader, depth, shadowbuffer);
        }

        depth.write_tga_file("depth.tga");
    }
    Matrix M = Viewport * Projection * ModelView;
    {

        lookat(eye, center, up);
        viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
        projection(-1.f / (eye - center).norm());
        TGAImage image(width, height, TGAImage::RGB);

        GouraudShader shader(ModelView, (Projection * ModelView).invert_transpose(), M * (Viewport * Projection * ModelView).invert());
        Vec4f screen_coords[3];
        for (int i = 0; i < model->nfaces(); i++)
        {
            for (int j = 0; j < 3; j++)
            {
                screen_coords[j] = shader.vertex(i, j);
            }
            triangle(screen_coords, shader, image, zbuffer);
        }

        image.write_tga_file("output.tga");
    }

    delete model;
    delete[] zbuffer;
    delete[] shadowbuffer;
    return 0;
}