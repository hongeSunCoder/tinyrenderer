
#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "../lesson3/geometry.h"
#include "../tgaimage/tgaimage.h"

class Model
{
private:
    std::vector<Vec3f> verts_;
    std::vector<std::vector<Vec3i>> faces_;
    std::vector<Vec2f> textures_;
    std::vector<Vec3f> normals_;

    TGAImage diffusemap_;
    TGAImage normalmap_;
    TGAImage specularmap_;

    void load_texture(std::string filename, const char *suffix, TGAImage &img);

public:
    Model(const char *filename);
    ~Model();

    int nverts();
    int nfaces();
    int ntextures();

    Vec3f vert(int i);
    Vec3f vert(int iface, int nthvert);

    Vec2f texture(int i);
    Vec2f uv(int iface, int nthvert);

    TGAColor diffuse(Vec2f uvf);
    float specular(Vec2f uv);
    Vec3f normal(Vec2f uvf);

    Vec3f normal(int i);
    Vec3f normal(int iface, int nthvert);

    std::vector<Vec3i> face(int idx);
};

#endif //__MODEL_H__