#include "Clickable.hpp"

#include <algorithm>

bool Magpie::BoundingBox::check_intersect(const glm::vec3 &ray_origin, const glm::vec3 &ray_dir) {
    float e;
    float f; 
    float t1;
    float t2;

    glm::vec3 t_max(100000.0f, 100000.0f, 100000.0f);
    glm::vec3 t_min(0.0f, 0.0f, 0.0f);

    glm::vec3 x_axis(1.0f, 0.0f, 0.0f);
    glm::vec3 y_axis(0.0f, 1.0f, 0.0f);
    glm::vec3 z_axis(0.0f, 0.0f, 1.0f);

    glm::vec3 delta = position - ray_origin;

    // Fist test the x-axis
    e = glm::dot(x_axis, delta);
    f = glm::dot(ray_dir, x_axis);

    t1 = (e + bb_min.x) / f;
    t2 = (e + bb_max.x) / f;

    if (t1 > t2){ // if wrong order
	    float w = t1; t1 = t2; t2 = w; // swap t1 and t2
    }

    if (t2 < t_max.x) t_max.x = t2;
    if (t1 > t_min.x) t_min.x = t1;

    // test the y-axis
    e = glm::dot(y_axis, delta);
    f = glm::dot(ray_dir, y_axis);

    t1 = (e + bb_min.y) / f;
    t2 = (e + bb_max.y) / f;

    if (t1 > t2){ // if wrong order
	    float w = t1; t1 = t2; t2 = w; // swap t1 and t2
    }

    if (t2 < t_max.y) t_max.y = t2;
    if (t1 > t_min.y) t_min.y = t1;

    // test the z-axis
    e = glm::dot(z_axis, delta);
    f = glm::dot(ray_dir, z_axis);

    t1 = (e + bb_min.z) / f;
    t2 = (e + bb_max.z) / f;

    if (t1 > t2){ // if wrong order
	    float w = t1; t1 = t2; t2 = w; // swap t1 and t2
    }

    if (t2 < t_max.z) t_max.z = t2;
    if (t1 > t_min.z) t_min.z = t1;

    float t_min_all = std::max(std::max(t_min.x, t_min.y), t_min.z);
    float t_max_all = std::min(std::min(t_max.x, t_max.y), t_max.z);

    if ( t_max_all < t_min_all ) {
        return false;
    } else {
        printf("Box Position: (x: %f, y: %f, z: %f)\n", position.x, position.y, position.z);
        printf("Ray origin: (x: %f, y: %f, z: %f)\n", ray_origin.x, ray_origin.y, ray_origin.z);
        printf("Ray Dir: (x: %f, y: %f, z: %f)\n", ray_dir.x, ray_dir.y, ray_dir.z);
        printf("Delta: (x: %f, y: %f, z: %f)\n", delta.x, delta.y, delta.z);
        printf("Max Values: (x: %f, y: %f, z: %f)\n", ray_origin.x + t_max.x * ray_dir.x, ray_origin.y + t_max.y * ray_dir.y, ray_origin.z + t_max.z * ray_dir.z);
        printf("Min Values: (x: %f, y: %f, z: %f)\n", ray_origin.x + t_min.x * ray_dir.x, ray_origin.y + t_min.y * ray_dir.y, ray_origin.z + t_min.z * ray_dir.z);
        return true;
    }
};