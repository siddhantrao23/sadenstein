#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <cassert>

#include "utils.h"
#include "ray_caster.h"

int wall_x_texcoord(const float hitx, const float hity, const Texture &tex_walls)
{
    float x = hitx - floor(hitx + .5);
    float y = hity - floor(hity + .5);
    int tex = x * tex_walls.size;
    if(std::abs(y) > std::abs(x)) {
        tex = y * tex_walls.size;
    }
    if(tex < 0)
        tex += tex_walls.size;
    assert(tex >= 0 && tex < (int)tex_walls.size);

    return tex;
}

void draw_map(FrameBuffer &fb, const std::vector<Sprite> &sprites, const Texture &tex_walls, 
        const Map &map, const size_t cell_w, const size_t cell_h)
{
    for(size_t j=0; j<map.h; ++j) {
        for(size_t i=0; i<map.w; ++i) {
            if(map.is_empty(i, j)) continue;
            size_t rect_x = i*cell_w;
            size_t rect_y = j*cell_h;
            size_t texid = map.get(i, j);
            assert(texid < tex_walls.count);
            fb.draw_rectangle(rect_x, rect_y, cell_w, cell_h, tex_walls.get(0, 0, texid));
        }
    }
    for(size_t i=0; i<sprites.size(); ++i) {
        fb.draw_rectangle(sprites[i].x*cell_w - 3, sprites[i].y*cell_h - 3, 6, 6, pack_color(255, 0, 0));
    }
}

void draw_sprite(FrameBuffer fb, const Sprite &sprite, const std::vector<float> &depth_buffer, const Player &player, const Texture &tex_sprites)
{
    // absolute direction from the player to the sprite (in radians)
    float sprite_dir = atan2(sprite.y - player.y, sprite.x - player.x);
    while (sprite_dir - player.a > M_PI)
        sprite_dir -= 2 * M_PI; // remove unncesessary periods from the relative direction
    while (sprite_dir - player.a < -M_PI)
        sprite_dir += 2 * M_PI;

    size_t sprite_screen_size = std::min(1000, static_cast<int>(fb.h / sprite.player_dist));                  // screen sprite size
    int h_offset = (sprite_dir - player.a) / player.fov * (fb.w / 2) + (fb.w / 2) / 2 - tex_sprites.size / 2; // do not forget the 3D view takes only a half of the framebuffer
    int v_offset = fb.h / 2 - sprite_screen_size / 2;

    for (size_t i = 0; i < sprite_screen_size; i++) {
        if (h_offset + int(i) < 0 || h_offset + i >= fb.w / 2)
            continue;
        if (depth_buffer[h_offset + i] < sprite.player_dist)
            continue; // this sprite column is occluded
        for (size_t j = 0; j < sprite_screen_size; j++) {
            if (v_offset + int(j) < 0 || v_offset + j >= fb.h)
                continue;
            uint32_t color = tex_sprites.get(i * tex_sprites.size / sprite_screen_size, j * tex_sprites.size / sprite_screen_size, sprite.tex_id);
            uint8_t r, g, b, a;
            unpack_color(color, r, g, b, a);
            if (a > 128)
                fb.set_pixel(fb.w / 2 + h_offset + i, v_offset + j, color);
        }
    }
}

void render(FrameBuffer &fb, const GameState &gs)
{
    const Map &map = gs.map;
    const Player &player = gs.player;
    const std::vector<Sprite> &sprites = gs.monsters;
    const Texture &tex_walls = gs.tex_walls;
    const Texture &tex_monst = gs.tex_monst;
    fb.clear(pack_color(255, 255, 255));

    // for drawing the map
    const size_t cell_w = fb.w / (map.w * 2);
    const size_t cell_h = fb.h / map.h;
    std::vector<float> depth_buffer(fb.w / 2, 1e3);

    // for drawing visibility cone and 3d render
    for (size_t i = 0; i < fb.w / 2; i++) {
        float angle = player.a - player.fov / 2 + player.fov * i / float(fb.w / 2);
        for (float t = 0; t < 20; t += .01) {
            float x = player.x + t*cos(angle);
            float y = player.y + t*sin(angle);
            fb.set_pixel(x * cell_w, y * cell_h, pack_color(160, 160, 160));

            if (map.is_empty(x, y))
                continue;

            size_t texid = map.get(x, y);
            assert(texid < tex_walls.count);
            float dist = t * cos(angle - player.a);
            depth_buffer[i] = dist;
            size_t column_height = fb.h / dist;
            int x_texcoord = wall_x_texcoord(x, y, tex_walls);
            std::vector<uint32_t> column = tex_walls.get_scaled_column(texid,
                                                                       x_texcoord, column_height);
            int pix_x = i + fb.w / 2;
            for (size_t j = 0; j < column_height; j++) {
                int pix_y = j + fb.h / 2 - column_height / 2;
                if (pix_y >= 0 && pix_y < (int)fb.h) {
                    fb.set_pixel(pix_x, pix_y, column[j]);
                }
            }
            break;
        }
    }

    draw_map(fb, sprites, tex_walls, map, cell_w, cell_h);

    for(size_t i = 0; i<sprites.size(); i++) {
        draw_sprite(fb, sprites[i], depth_buffer, player, tex_monst);
    }
}

/*
int main()
{
    FrameBuffer fb{1024, 512, std::vector<uint32_t>(1024 * 512, pack_color(255, 255, 255))};
    Player player{3.456, 2.345, 1.523, M_PI / 3.};
    Map map;
    Texture tex_walls("../textures/walltext.png");
    Texture tex_monst("../textures/monsters.png");
    if (!tex_walls.count || !tex_monst.count)
    {
        std::cerr << "Error: Failed to load textures" << std::endl;
        return -1;
    }
    std::vector<Sprite> sprites{{3.523, 3.812, 2, 0},
                                {1.834, 8.765, 0, 0},
                                {5.323, 5.365, 1, 0},
                                {4.123, 10.265, 1, 0}};

#if 0
    for (size_t frame=0; frame<360; frame++) {
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(5) << frame << ".ppm";
        player.a += 2*M_PI/360;

        render(fb, map, player, sprites, tex_walls, tex_monst);
        drop_ppm_image(ss.str(), fb.img, fb.w, fb.h);
    }
#endif

    render(fb, map, player, sprites, tex_walls, tex_monst);
    drop_ppm_image("./out.ppm", fb.img, fb.w, fb.h);

    return 0;
}
*/
