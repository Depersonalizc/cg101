#include "software_renderer.h"

#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>

#include "triangulation.h"

using namespace std;

namespace CMU462 {

// Utils
bool on_left(Vector2D start, Vector2D dest, Vector2D p) {
  auto sd = dest - start;
  auto sp = p - start;
  float cross = sp.x * sd.y - sp.y * sd.x;
  // return true <-> neg cross <-> p is on the left of sd
  return std::signbit(cross);
}

// Implements SoftwareRenderer //

void SoftwareRendererImp::draw_svg( SVG& svg ) {

  // set top level transformation
  transformation = svg_2_screen;

  // draw all elements
  for ( size_t i = 0; i < svg.elements.size(); ++i ) {
    draw_element(svg.elements[i]);
  }

  // draw canvas outline
  Vector2D a = transform(Vector2D(    0    ,     0    )); a.x--; a.y--;
  Vector2D b = transform(Vector2D(svg.width,     0    )); b.x++; b.y--;
  Vector2D c = transform(Vector2D(    0    ,svg.height)); c.x--; c.y++;
  Vector2D d = transform(Vector2D(svg.width,svg.height)); d.x++; d.y++;

  rasterize_line(a.x, a.y, b.x, b.y, Color::Black);
  rasterize_line(a.x, a.y, c.x, c.y, Color::Black);
  rasterize_line(d.x, d.y, b.x, b.y, Color::Black);
  rasterize_line(d.x, d.y, c.x, c.y, Color::Black);

  // resolve and send to render target
  resolve();

}

void SoftwareRendererImp::set_sample_rate( size_t sample_rate ) {

  // Task 4: 
  // You may want to modify this for supersampling support
  this->sample_rate = sample_rate;

}

void SoftwareRendererImp::set_render_target( unsigned char* render_target,
                                             size_t width, size_t height ) {

  // Task 4: 
  // You may want to modify this for supersampling support
  this->render_target = render_target;
  this->target_w = width;
  this->target_h = height;

}

void SoftwareRendererImp::draw_element( SVGElement* element ) {

  // Task 5 (part 1):
  // Modify this to implement the transformation stack

  switch(element->type) {
    case POINT:
      draw_point(static_cast<Point&>(*element));
      break;
    case LINE:
      draw_line(static_cast<Line&>(*element));
      break;
    case POLYLINE:
      draw_polyline(static_cast<Polyline&>(*element));
      break;
    case RECT:
      draw_rect(static_cast<Rect&>(*element));
      break;
    case POLYGON:
      draw_polygon(static_cast<Polygon&>(*element));
      break;
    case ELLIPSE:
      draw_ellipse(static_cast<Ellipse&>(*element));
      break;
    case IMAGE:
      draw_image(static_cast<Image&>(*element));
      break;
    case GROUP:
      draw_group(static_cast<Group&>(*element));
      break;
    default:
      break;
  }

}


// Primitive Drawing //

void SoftwareRendererImp::draw_point( Point& point ) {

  Vector2D p = transform(point.position);
  rasterize_point( p.x, p.y, point.style.fillColor );

}

void SoftwareRendererImp::draw_line( Line& line ) { 

  Vector2D p0 = transform(line.from);
  Vector2D p1 = transform(line.to);
  rasterize_line( p0.x, p0.y, p1.x, p1.y, line.style.strokeColor );

}

void SoftwareRendererImp::draw_polyline( Polyline& polyline ) {

  Color c = polyline.style.strokeColor;

  if( c.a != 0 ) {
    int nPoints = polyline.points.size();
    for( int i = 0; i < nPoints - 1; i++ ) {
      Vector2D p0 = transform(polyline.points[(i+0) % nPoints]);
      Vector2D p1 = transform(polyline.points[(i+1) % nPoints]);
      rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    }
  }
}

void SoftwareRendererImp::draw_rect( Rect& rect ) {

  Color c;
  
  // draw as two triangles
  float x = rect.position.x;
  float y = rect.position.y;
  float w = rect.dimension.x;
  float h = rect.dimension.y;

  Vector2D p0 = transform(Vector2D(   x   ,   y   ));
  Vector2D p1 = transform(Vector2D( x + w ,   y   ));
  Vector2D p2 = transform(Vector2D(   x   , y + h ));
  Vector2D p3 = transform(Vector2D( x + w , y + h ));
  
  // draw fill
  c = rect.style.fillColor;
  if (c.a != 0 ) {
    rasterize_triangle( p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, c );
    rasterize_triangle( p2.x, p2.y, p1.x, p1.y, p3.x, p3.y, c );
  }

  // draw outline
  c = rect.style.strokeColor;
  if( c.a != 0 ) {
    rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    rasterize_line( p1.x, p1.y, p3.x, p3.y, c );
    rasterize_line( p3.x, p3.y, p2.x, p2.y, c );
    rasterize_line( p2.x, p2.y, p0.x, p0.y, c );
  }

}

void SoftwareRendererImp::draw_polygon( Polygon& polygon ) {

  Color c;

  // draw fill
  c = polygon.style.fillColor;
  if( c.a != 0 ) {

    // triangulate
    vector<Vector2D> triangles;
    triangulate( polygon, triangles );

    // draw as triangles
    for (size_t i = 0; i < triangles.size(); i += 3) {
      Vector2D p0 = transform(triangles[i + 0]);
      Vector2D p1 = transform(triangles[i + 1]);
      Vector2D p2 = transform(triangles[i + 2]);
      rasterize_triangle( p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, c );
    }
  }

  // draw outline
  c = polygon.style.strokeColor;
  if( c.a != 0 ) {
    int nPoints = polygon.points.size();
    for( int i = 0; i < nPoints; i++ ) {
      Vector2D p0 = transform(polygon.points[(i+0) % nPoints]);
      Vector2D p1 = transform(polygon.points[(i+1) % nPoints]);
      rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    }
  }
}

void SoftwareRendererImp::draw_ellipse( Ellipse& ellipse ) {

  // Extra credit 

}

void SoftwareRendererImp::draw_image( Image& image ) {

  Vector2D p0 = transform(image.position);
  Vector2D p1 = transform(image.position + image.dimension);

  rasterize_image( p0.x, p0.y, p1.x, p1.y, image.tex );
}

void SoftwareRendererImp::draw_group( Group& group ) {

  for ( size_t i = 0; i < group.elements.size(); ++i ) {
    draw_element(group.elements[i]);
  }

}

// Rasterization //

// The input arguments in the rasterization functions 
// below are all defined in screen space coordinates

bool SoftwareRendererImp::inscreen(int sx, int sy) {
  return (sx >= 0 && sx < target_w &&
          sy >= 0 && sy < target_h);
}

void SoftwareRendererImp::rasterize_point( float x, float y, Color color ) {

  // fill in the nearest pixel
  int sx = (int) floor(x);
  int sy = (int) floor(y);

  // check bounds
  if (!inscreen(sx, sy)) return;

  // fill sample - NOT doing alpha blending!
  render_target[4 * (sx + sy * target_w)    ] = (uint8_t) (color.r * 255);
  render_target[4 * (sx + sy * target_w) + 1] = (uint8_t) (color.g * 255);
  render_target[4 * (sx + sy * target_w) + 2] = (uint8_t) (color.b * 255);
  render_target[4 * (sx + sy * target_w) + 3] = (uint8_t) (color.a * 255);

}

void SoftwareRendererImp::rasterize_line( float x0, float y0,
                                          float x1, float y1,
                                          Color color) {

  // Task 2: 
  // Implement line rasterization
  // TODO: Wu's algorithm

  /* Find nearest start/end pixels */
  int sx0, sx1, sy0, sy1;
  if (x0 < x1) {
    sx0 = (int) x0;
    sy0 = (int) y0;
    sx1 = (int) x1;
    sy1 = (int) y1;
  } else {
    sx0 = (int) x1;
    sy0 = (int) y1;
    sx1 = (int) x0;
    sy1 = (int) y0;
  }

  int dx = sx1 - sx0;  // guaranteed non-neg
  int dy = sy1 - sy0;
  int sx = sx0;
  int sy = sy0;

  int ey = dx / 2;
  int ex = dy / 2;
  if (dy >= dx) {
    /* steep down */
    for (; sy <= sy1; sy++, ex += dx) {
      // check bounds
      if (ex >= dy) {
        ex -= dy;
        sx++;
      }
      if (inscreen(sx, sy)) {
        render_target[4 * (sx + sy * target_w)    ] = (uint8_t) (color.r * 255);
        render_target[4 * (sx + sy * target_w) + 1] = (uint8_t) (color.g * 255);
        render_target[4 * (sx + sy * target_w) + 2] = (uint8_t) (color.b * 255);
        render_target[4 * (sx + sy * target_w) + 3] = (uint8_t) (color.a * 255);        
      }
    }
  } else if (dy < -dx) {
    /* steep up */
    for (; sy >= sy1; sy--, ex += dx) {
      if (ex >= 0) {
        ex += dy;
        sx++;
      }
      if (inscreen(sx, sy)) {
        render_target[4 * (sx + sy * target_w)    ] = (uint8_t) (color.r * 255);
        render_target[4 * (sx + sy * target_w) + 1] = (uint8_t) (color.g * 255);
        render_target[4 * (sx + sy * target_w) + 2] = (uint8_t) (color.b * 255);
        render_target[4 * (sx + sy * target_w) + 3] = (uint8_t) (color.a * 255);        
      }
    }
  } else if (dy >= 0) {
    /* flat down */
    for (; sx <= sx1; sx++, ey += dy) {
      if (ey >= dx) {
        ey -= dx;
        sy++;
      }
      if (inscreen(sx, sy)) {
        render_target[4 * (sx + sy * target_w)    ] = (uint8_t) (color.r * 255);
        render_target[4 * (sx + sy * target_w) + 1] = (uint8_t) (color.g * 255);
        render_target[4 * (sx + sy * target_w) + 2] = (uint8_t) (color.b * 255);
        render_target[4 * (sx + sy * target_w) + 3] = (uint8_t) (color.a * 255);        
      }
    }
  } else {
    /* flat up */
    for (; sx <= sx1; sx++, ey += dy) {
      if (ey <= 0) {
        ey += dx;
        sy--;
      }
      if (inscreen(sx, sy)) {
        render_target[4 * (sx + sy * target_w)    ] = (uint8_t) (color.r * 255);
        render_target[4 * (sx + sy * target_w) + 1] = (uint8_t) (color.g * 255);
        render_target[4 * (sx + sy * target_w) + 2] = (uint8_t) (color.b * 255);
        render_target[4 * (sx + sy * target_w) + 3] = (uint8_t) (color.a * 255);        
      }
    }
  }

}

void SoftwareRendererImp::rasterize_triangle( float x0, float y0,
                                              float x1, float y1,
                                              float x2, float y2,
                                              Color color ) {
  // Task 3: 
  // Implement triangle rasterization
  static const int BSIZE = 4;

  auto verts = vector<Vector2D> {Vector2D(x0, y0), Vector2D(x1, y1), Vector2D(x2, y2),};
  
  int sx0 = (int) x0;
  int sy0 = (int) y0;
  int sx1 = (int) x1;
  int sy1 = (int) y1;
  int sx2 = (int) x2;
  int sy2 = (int) y2;
  int sx_min = std::min({sx0, sx1, sx2});
  int sx_max = std::max({sx0, sx1, sx2});
  int sy_min = std::min({sy0, sy1, sy2});
  int sy_max = std::max({sy0, sy1, sy2});
  int pad_x = (BSIZE - 1) - ( (sx_max - sx_min) % BSIZE );
  int pad_y = (BSIZE - 1) - ( (sy_max - sy_min) % BSIZE );

  /* early in/out optimization w/ block size = 4 */
  for (int sx = sx_min - pad_x; sx < sx_max; sx += BSIZE) {
    for (int sy = sy_min - pad_y; sy < sy_max; sy += BSIZE) {
      auto xl = sx + 0.5f;  // topleft : (sx, sy) + .5
      auto yt = sy + 0.5f;
      auto xr = xl + BSIZE - 1;  // botright: (sx, sy) + .5 + BSIZE-1
      auto yb = yt + BSIZE - 1;
      auto corners = std::vector<Vector2D> {
        Vector2D(xl, yt), Vector2D(xr, yt), 
        Vector2D(xr, yb), Vector2D(xl, yb),
      };

      bool leftside;
      bool early_in = true;
      bool early_out = false;
      for (int i = 0; i < 3 && !early_out; i++) {
        early_out = true;
        for (auto & p : corners) {
          leftside = on_left(verts[i], verts[(i+1)%3], p);
          early_in  &= leftside;
          early_out &= !leftside;
        }
      }

      if (early_in) {
        /* early in, fill all pixels. */
        for (int _sx = sx; _sx < sx + BSIZE; _sx++) {
          for (int _sy = sy; _sy < sy + BSIZE; _sy++) {
            if (inscreen(_sx, _sy)) {
              render_target[4 * (_sx + _sy * target_w)    ] = (uint8_t) (color.r * 255);
              render_target[4 * (_sx + _sy * target_w) + 1] = (uint8_t) (color.g * 255);
              render_target[4 * (_sx + _sy * target_w) + 2] = (uint8_t) (color.b * 255);
              render_target[4 * (_sx + _sy * target_w) + 3] = (uint8_t) (color.a * 255);
              // render_target[4 * (_sx + _sy * target_w)    ] = 0;
              // render_target[4 * (_sx + _sy * target_w) + 1] = 255;
              // render_target[4 * (_sx + _sy * target_w) + 2] = 0;
              // render_target[4 * (_sx + _sy * target_w) + 3] = 255;
            }
          }
        }
      }
      // else if (early_out) {
      //   for (int _sx = sx; _sx < sx + BSIZE; _sx++) {
      //     for (int _sy = sy; _sy < sy + BSIZE; _sy++) {
      //       // render_target[4 * (_sx + _sy * target_w)    ] = (uint8_t) (color.r * 255);
      //       // render_target[4 * (_sx + _sy * target_w) + 1] = (uint8_t) (color.g * 255);
      //       // render_target[4 * (_sx + _sy * target_w) + 2] = (uint8_t) (color.b * 255);
      //       // render_target[4 * (_sx + _sy * target_w) + 3] = (uint8_t) (color.a * 255);
      //       render_target[4 * (_sx + _sy * target_w)    ] = 255;
      //       render_target[4 * (_sx + _sy * target_w) + 1] = 0;
      //       render_target[4 * (_sx + _sy * target_w) + 2] = 0;
      //       render_target[4 * (_sx + _sy * target_w) + 3] = 255;
      //     }
      //   }
      // }
      else if (!early_out) {
        /* partially in, need to check all pixels. */
        for (int _sx = sx; _sx < sx + BSIZE; _sx++) {
          for (int _sy = sy; _sy < sy + BSIZE; _sy++) {
            if (!inscreen(_sx, _sy)) continue;
            if (inside(x0, y0, x1, y1, x2, y2, _sx + 0.5f, _sy + 0.5f)) {
              render_target[4 * (_sx + _sy * target_w)    ] = (uint8_t) (color.r * 255);
              render_target[4 * (_sx + _sy * target_w) + 1] = (uint8_t) (color.g * 255);
              render_target[4 * (_sx + _sy * target_w) + 2] = (uint8_t) (color.b * 255);
              render_target[4 * (_sx + _sy * target_w) + 3] = (uint8_t) (color.a * 255);
              // render_target[4 * (_sx + _sy * target_w)    ] = 0;
              // render_target[4 * (_sx + _sy * target_w) + 1] = 0;
              // render_target[4 * (_sx + _sy * target_w) + 2] = 255;
              // render_target[4 * (_sx + _sy * target_w) + 3] = 255;
            } 
            // else {
            //   render_target[4 * (_sx + _sy * target_w)    ] = 255;
            //   render_target[4 * (_sx + _sy * target_w) + 1] = 255;
            //   render_target[4 * (_sx + _sy * target_w) + 2] = 0;
            //   render_target[4 * (_sx + _sy * target_w) + 3] = 255;
            // }
          }
        }
      }
    }
  }

  // /* Brute force within bbox */
  // for (int sx = sx_min; sx <= sx_max; sx++) {
  //   for (int sy = sy_min; sy <= sy_max; sy++) {
  //     // topleft : (sx, sy) + .5
  //     // botright: (sx, sy) + .5 + BSIZE-1
  //     auto p = Vector2D(sx + .5, sy + .5);
  //     bool early_in = true;
  //     for (int i = 0; i < 3; i++)
  //       early_in &= on_left(verts[i], verts[(i+1)%3], p);
  //     if (early_in)
  //     {
  //       render_target[4 * (sx + sy * target_w)    ] = (uint8_t) (color.r * 255);
  //       render_target[4 * (sx + sy * target_w) + 1] = (uint8_t) (color.g * 255);
  //       render_target[4 * (sx + sy * target_w) + 2] = (uint8_t) (color.b * 255);
  //       render_target[4 * (sx + sy * target_w) + 3] = (uint8_t) (color.a * 255);
  //     }
  //   }
  // }

}

void SoftwareRendererImp::rasterize_image( float x0, float y0,
                                           float x1, float y1,
                                           Texture& tex ) {
  // Task 6: 
  // Implement image rasterization

}

// resolve samples to render target
void SoftwareRendererImp::resolve( void ) {

  // Task 4: 
  // Implement supersampling
  // You may also need to modify other functions marked with "Task 4".
  return;

}


} // namespace CMU462
