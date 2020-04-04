#ifndef SPRITE_H
#define SPRITE_H

#include <cstdlib>

struct Sprite {
    float x, y;     // coordinates of the sprite in 2d
    size_t tex_id;  // which monster
    float player_dist;
    bool operator < (const Sprite& s) const;
};

#endif
