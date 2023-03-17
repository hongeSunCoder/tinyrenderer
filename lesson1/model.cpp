#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_(), textures_(), normals_(), diffusemap_(), normalmap_(), specularmap_()
{
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail())
        return;
    std::string line;
    while (!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v "))
        {
            iss >> trash;
            Vec3f v;
            for (int i = 0; i < 3; i++)
            {

                iss >> v[i];
                // printf("read vertex coords: %f\n", v.raw[i]);
            }

            verts_.push_back(v);
        }
        else if (!line.compare(0, 2, "f "))
        {
            std::vector<Vec3i> f;

            Vec3i tmp;

            iss >> trash;
            while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2])
            {

                for (int i = 0; i < 3; i++) // in wavefront obj all indices start at 1, not zero
                    tmp[i]--;
                f.push_back(tmp);
            }
            faces_.push_back(f);
        }
        else if (!line.compare(0, 4, "vt  "))
        {
            iss >> trash >> trash;

            Vec2f t;
            for (int i = 0; i < 2; i++)
            {
                iss >> t[i];
            }

            textures_.push_back(t);
        }
        else if (!line.compare(0, 4, "vn  "))
        {
            iss >> trash >> trash;

            Vec3f n;
            for (int i = 0; i < 3; i++)
            {
                iss >> n[i];
            }

            normals_.push_back(n);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << " vt# " << textures_.size() << " vn# " << normals_.size() << std::endl;

    load_texture(filename, "_diffuse.tga", diffusemap_);
    load_texture(filename, "_nm.tga", normalmap_);
    load_texture(filename, "_spec.tga", specularmap_);
}

void Model::load_texture(std::string filename, const char *suffix, TGAImage &img)
{
    std::string texfile(filename);
    size_t dot = texfile.find_last_of(".");
    if (dot != std::string::npos)
    {
        texfile = texfile.substr(0, dot) + std::string(suffix);
        std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
        img.flip_vertically();
    }
}

Model::~Model()
{
}

TGAColor Model::diffuse(Vec2f uvf)
{
    Vec2i uv(uvf[0] * diffusemap_.width(), uvf[1] * diffusemap_.height());
    return diffusemap_.get(uv[0], uv[1]);
}

Vec3f Model::normal(Vec2f uvf)
{
    Vec2i uv(uvf[0] * normalmap_.width(), uvf[1] * normalmap_.height());
    TGAColor color = normalmap_.get(uv[0], uv[1]);

    Vec3f res;
    for (int i = 0; i < 3; i++)
        res[2 - i] = (float)color[i] / 255.f * 2.f - 1.f;
    return res;
}

float Model::specular(Vec2f uvf)
{
    Vec2i uv(uvf[0] * normalmap_.width(), uvf[1] * normalmap_.height());
    return specularmap_.get(uv[0], uv[1])[0] / 1.f;
}

int Model::nverts()
{
    return (int)verts_.size();
}

int Model::ntextures()
{
    return (int)textures_.size();
}

int Model::nfaces()
{
    return (int)faces_.size();
}

std::vector<Vec3i> Model::face(int idx)
{
    return faces_[idx];
}

Vec3f Model::vert(int i)
{
    return verts_[i];
}

Vec3f Model::vert(int iface, int nthvert)
{
    int vert_index = faces_[iface][nthvert][0];
    return vert(vert_index);
}

Vec2f Model::texture(int i)
{
    return textures_[i];
}

Vec2f Model::uv(int iface, int nthvert)
{
    int uv_index = faces_[iface][nthvert][1];

    return textures_[uv_index];
}

Vec3f Model::normal(int i)
{
    return normals_[i];
}

Vec3f Model::normal(int iface, int nthvert)
{
    int nor_index = faces_[iface][nthvert][2];
    return normals_[nor_index];
}
