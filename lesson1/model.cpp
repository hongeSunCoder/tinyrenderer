#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_(), textures_()
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
                iss >> v.raw[i];
            verts_.push_back(v);
        }
        else if (!line.compare(0, 2, "f "))
        {
            std::vector<std::vector<int>> f;
            f.push_back(std::vector<int>());
            f.push_back(std::vector<int>());
            int itrash, v_idx, t_idx;
            iss >> trash;
            while (iss >> v_idx >> trash >> t_idx >> trash >> itrash)
            {
                v_idx--; // in wavefront obj all indices start at 1, not zero
                t_idx--;
                f[0].push_back(v_idx);
                f[1].push_back(t_idx);
            }
            faces_.push_back(f);
        }
        else if (!line.compare(0, 3, "vt "))
        {
            iss >> trash;
            Vec2f t;
            for (int i = 0; i < 2; i++)
                iss >> t.raw[i];
            textures_.push_back(t);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << " vt# " << textures_.size() << std::endl;
}

Model::~Model()
{
}

int Model::nverts()
{
    return (int)verts_.size();
}

int Model::nfaces()
{
    return (int)faces_.size();
}

std::vector<std::vector<int>> Model::face(int idx)
{
    return faces_[idx];
}

Vec3f Model::vert(int i)
{
    return verts_[i];
}