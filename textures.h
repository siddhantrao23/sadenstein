#ifndef TEXTURES_H
#define TEXTURES_H

#include <cstdint>
#include <vector>
#include <string>

struct Texture {
    size_t img_w, img_h;
    size_t count, size;
    std::vector<uint32_t> img;

    Texture(const std::string filename, const uint32_t format);
    uint32_t get(const size_t i, const size_t j, const size_t idx) const;
    std::vector<uint32_t> get_scaled_column(const size_t texture_id,
            const size_t tex_coord, const size_t column_height) const;
};

#endif

