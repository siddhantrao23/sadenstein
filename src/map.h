#ifndef MAP_H
#define MAP_H

#include <cstdio>

struct Map {
    size_t w, h;    // map dimensions
    Map();

    int get(const size_t i, const size_t j) const;
    bool is_empty(const size_t i, const size_t j) const;
};

#endif

