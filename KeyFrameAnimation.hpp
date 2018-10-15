#pragma once

#include "Scene.hpp"
#include "GL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <string>
#include <map>
#include <iostream>

namespace Magpie
{
    /* Handles a single mesh within the animation */
    struct KeyFrame{
        uint32_t frame;
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;

        void print() {
            std::cout << "Frame #:" << frame  << "\nPosition: ("  << position.x << ", " << position.y << ", "  << position.z  << ")" << "\n" <<
             "Rotation: (" << rotation.x << ", "  << rotation.y  << ", "  << rotation.z <<  ", " << rotation.w  << ")"  << "\n" <<
                "Scale: (" << scale.x  << ", " << scale.y << ", " << scale.z << ")" << std::endl;
        }

        static KeyFrame get_difference(KeyFrame a, KeyFrame b) {
            KeyFrame difference;
            difference.position = a.position - b.position;
            difference.rotation = a.rotation * glm::inverse(b.rotation);
            difference.scale = a.scale - b.scale;
            return difference;
        }
    };

    /* Handles the swapping of the vertices when drawing */
    struct KeyFrameAnimation
    {
        
        void load_model(std::string const &filename);

        float tween_factor;
        uint32_t elapsed_frames;

        uint32_t keyframe_index = 0;
        uint32_t next_keyframe_index = 0;

        std::vector< uint32_t > current_key_frames;
        //std::vector< KeyFrame > frames;
        std::map< std::string, std::vector< KeyFrame > > frames;

        void print_frames(std::string const &name) {
            if (frames.count(name) > 0) {
                std::cout << "Frames for: " << name << std::endl;
                for (auto iter = frames[name].begin(); iter != frames[name].end(); iter++) {
                    iter->print();
                }
		    }
        }

        void update(Scene *scene);
    };

    
}