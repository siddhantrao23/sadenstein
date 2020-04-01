#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdint>
#include <cassert>
#include <sstream>
#include <iomanip>

#include "map.h"
#include "utils.h"
#include "player.h"
#include "framebuffer.h"
#include "textures.h"

int wall_x_texcoord(const float x, const float y, Texture &tex_walls)
{
    float hitx = x - floor(x+.5);
    float hity = y - floor(y+.5);
    int tex = hitx * tex_walls.size;
    if(std::abs(hity) > std::abs(hitx)) {
        tex = hity * tex_walls.size;
    }
    if(tex < 0) tex += tex_walls.size;
    assert(tex >= 0 && tex < (int)tex_walls.size);
    
    return tex;
}

void render(FrameBuffer &fb, Map &map, Player &player, Texture &tex_walls)
{
    fb.clear(pack_color(255, 255, 255));
    const size_t rect_w = fb.w / (map.w*2);
    const size_t rect_h = fb.h / map.h;
    for(size_t j = 0; j<map.h; ++j) {
        for(size_t i = 0; i<map.w; ++i) {
            if(map.is_empty(i, j)) continue;
            size_t rect_x = i * rect_w;
            size_t rect_y = j * rect_h;
            size_t texid = map.get(i, j);
            assert(texid < tex_walls.count);
            fb.draw_rectangle(rect_x, rect_y, rect_w, rect_h,
                    tex_walls.get(0, 0, texid));
        }
    }
    for (size_t i=0; i<fb.w/2; i++) {        
        float angle = player.a-player.fov/2 + player.fov*i/float(fb.w/2);
        for (float t=0; t<20; t+=.01) { // ray marching loop
            float x = player.x + t*cos(angle);
            float y = player.y + t*sin(angle);
            fb.set_pixel(x*rect_w, y*rect_h, pack_color(160, 160, 160));
            if (map.is_empty(x, y)) continue;
            size_t texid = map.get(x, y);
            assert(texid<tex_walls.count);
            size_t column_height = fb.h/(t*cos(angle-player.a));
            int x_texcoord = wall_x_texcoord(x, y, tex_walls);
            std::vector<uint32_t> column = tex_walls.get_scaled_column(texid,
                   x_texcoord, column_height);
            int pix_x = i + fb.w/2;
            for (size_t j=0; j<column_height; j++) {
                int pix_y = j + fb.h/2 - column_height/2;
                if (pix_y>=0 && pix_y<(int)fb.h) {
                    fb.set_pixel(pix_x, pix_y, column[j]);
                }
            }
            break;
        }
    }
}

int main()
{
    FrameBuffer fb{1024, 512, std::vector<uint32_t>(1024*512,
        pack_color(255, 255, 255))};
    Player player {3.456, 2.345, 1.523, M_PI/3.};
    Map map;
    Texture tex_walls("./walltext.png");
    if(!tex_walls.count) {
        std::cerr << "Error: Failed to load wall textures" << std::endl;
        return -1;
    }

    for(size_t frame = 0; frame<360; ++frame) {
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(5) << frame << ".ppm";
        player.a += 2 * M_PI / 360;

        render(fb, map, player, tex_walls);
        drop_ppm_image(ss.str(), fb.img, fb.w, fb.h);
    }

    return 0;
}
