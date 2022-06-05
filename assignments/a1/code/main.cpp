#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <unistd.h>
#include <string.h>


constexpr double MY_PI = 3.14159265359;

float deg2rad(float deg) {
    return deg / 180.0 * MY_PI;
}

/* Assume the camera looks straight in
 * the direction of -Z so no rotation is needed */
Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], \
                 0, 1, 0, -eye_pos[1], \
                 0, 0, 1, -eye_pos[2], \
                 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(Eigen::Vector3f axis, float rot_deg)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // Create the model matrix for rotating the triangle around
    // an arbitray axis centered at the origin. Then return it.

    Eigen::Matrix4f rot = Eigen::Matrix4f::Identity();
    Eigen::Matrix3f N;

    axis.normalize();
    N << 0, -axis.z(), axis.y(), \
         axis.z(), 0, -axis.x(), \
         -axis.y(), axis.x(), 0; // Cross product

    auto rad = deg2rad(rot_deg);
    auto c = cos(rad);
    auto s = sin(rad);
    auto R1 = axis * axis.transpose() * (1 - c);
    auto R2 = Eigen::Matrix3f::Identity() * c;
    auto R3 = N * s;
    auto R = R1 + R2 + R3;
    rot.topLeftCorner<3, 3>() = R;

    model = rot * model;

    return model;
}

/* get_projection_matrix
 *    eye_fov: (horizontal) field of view, in degrees
 *    aspect_ratio: width / height == r / t
 *    near, far: depths (>0) of the clipping planes
 */
Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float near, float far)
{
    Eigen::Matrix4f projection;

    /* Convert from fovX & aspect ratio to u & r */
    auto r = near * tan(deg2rad(eye_fov) / 2);
    auto t = r / aspect_ratio;
    
    auto p11 = -near / r;
    auto p22 = -near / t;
    auto p33 = (near + far) / (near - far);
    auto p34 = 2 * near*far / (near - far);

    projection << p11, 0,   0,   0,   \
                  0,   p22, 0,   0,   \
                  0,   0,   p33, p34, \
                  0,   0,   1,   0;

    return projection;
}

int main(int argc, char* argv[])
{
    float rot_deg = 0.0;
    Eigen::Vector3f axis = {0, 0, 1};  // Z-axis
    
    bool write = false;
    std::string filename = "output.png";

    int opt;    
    while ( (opt = getopt(argc, argv, "o::a:d:")) != -1) {
        switch (opt) {
            case 'o':  // output dir
                write = true;
                if (optarg)
                    filename = std::string(optarg);
                break;
            case 'a':  // roation axis
                axis[0] = std::stof(std::string(strtok(optarg, " ")));
                axis[1] = std::stof(std::string(strtok(NULL, " ")));
                axis[2] = std::stof(std::string(strtok(NULL, " ")));
                break;
            case 'd':  // rotation angle, in radius
                rot_deg = std::stof(std::string(optarg));
                break;
        }
    }

    rst::rasterizer r(700, 700);

    /* camera position */
    Eigen::Vector3f eye_pos = {0, 0, 5};

    /* vertex position */
    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};
    
    /* face indices */
    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (write) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(axis, rot_deg));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);
        
        std::cout << "Output to: " << filename << std::endl;

        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(axis, rot_deg));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(1);

        std::cout << "frame count: " << frame_count++ << std::endl;

        if (key == 'a')
            rot_deg += 10;  // Rotate CCW
        else if (key == 'd')
            rot_deg -= 10;  // Rotate CW
    }

    return 0;
}
