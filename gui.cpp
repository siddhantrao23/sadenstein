#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <cmath>
#include "SDL.h"

#include "utils.h"
#include "ray_caster.h"

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr <<"Couldn't initialize SDL: "<< SDL_GetError() << std::endl;
        return -1;
    }

    FrameBuffer fb{1024, 512, std::vector<uint32_t>(1024*512,
            pack_color(255, 255, 255))};

    GameState gs {
        Map(),                              // game map
        {3, 2, 1.523, M_PI/3., 0, 0},       // player
        { {3.523, 3.812, 2, 0},             // monsters
          {1.834, 8.765, 0, 0},
          {5.323, 5.365, 1, 0},
          {14.32, 13.36, 3, 0},
          {4.123, 10.76, 1, 0}
        },
        Texture("../textures/walltext.bmp", SDL_PIXELFORMAT_ABGR8888),
        Texture("../textures/monsters.bmp", SDL_PIXELFORMAT_ABGR8888)
    };
    if(!gs.tex_walls.count || !gs.tex_monst.count) {
        std::cerr << "Failed to load textures" << std::endl;
        return -1;
    }

    render(fb, gs);

    SDL_Window   *window   = nullptr;
    SDL_Renderer *renderer = nullptr;


    if (SDL_CreateWindowAndRenderer(fb.w, fb.h, SDL_WINDOW_SHOWN |
                SDL_WINDOW_INPUT_FOCUS, &window, &renderer)) {
        std::cerr << "Couldn't create window and renderer: " 
            << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Texture *framebuffer_texture = SDL_CreateTexture(renderer,
            SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, fb.w, fb.h);
    if (!framebuffer_texture) {
        std::cerr << "Failed to create framebuffer texture : "
            << SDL_GetError() << std::endl;
        return -1;
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    while (1) {
        {
            auto t2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> fp_ms = t2 - t1;
            if (fp_ms.count()<20) {
                std::this_thread::sleep_for(std::chrono::milliseconds(3));
                continue;
            }
            t1 = t2;
        }

        {
            SDL_Event event;
            if(SDL_PollEvent(&event)) {
                if (SDL_QUIT==event.type || (SDL_KEYDOWN==event.type &&
                            SDLK_ESCAPE==event.key.keysym.sym)) break;
                if (SDL_KEYUP==event.type) {
                    if ('a'==event.key.keysym.sym ||
                            'd'==event.key.keysym.sym) gs.player.turn = 0;
                    if ('w'==event.key.keysym.sym ||
                            's'==event.key.keysym.sym) gs.player.walk = 0;
                }
                if (SDL_KEYDOWN==event.type) {
                    if ('a'==event.key.keysym.sym) gs.player.turn = -1;
                    if ('d'==event.key.keysym.sym) gs.player.turn =  1;
                    if ('w'==event.key.keysym.sym) gs.player.walk =  1;
                    if ('s'==event.key.keysym.sym) gs.player.walk = -1;
                }
            }
        }

        { 
            gs.player.a += float(gs.player.turn)*.05;
            float nx = gs.player.x + gs.player.walk*cos(gs.player.a)*.05;
            float ny = gs.player.y + gs.player.walk*sin(gs.player.a)*.05;

            if (int(nx)>=0 && int(nx)<int(gs.map.w) && int(ny)>=0 &&
                    int(ny)<int(gs.map.h)) {
                if (gs.map.is_empty(nx, gs.player.y)) gs.player.x = nx;
                if (gs.map.is_empty(gs.player.x, ny)) gs.player.y = ny;
            }
            for (size_t i=0; i<gs.monsters.size(); i++) {
                gs.monsters[i].player_dist = std::sqrt(
                        pow(gs.player.x - gs.monsters[i].x, 2) +
                        pow(gs.player.y - gs.monsters[i].y, 2));
            }
            std::sort(gs.monsters.begin(), gs.monsters.end());
        }

        render(fb, gs);

        {
            SDL_UpdateTexture(framebuffer_texture, NULL,
                    reinterpret_cast<void *>(fb.img.data()), fb.w*4);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, framebuffer_texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
    }

    SDL_DestroyTexture(framebuffer_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
