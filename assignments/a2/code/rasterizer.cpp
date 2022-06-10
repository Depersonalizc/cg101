// clang-format off
//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include <vector>
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>


rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f> &positions)
{
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
{
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &cols)
{
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}

/* Check whether the point (x, y) is inside the 
 * triangle represented by v[0], v[1], v[2]
 */
static bool insideTriangle(float x, float y, const std::array<Vector3f, 3> v)
{
    auto pt = Eigen::Vector3f(x, y, 0);
    auto z0 = (v[0] - v[1]).cross(v[0] - pt).z();
    auto z1 = (v[1] - v[2]).cross(v[1] - pt).z();
    if (std::signbit(z0) ^ std::signbit(z1)) {
        return 0;
    }
    auto z2 = (v[2] - v[0]).cross(v[2] - pt).z();
    return std::signbit(z1) == std::signbit(z2);
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const std::array<Vector3f, 3> v)
{
    float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {c1, c2, c3};
}

void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
    Eigen::Matrix4f mvp = projection * view * model;

    /* For vertices of each triangle indexed by `ind`, load:
     * - position from buffer `buf` (indexed by `pos_buffer.pos_id`)
     * - color    from buffer `col` (indexed by `col_buffer.col_id`)
     * ...
     * Then pack them in a `Triangle` primitive sent to `rasterize_triangle`
     */

    auto& buf = pos_buf[pos_buffer.pos_id];
    auto& ind = ind_buf[ind_buffer.ind_id];
    auto& col = col_buf[col_buffer.col_id];

    for (auto & i : ind) {  // for each triangle
        Triangle t;
        for (auto j = 0; j < 3; ++j) {  // for each vertex of triangle
            
            /* MVP transformations */
            Eigen::Vector4f v = mvp * to_vec4(buf[i[j]], 1.f);
            Eigen::Vector3f vCol = col[i[j]];

            /* Homogeneous division */
            v /= v.w();

            /* Viewport transformation */
            v.x() = 0.5f * width  * (v.x() + 1.f);
            v.y() = 0.5f * height * (v.y() + 1.f);
            
            // std::cout << v.z() << std::endl;
            
            /* Convert z into range [0, 1] */
            v.z() = 0.5f * v.z() + 0.5f;

            // std::cout << v.z() << std::endl;

            /* Pack vertex data into Triangle primitive */
            t.setVertex(j, v.head<3>());  // viewport position
            t.setColor(j, vCol);
        }
        rasterize_triangle(t);
    }

    /* Transfer MSAA_buffers to drawing buffers */
    set_pixel_MSAA();
    set_depth_MSAA();

}

/* Screen space rasterization */
void rst::rasterizer::rasterize_triangle(const Triangle& t) 
{
    /* Find the bounding box of current triangle. */
    float xmin, xmax;
    float ymin, ymax;
    xmin = xmax = t.v[0].x();
    ymin = ymax = t.v[0].y();

    for (auto v = std::next(t.v.begin()); v != t.v.end(); ++v) {
        if (v->x() > xmax) {
            xmax = v->x();
        }
        else if (v->x() < xmin) {
            xmin = v->x();
        }

        if (v->y() > ymax) {
            ymax = v->y();
        }
        else if (v->y() < ymin) {
            ymin = v->y();
        }
    }

    /* Iterate over pixels within bbox and determine 
     * whether current pixel is inside the triangle */
    float delta = 1.f / (MSAA_level + 1);
    for (int i = (int)xmin; i <= (int)xmax; ++i) {
        for (int j = (int)ymin; j <= (int)ymax; ++j) {

            /* Pixel indexed (i, j) from bottom-left */
            // auto  pxColorBuf = Eigen::Vector3f(0, 0, 0);
            // float pxDepthBuf = 0;
            // int pxDepthCount = 0;
            
            /* Sample at LEVEL^2 subpixel locations */
            int subidx = 0;
            for (int u = 1; u <= MSAA_level; ++u) {
                for (int v = 1; v <= MSAA_level; ++v) {

                    auto x = i + u * delta;
                    auto y = j + v * delta;

                    if (insideTriangle(x, y, t.v)) {
                        /* If inside, barycentric-interpolate z value
                        * of point (x, y, _) on triangle t. */
                        auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);

                        /* The two commented lines below are the general formulae for any w;
                        * We already did the homogeneous division (w=1) so it gets simplier. */
                        // float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                        // float z_interp = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                        // z_interp *= w_reciprocal;
                        auto z_interp = (alpha * t.v[0].z() +
                                         beta  * t.v[1].z() +
                                         gamma * t.v[2].z() ) / (alpha + beta + gamma);
                        auto newDepth = set_subpixel_depth(i, j, subidx, z_interp, 1);

                        if (newDepth) {
                            Eigen::Vector3f color_interp = (
                                alpha * t.color[0] + 
                                beta  * t.color[1] +
                                gamma * t.color[2]
                            );
                            set_subpixel(i, j, subidx, color_interp);
                        }
                        ++subidx;
                    }

                }
            } // subpixels done

        }
    } // pixels done
}

void rst::rasterizer::set_model(const Eigen::Matrix4f& m)
{
    model = m;
}

void rst::rasterizer::set_view(const Eigen::Matrix4f& v)
{
    view = v;
}

void rst::rasterizer::set_projection(const Eigen::Matrix4f& p)
{
    projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
        for (auto & v : MSAA_frame_buf) {
            std::fill(v.begin(), v.end(), Eigen::Vector3f{0, 0, 0});
        }
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity());
        for (auto & v : MSAA_depth_buf) {
            std::fill(v.begin(), v.end(), std::numeric_limits<float>::infinity());
        }
    }
}

rst::rasterizer::rasterizer(int w, int h, int msaa) : width(w), height(h), MSAA_level(msaa)
{
    area = w * h;
    frame_buf.resize(area);
    depth_buf.resize(area);
    MSAA_frame_buf.resize(area);
    MSAA_depth_buf.resize(area);

    if (MSAA_level < 1) {
        MSAA_level = 1;
    }
    for (auto & v : MSAA_frame_buf) {
        v.resize(MSAA_level * MSAA_level);
    }
    for (auto & v : MSAA_depth_buf) {
        v.resize(MSAA_level * MSAA_level);
    }
}

int rst::rasterizer::get_index(int i, int j)
{
    return ( height - (j+1) ) * width + i;
}

void rst::rasterizer::set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color)
{
    set_pixel((int)point.x(), (int)point.y(), color);
}

void rst::rasterizer::set_pixel(int i, int j, const Eigen::Vector3f& color)
{
    auto ind = get_index(i, j);
    frame_buf[ind] = color;
}

void rst::rasterizer::set_subpixel(int i, int j, int k, const Eigen::Vector3f& color)
{
    auto ind = get_index(i, j);
    MSAA_frame_buf[ind][k] = color;
}

void rst::rasterizer::set_pixel_MSAA()
{
    auto norm = 1.f / (MSAA_level * MSAA_level);
    for (int i = 0; i < area; ++i) {
        frame_buf[i].setZero();
        for (auto & col : MSAA_frame_buf[i]) {
            frame_buf[i] += col;
        }
        frame_buf[i] *= norm;
    }
}


bool rst::rasterizer::set_depth(const Eigen::Vector3f& point, float depth, bool compareZ)
{
    return set_depth((int)point.x(), (int)point.y(), depth, compareZ);
}

bool rst::rasterizer::set_depth(int i, int j, float depth, bool compareZ)
{
    auto ind = get_index(i, j);
    if (compareZ) {
        if (depth < depth_buf[ind]) {
            depth_buf[ind] = depth;
            return 1;
        }
    }
    else {
        depth_buf[ind] = depth;
        return 1;
    }
    return 0;
}

bool rst::rasterizer::set_subpixel_depth(int i, int j, int k, float depth, bool compareZ)
{
    auto ind = get_index(i, j);
    if (compareZ) {
        if (depth < MSAA_depth_buf[ind][k]) {
            MSAA_depth_buf[ind][k] = depth;
            return 1;
        }
    }
    else {
        MSAA_depth_buf[ind][k] = depth;
        return 1;
    }
    return 0;
}

void rst::rasterizer::set_depth_MSAA()
{
    std::fill(depth_buf.begin(), depth_buf.end(), 0.f);
    for (int i = 0; i < area; ++i) {
        int count = 0;
        for (auto z : MSAA_depth_buf[i]) {
            if (!std::isinf(z)) {
                depth_buf[i] += z;
                ++count;
            }
        }
        if (count) {
            depth_buf[i] /= count;
        }
    }
}




// clang-format on