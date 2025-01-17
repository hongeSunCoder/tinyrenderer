#pragma once
#include <cstdint>
#include <fstream>
#include <vector>

#pragma pack(push, 1)
struct TGAHeader
{
    std::uint8_t idlength = 0;
    std::uint8_t colormaptype = 0;
    std::uint8_t datatypecode = 0;
    std::uint16_t colormaporigin = 0;
    std::uint16_t colormaplength = 0;
    std::uint8_t colormapdepth = 0;
    std::uint16_t x_origin = 0;
    std::uint16_t y_origin = 0;
    std::uint16_t width = 0;
    std::uint16_t height = 0;
    std::uint8_t bitsperpixel = 0;
    std::uint8_t imagedescriptor = 0;
};
#pragma pack(pop)

struct TGAColor
{
    // TGAColor(std::uint8_t b, std::uint8_t g, std::uint8_t r);
    unsigned char bgra[4] = {0, 0, 0, 0};
    unsigned char bytespp = 4;
    std::uint8_t &operator[](const int i) { return bgra[i]; }

    TGAColor() {}

    // TGAColor(unsigned char bpp) : bytespp(bpp)
    // {
    // }

    TGAColor(unsigned char v) : bytespp(1)
    {
        bgra[0] = v;
    }

    TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A = 255)
    {
        bgra[0] = B;
        bgra[1] = G;
        bgra[2] = R;
        bgra[3] = A;
    }

    TGAColor(const unsigned char *p, unsigned char bpp) : bgra(), bytespp(bpp)
    {
        for (int i = 0; i < (int)bpp; i++)
        {
            bgra[i] = p[i];
        }
        for (int i = bpp; i < 4; i++)
        {
            bgra[i] = 0;
        }
    }
    TGAColor operator*(float intensity) const
    {

        TGAColor res = *this;
        intensity = (intensity > 1.f ? 1.f : intensity < 0.f ? 0.f
                                                             : intensity);
        for (int i = 0; i < 4; i++)
            res.bgra[i] = bgra[i] * intensity;
        return res;
    }
};

struct TGAImage
{
    enum Format
    {
        GRAYSCALE = 1,
        RGB = 3,
        RGBA = 4
    };

    TGAImage() = default;
    TGAImage(const int w, const int h, const int bpp);
    bool read_tga_file(const std::string filename);
    bool write_tga_file(const std::string filename, const bool vflip = true, const bool rle = true) const;
    void flip_horizontally();
    void flip_vertically();
    TGAColor get(const int x, const int y) const;
    void set(const int x, const int y, const TGAColor &c);
    int width() const;
    int height() const;

private:
    bool load_rle_data(std::ifstream &in);
    bool unload_rle_data(std::ofstream &out) const;

    int w = 0;
    int h = 0;
    std::uint8_t bpp = 0;
    std::vector<std::uint8_t> data = {};
};
