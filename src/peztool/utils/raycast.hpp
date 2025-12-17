#pragma once
#include "peztool/utils/grid.hpp"
#include "peztool/utils/math.hpp"
#include "peztool/utils/vec.hpp"


struct Raycast
{
    struct Ray
    {
        Vec2f start = {};
        Vec2f direction = {};
    };

    struct RayResult
    {
        bool  hit{false};
        float dist{0.0f};
    };

    template<typename TCell>
    [[nodiscard]]
    static RayResult cast(pez::Grid<TCell> const& grid, Ray const ray, float const max_dist)
    {
        constexpr float eps{0.0001f};

        Vec2i cell_p{ray.start};
        const Vec2i step(ray.direction.x < 0.0f ? -1 : 1, ray.direction.y < 0.0f ? -1 : 1);
        const Vec2f inv_d(1.0f / (ray.direction.x == 0.0f ? eps : ray.direction.x), 1.0f / (ray.direction.y == 0.0f ? eps : ray.direction.y));
        const float t_dx = std::abs(inv_d.x);
        const float t_dy = std::abs(inv_d.y);
        float t_max_x = ((cell_p.x + (step.x > 0)) - ray.start.x) * inv_d.x;
        float t_max_y = ((cell_p.y + (step.y > 0)) - ray.start.y) * inv_d.y;
        const int32_t max_width  = grid.width;
        const int32_t max_height = grid.height;
        float dist = 0.0f;

        while (dist < max_dist) {
            // Advance in grid
            if (t_max_x < t_max_y) {
                dist = t_max_x;
                t_max_x += t_dx;
                cell_p.x += step.x;
            } else {
                dist = t_max_y;
                t_max_y += t_dy;
                cell_p.y += step.y;
            }

            if (cell_p.x >= 0 && cell_p.x < max_width && cell_p.y >= 0 && cell_p.y < max_height) {
                TCell const& cell = grid.get(cell_p);
                if (cell.wall) {
                    return {true, std::min(dist, max_dist)};
                }
            } else {
                break;
            }
        }

        return {false, max_dist};
    }

    template<typename TCell, typename TCallback>
    static void castCallback(pez::Grid<TCell> const& grid, Ray const ray, float const max_dist, TCallback&& callback)
    {
        float constexpr eps{0.0001f};

        Vec2i cell_p{ray.start};
        const Vec2i step(ray.direction.x < 0.0f ? -1 : 1, ray.direction.y < 0.0f ? -1 : 1);
        const Vec2f inv_d(1.0f / (ray.direction.x == 0.0f ? eps : ray.direction.x), 1.0f / (ray.direction.y == 0.0f ? eps : ray.direction.y));
        const float t_dx = std::abs(inv_d.x);
        const float t_dy = std::abs(inv_d.y);
        float t_max_x = ((cell_p.x + (step.x > 0)) - ray.start.x) * inv_d.x;
        float t_max_y = ((cell_p.y + (step.y > 0)) - ray.start.y) * inv_d.y;
        const int32_t max_width  = grid.width;
        const int32_t max_height = grid.height;
        float dist = 0.0f;

        while (dist < max_dist) {
            // Advance in grid
            bool const min_axis_x{t_max_x < t_max_y};
            if (min_axis_x) {
                dist = t_max_x;
                t_max_x += t_dx;
                cell_p.x += step.x;
            } else {
                dist = t_max_y;
                t_max_y += t_dy;
                cell_p.y += step.y;
            }

            // Ensure cell in within the world
            if (cell_p.x >= 0 && cell_p.x < max_width && cell_p.y >= 0 && cell_p.y < max_height) {
                Vec2f const normal = min_axis_x ? Vec2f{-static_cast<float>(step.x), 0.0f} : Vec2f{0.0f, -static_cast<float>(step.y)};
                TCell const& cell = grid.get(cell_p);
                if (callback(cell, dist, normal)) {
                    break;
                }
            } else {
                break;
            }
        }
    }
};