#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cassert>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

uint32_t pack_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a=255)
{
    return (a<<24) + (b<<16) + (g<<8) + r;
}

void unpack_color(const uint32_t &color, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a)
{
    r = (color >>  0) & 255;
    g = (color >>  8) & 255;
    b = (color >> 16) & 255;
    a = (color >> 24) & 255;
}

void draw_rectangle(std::vector<uint32_t> &img, const size_t img_w,
        const size_t img_h, const size_t x, const size_t y,
        const size_t w, const size_t h, const uint32_t color)
{
    assert(img.size() == img_w * img_h);
    for(size_t i = 0; i<w; ++i) {
        for(size_t j = 0; j<h; ++j) {
            size_t cx = x + i;
            size_t cy = y + j;
            if(cx >= img_w || cy >= img_h) continue;
            img[cx + cy*img_w] = color;
        }
    }
}

void drop_ppm_image(const std::string filename,
        const std::vector<uint32_t> &image, const size_t w, const size_t h)
{
    assert(image.size() == w * h);
    std::ofstream ofs(filename, std::ios::binary);
    ofs << "P6\n" << w << " " << h << "\n255\n";
    for(size_t i = 0; i < h*w; ++i) {
        uint8_t r, g, b, a;
        unpack_color(image[i], r, g, b, a);
        ofs << static_cast<char>(r) << static_cast<char>(g) 
            << static_cast<char>(b); 
    }
    ofs.close();
}

bool load_texture(const std::string filename, std::vector<uint32_t> &texture, 
        size_t &txt_size, size_t &txt_cnt)
{
    int nchannels = -1, w, h;
    unsigned char *pixmap = stbi_load(filename.c_str(), &w, &h, &nchannels, 0);
    if(!pixmap) {
        std::cerr << "Error: could not load the textures" <<std::endl;
        return false;
    }
    if(4 != nchannels) {
        std::cerr << "Error: the texture must be a 32 bit image" << std::endl;
        stbi_image_free(pixmap);
        return false;
    }

    txt_cnt = w/h;
    txt_size = w/txt_cnt;
    if(w != h * int(txt_cnt)) {
        std::cerr << "Error: the texture file must contain N square textures"
            << "packed horizontally" << std::endl;
        stbi_image_free(pixmap);
        return false;
    }

    texture = std::vector<uint32_t>(w * h);
    for(int j = 0; j<h; ++j) {
        for(int i = 0; i<w; ++i) {
            uint8_t r = pixmap[(i+j*w)*4+0];
            uint8_t g = pixmap[(i+j*w)*4+1];
            uint8_t b = pixmap[(i+j*w)*4+2];
            uint8_t a = pixmap[(i+j*w)*4+3];
            texture[i+j*w] = pack_color(r, g, b, a);
        }
    }
    stbi_image_free(pixmap);

    return true;
}

std::vector<uint32_t> texture_column(const std::vector<uint32_t> &img,
        const size_t texsize, const size_t ntextures, const size_t texid,
        const size_t texcoord, const size_t column_height) {
    const size_t img_w = texsize * ntextures;
    const size_t img_h = texsize;
    assert(img.size()==img_w*img_h && texcoord < texsize && texid < ntextures);
    std::vector<uint32_t> column(column_height);
    for (size_t y = 0; y<column_height; ++y) {
        size_t pix_x = texid*texsize + texcoord;
        size_t pix_y = (y * texsize) / column_height;
        column[y] = img[pix_x + pix_y*img_w];
    }
    return column;
}

int main()
{
    const size_t win_w = 1024; 
    const size_t win_h = 512; 
    std::vector<uint32_t> framebuffer(win_w*win_h, pack_color(255, 255, 255));

    const size_t map_w = 16;
    const size_t map_h = 16;
    const char map[] = "0000222222220000"\
                        "1              0"\
                        "1      11111   0"\
                        "1     0        0"\
                        "0     0  1110000"\
                        "0     3        0"\
                        "0   10000      0"\
                        "5   3   11100  0"\
                        "5   4   0      0"\
                        "0   4   1  00000"\
                        "0       1      0"\
                        "2       1      0"\
                        "0       0      0"\
                        "0 0000000      0"\
                        "0              0"\
                        "0002222222200000";
    assert(sizeof(map) == map_w * map_h + 1);

    float player_x = 3.426;
    float player_y = 2.345;
    float player_a = 1.58353;
    float fov = M_PI / 3;

    std::vector<uint32_t> walltxt;
    size_t walltxt_size;
    size_t walltxt_cnt;
    if(!load_texture("./walltext.png", walltxt, walltxt_size, walltxt_cnt)) {
        std::cerr << "Error: Failed to load wall textures" << std::endl;
        return -1;
    }

    const size_t rect_w = win_w / (map_w * 2);
    const size_t rect_h = win_h / map_w;

    // draw the map
    for(size_t j=0; j<map_h; ++j) {
        for(size_t i=0; i<map_w; ++i) {
            if(map[i+j*map_w] == ' ') continue;
            size_t rect_x = rect_w * i;
            size_t rect_y = rect_h * j;
            size_t texid = map[i + j*map_w] - '0';
            assert(texid < walltxt_cnt);
            draw_rectangle(framebuffer, win_w, win_h, rect_x, rect_y, 
                    rect_w, rect_h, walltxt[texid*walltxt_size]);
        }
    }

    // draw the player
    draw_rectangle(framebuffer, win_w, win_h, player_x * rect_w,
            player_y * rect_h, 5, 5, 255);

    // draw range and 3D view
    uint32_t color_range = pack_color(160, 160, 160);
    for (size_t i=0; i<win_w/2; i++) {
        float angle = player_a-fov/2 + fov*i/float(win_w/2);
        for (float t=0; t<20; t+=.01) {
            float cx = player_x + t*cos(angle);
            float cy = player_y + t*sin(angle);

            int pix_x = cx * rect_w;
            int pix_y = cy * rect_h;
            framebuffer[pix_x + pix_y*win_w] = color_range;
            
            if(map[int(cx) + int(cy)*map_w] != ' ') {
                size_t texid = map[int(cx)+int(cy)*map_w] - '0';
                assert(texid < walltxt_cnt);
                size_t column_height = win_h / (t * cos(angle-player_a));

                float hitx = cx - floor(cx+.5);
                float hity = cy - floor(cy+.5);
                int x_texcoord = hitx * walltxt_size;
                if(std::abs(hity) > std::abs(hitx)) {
                    x_texcoord = hity * walltxt_size;
                }
                if(x_texcoord < 0) x_texcoord += walltxt_size;
                assert(x_texcoord>=0 && x_texcoord<(int)walltxt_size);

                std::vector<uint32_t> column = texture_column(walltxt, 
                        walltxt_size, walltxt_cnt, texid, x_texcoord, column_height);
                pix_x = win_w / 2 + i;
                for(size_t j = 0; j<column_height; j++) {
                    pix_y = j + win_h/2 - column_height/2;
                    if (pix_y < 0 || pix_y >= (int)win_h) continue;
                    framebuffer[pix_x + pix_y*win_w] = column[j];
                }
                break;
            }
        }
    }

    drop_ppm_image("./out.ppm", framebuffer, win_w, win_h);

    return 0;
}
