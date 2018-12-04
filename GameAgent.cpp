#include "GameAgent.hpp"

#include <iostream>


glm::vec2 Magpie::GameAgent::getDirectionVec2() {
    switch (orientation) {
        case DIRECTION::UP:
            return glm::vec2(0, 1);
        case DIRECTION::RIGHT:
            return glm::vec2(1, 0);
        case DIRECTION::DOWN:
            return glm::vec2(0, -1);
        case DIRECTION::LEFT:
            return glm::vec2(-1, 0);
        default:
            return glm::vec2(0, 0);
    }
};

Magpie::GameAgent::DIRECTION Magpie::GameAgent::get_orientation() {
    return this->orientation;
};

void Magpie::GameAgent::setDestination(glm::vec3 destination) {
    current_destination = glm::vec3(this->path.top(), 0.0f);
    turnTo(current_destination);
};

uint32_t Magpie::GameAgent::get_state() {
    return current_state;
};

void Magpie::GameAgent::set_state(uint32_t state) {
    current_state = state;
};

void Magpie::GameAgent::turnTo(glm::vec3 destination) {
    // Do nothing
};

Magpie::Path* Magpie::GameAgent::get_path() {
    return &path;
};

void Magpie::GameAgent::set_path(Magpie::Path path) {

    if (path.get_path().size() == 0) {
        std::cout << "EMPTY PATH" << std::endl;
        return;
    }
    // The magpie has finished the previous path and this one
    // should replace the old one
    if (this->next_destination_index > this->path.get_path().size() + 1 ||
        this->next_destination_index == 0) {
        
        this->is_new_path = true;
        this->new_path = path;
        this->next_destination_index = 0;
       // printf("Reseting next destination index to %d\n", next_destination_index);
    }

    // The player has clicked for the magpie to move on a different path
    // while the Magpie was currently navigating a path
    else {
        printf("PIZZA: Appending new path to previous\n");
        // Remove all locations in the path vector after the current destination
        // Append this path to the end of the old path and let the magpie continue
        // as normal
        std::vector<glm::vec2> modified_path = this->path.get_path();
        std::vector<glm::vec2> new_path = path.get_path();

        if (new_path.back() == this->get_path()->get_path().back()) {
            return;
        }

        // Erase all locations after the next destination
        modified_path.erase(modified_path.begin() + next_destination_index, modified_path.end());

        // Append all the locations in the given path
        for(auto &pos : new_path) {
            modified_path.push_back(pos);
        }

        

        /*
        size_t size = modified_path.size();
        int x_direction = 0;
        if (modified_path[size-1].x != modified_path[0].x) {
            x_direction = (int)(abs(modified_path[size-1].x - modified_path[0].x) / (modified_path[size-1].x - modified_path[0].x));
        }
        int y_direction = 0;
        if (modified_path[size-1].y != modified_path[0].y) {
            y_direction = (int)(abs(modified_path[size-1].y - modified_path[0].y) / (modified_path[size-1].y - modified_path[0].y));
        }

        std::vector<glm::vec2> result_path;

        auto is_in = [modified_path](float x, float y){
            for (auto i : modified_path) {
                if (i.x == x && i.y == y) {
                    return true;
                }
            }
            return false;
        };

        float xx = modified_path[0].x;
        float yy = modified_path[0].y;

        float original_x = modified_path[next_destination_index-1].x;
        float original_y = modified_path[next_destination_index-1].y;

        result_path.emplace_back(glm::vec2(xx, yy));
        while (xx != modified_path[size-1].x || yy != modified_path[size-1].y) {
            if (xx == original_x && yy == original_y) {
                this->next_destination_index = result_path.size();
            }
            if (x_direction != 0 && is_in(xx + x_direction, yy)) {
                result_path.emplace_back(glm::vec2(xx + x_direction, yy));
                xx = xx + x_direction;
                continue;
            } else if(y_direction != 0 && is_in(xx, yy + y_direction)) {
                result_path.emplace_back(glm::vec2(xx, yy + y_direction));
                yy = yy + y_direction;
                continue;
            }
            break;
        }
        */



//         print the new path
        //std::cout << "**== Modified Path ==**" << std::endl;
        //std::cout << next_destination_index << ":" << result_path[next_destination_index].x  << "," << result_path[next_destination_index].y << std::endl;
        //for(auto &pos : result_path) {
        //    std::cout << "\t( " << pos.x << ", " << pos.y << " )" << std::endl;
        //};



        // Set the path to the newly modified one
        this->path.set_path(modified_path);
        this->new_path.set_path(modified_path);
    }
};