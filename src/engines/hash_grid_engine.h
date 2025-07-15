#pragma once
#include "../olc/olcPixelGameEngine.h"
#include "point_cloud/hash_grid.hpp"
#include "utilities.hpp" // scrW , scrH
#include "vertex/vertex.h"

#include "sdf/sdfs.h"
#include <array>
#include <queue>

// Override base class with your custom functionality
class Example : public olc::PixelGameEngine {

  using V = Vertex;
  using HashGrid = pc::HashGrid<V>;
  using vert_handle = typename HashGrid::vert_handle;

  using vec2 = olc::vf2d;
  using ivec2 = olc::vi2d;
  using uvec2 = olc::vu2d;

  HashGrid hash_grid;

  vec2 point;
  float radius;

  uvec2 curr_cell;
  std::vector<bool> visited;
  std::vector<bool> queued;
  // std::queue<uvec2> cells;
  std::vector<uvec2> cells;

  float timer;

public:
  Example()
      //      : hash_grid(vec2(0.0f), vec2(scrW, scrH), uvec2(16, 9)),
      : hash_grid(vec2(0.0f), vec2(scrW, scrH), uvec2(48, 27)),
        // point(0.0f, 0.0f),
        point(0.5f * scrW, 0.5f * scrH), radius(0.0f),
        visited(hash_grid._buckets.size(), false),
        queued(hash_grid._buckets.size(), false), curr_cell(-1, -1) {
    // Name your application
    sAppName = "Example";

    uvec2 init_cell = hash_grid.pos_to_cell(point);
    cells.push_back(init_cell);
    queued[hash_grid.cell_to_index(init_cell)] = true;

    timer = 0.0f;
  }

  void DrawPoints(const HashGrid &hg, const olc::Pixel &color = olc::WHITE,
                  const int r = 4) {
    for (vert_handle vh : hg.verts()) {
      const V &v = hg.vert(vh);
      FillCircle(v.pos, r, color);
    }
  }

  void DrawGrid(const HashGrid &hg, const olc::Pixel &color = olc::GREY) {
    const vec2 &max_pos = hg._max_pos;
    const vec2 &min_pos = hg._min_pos;
    const uvec2 &n = hg._n;
    const vec2 &sep = hg._sep;

    for (uint i = 0U; i < n.x; ++i) {
      float x = sep.x * float(i);
      DrawLine(ivec2(x, 0), ivec2(x, scrH), color);
    }

    for (uint j = 0U; j < n.x; ++j) {
      float y = sep.y * float(j);
      DrawLine(ivec2(0, y), ivec2(scrW, y), color);
    }
  }

  void DrawCell(const HashGrid &hg, const uvec2 &cell,
                const olc::Pixel &color = olc::DARK_RED) {
    FillRect(vec2(cell) * hg._sep + vec2(1), hg._sep - vec2(1), color);
  }

  void DrawOverlap(const HashGrid &hg, const vec2 &p, const float r) {
    vec2 min_cell_pos(0.0f, 0.0f);

    for (uint j = 0U; j < hg._n.y; ++j) {
      for (uint i = 0U; i < hg._n.x; ++i) {
        uvec2 c(i, j);

        // bool bCondition = sdf::box(p - min_cell_pos - 0.5f * hg._sep, 0.5f *
        // hg._sep) < r; bool bCondition = (c - hg.pos_to_cell(p)).mag2() <
        // (vec2(r) / hg._sep).mag2() + 1;
        bool bOverlap =
            sdf::box_max(p - min_cell_pos - 0.5f * hg._sep, 0.5f * hg._sep) < r;
        bool bCollide =
            sdf::box(p - min_cell_pos - 0.5f * hg._sep, 0.5f * hg._sep) < r;

        if (bCollide && !bOverlap)
          FillRect(min_cell_pos + vec2(1), hg._sep - vec2(1), olc::DARK_RED);

        min_cell_pos.x += hg._sep.x;
      }

      min_cell_pos.x = 0.0f;
      min_cell_pos.y += hg._sep.y;
    }

    // std::cout << min_cell_pos <<"\n";

    DrawCircle(p, r, olc::WHITE);
  }

  /*  void BreadthFirstSearch() {
      HashGrid &hg = hash_grid;

      if (cells.empty())
        return;

      curr_cell = cells.front();
      cells.pop_back();
      visited[hg.cell_to_index(curr_cell)] = true;

      //static const ivec2 relative_cells[4] = {
      //    ivec2(1, 0), ivec2(-1, 0), ivec2(0, 1),  ivec2(0, -1) };
      //    ivec2(1, 1), ivec2(-1, 1), ivec2(1, -1), ivec2(-1, -1)};

      //static const ivec2 relative_cells[8] = {
      //  ivec2(-1, -1), ivec2(0, -1), ivec2(1, -1),
      //  ivec2(-1, 0), ivec2(1, 0),
      //  ivec2(-1, 1), ivec2(0, 1), ivec2(1, 1)
      //};

      //static const ivec2 relative_cells[8] = {
      //  ivec2(-1, -1), ivec2(0, -1), ivec2(1, -1), ivec2(1, 0),
      //  ivec2(1, 1), ivec2(0, 1), ivec2(-1, 1), ivec2(-1, 0)
      //};

      static const ivec2 relative_cells[8] = {
        ivec2(-1, 0), ivec2(-1, 1), ivec2(0, 1), ivec2(1, 1),
        ivec2(1, 0), ivec2(1, -1), ivec2(0, -1), ivec2(-1, -1)
      };

      for (const ivec2 &rc : relative_cells) {
        uvec2 c = curr_cell + rc;
        uint id = hg.cell_to_index(c);

        if (hg.is_in_range_cell(c) && !queued[id]) {
          cells.push_back(c);
          queued[id] = true;
        }
      }
    }*/

  /*  void BreadthFirstSearch() {
      HashGrid &hg = hash_grid;

      if (cells.empty()) {
        uvec2 c = hg.pos_to_cell(point);

        //      uvec2 init_cells[9] = {
        //          {c.x, c.y},         {c.x, c.y + 1},     {c.x + 1, c.y + 1},
        //          {c.x + 1, c.y},     {c.x + 1, c.y - 1}, {c.x, c.y - 1},
        //          {c.x - 1, c.y - 1}, {c.x - 1, c.y},     {c.x - 1, c.y + 1}};

        uvec2 init_cells[9] = {
            {c.x - 1, c.y - 1}, {c.x, c.y - 1}, {c.x + 1, c.y - 1},
            {c.x - 1, c.y},     {c.x, c.y},     {c.x + 1, c.y},
            {c.x - 1, c.y + 1}, {c.x, c.y + 1}, {c.x + 1, c.y + 1}};

        for (const uvec2 &c : init_cells) {
          if (hg.is_in_range_cell(c)) {
            queued[hg.cell_to_index(c)] = true;
            cells.push_back(c);
          }
        }

        return;
      }

      for (const uvec2 &c : cells) {
        if (!hg.is_empty_cell(c)) {
          std::cout << c << "\n";

          return;
        }
      }

      for (const uvec2 &c : cells) {
        vec2 min_cell_pos = hg.cell_to_pos(c);
        radius = std::max(radius, sdf::box_max(
          point - min_cell_pos - 0.5f * hg._sep,
          0.5f * hg._sep
        ));
      }

      static const ivec2 relative_cells[8] = {
        {-1, -1}, { 0, -1}, { 1, -1},
        {-1,  0},           { 1,  0},
        {-1,  1}, { 0,  1}, { 1,  1}
      };

      std::vector<uvec2> new_cells;
      new_cells.reserve(cells.size() + 4);
      //cells.reserve(cells.size() + new_cells.capacity());

      for(const uvec2& cell : cells)
        visited[hg.cell_to_index(cell)] = true;

      while (!cells.empty()) {
        uvec2 cell = cells.back();
        new_cells.push_back(cell);
        cells.pop_back();

        //  visited[hg.cell_to_index(cell)] = true;

        for(const uvec2& rc : relative_cells) {
          uvec2 c = cell + rc;
          uint id = hg.cell_to_index(c);

          vec2 half_size = 0.5f * hg._sep;  // half cell size
          vec2 min_cell_pos = hg.cell_to_pos(c);
          if (hg.is_in_range_cell(c) &&
              !queued[id] &&
              sdf::box(
              point - min_cell_pos - half_size,
              half_size) < radius) {

            cells.push_back(c);
            //new_cells.push_back(c);
            queued[id] = true;
          }
        }
      }

      cells = std::move(new_cells);
    }*/

  void BreadthFirstSearch() {
    HashGrid &hg = hash_grid;

    /*static const ivec2 relative_cells[8] = {
      {-1, -1}, {0, -1}, {1, -1}, 
      {-1,  0},          {1,  0},
      {-1,  1}, {0,  1}, {1,  1}
    };*/

    static const ivec2 relative_cells[8] = {
                {0, -1}, 
      {-1,  0},          {1,  0},
                {0,  1}
    };

    for(const uvec2& c : cells) {
      visited[hg.cell_to_index(c)] = true;
      radius = std::max(radius, hg.box_max(point, c));
    }

    std::vector<uvec2> new_cells;
    new_cells.reserve(cells.size() + 4ULL);

    while (!cells.empty()) {
      uvec2 cell = cells.back();
      //if (!visited[hg.cell_to_index(cell)])
      //  new_cells.push_back(cell);
      cells.pop_back();
      
      for(const uvec2& rc : relative_cells) {
        uvec2 c = cell + rc;
        uint id = hg.cell_to_index(c);

        if (hg.is_in_range_cell(c) && 
            !queued[id] && 
            hg.box_min(point, c) < radius) {

          cells.push_back(c);
          new_cells.push_back(c);
          queued[id] = true;
        }
      } 
    }

    cells = std::move(new_cells);

//    std::cout << cells.size() << "\n";
//    for(const uvec2& c : cells)
//      std::cout << c << ", ";
//    std::cout << "\n==================================================\n";
  
  }


  void DrawVisited() {
    HashGrid &hg = hash_grid;

    for (uint j = 0U; j < hg._n.y; ++j) {
      for (uint i = 0U; i < hg._n.x; ++i) {
        uvec2 cell(i, j);
        uint id = hg.cell_to_index(cell);

        if (visited[id])
          DrawCell(hg, cell, olc::DARK_RED);
        else if (queued[id])
          DrawCell(hg, cell, olc::DARK_YELLOW);
      }
    }
  }

void CircleSearch() {}

public:
bool OnUserCreate() override {
  // Called once at the start, so create things here

  HashGrid &hg = hash_grid;

  hg.reserve_verts(4 * 4);

  //    for (int j = 0, ny = 4; j < ny; ++j)
  //      for (int i = 0, nx = 4; i < nx; ++i)
  //        hg.add_vert(vec2((scrW * i) / (nx - 1), (scrH * j) / (ny - 1)));

  //  std::cout << std::boolalpha << hg.are_verts_shrinked() << "\n";

  //  hg.remove_vert(vert_handle(7U));
  //  hg.shrink_verts();

  /*{ Timer t;
    for(vert_handle vh : hg.verts()) {
      const vf2d& pos = hg.vert(vh).pos;
      vert_handle ovh = hg.find_closest(pos); // other

    }
  }*/

  /*for(vert_handle vh : hg.verts()) {
    if (vh != 1U && vh != 7U)
      hg.remove_vert(vh);
  }*/

  //    vec2 p(0, 0);

  /*    hg.add_vert(vec2(scrW - 10.0f, 0));
      hg.add_vert(vec2(scrW - 40.0f, scrH - 40.0f));
    //  hg.add_vert(0.5f * vec2(scrW, scrH));
      hg.add_vert(vec2(0.6f * scrW, 0.0f));*/

  hg.add_vert(vec2(260.0f, 260.0f));


  // NOTE: But how do you find the closest to a given vertex, it will return
  // you the exact same vertex

  /*    vert_handle vh = hg.find_closest(p); // 5
      std::cout << vh << "\n";

      vert_handle ovh;
      float min_dist = std::numeric_limits<float>::max();
      for (vert_handle vi : hg.verts()) {
        if (hg.is_removed_vert(vi))
          continue;

        const V &v = hg.vert(vi);
        float dist = (v.pos - p).mag2();
        if (dist < min_dist) {
          min_dist = dist;
          ovh = vi;
        }
      }
      std::cout << "real: " << ovh << "\n";*/

  /*  for(vert_handle vi : hg.verts()) {
      if (hg.is_removed_vert(vi)) {
        std::cout << "R, ";
        continue;
      }
      std::cout << hg.vert(vi).pos << "\n";
  //    std::cout << vi << ", ";
    }*/

  return true;
}

bool OnUserUpdate(float fElapsedTime) override {
  Clear(olc::Pixel(16, 16, 32));

  DrawGrid(hash_grid);

  // if (GetMouse(0).bHeld)
  //   radius =(GetMousePos() - point).mag();
  // std::cout << GetMouseWheel() << "\n";
  // radius += float(GetMouseWheel() / 120) * 420.0f * fElapsedTime;
  radius += float(GetMouseWheel() / 120) * 7.0f;
  radius = std::max(radius, 0.0f);

  if (GetMouse(1).bHeld)
    point = GetMousePos();

  //    DrawOverlap(hash_grid, point, radius);

  /*const float cycle = 0.95f;
  timer += fElapsedTime;
  if (timer > cycle) {
    BreadthFirstSearch();
    timer -= cycle;
  }*/
  if (GetKey(olc::N).bPressed)
    BreadthFirstSearch();
  DrawVisited();
  DrawCell(hash_grid, curr_cell, olc::GREEN);

  DrawPoints(hash_grid, olc::YELLOW);

  return true;
}
}
;
