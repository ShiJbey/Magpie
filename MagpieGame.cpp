#include "MagpieGame.hpp"

Scene::Object* Magpie::MagpieGame::remove_placed_item(uint32_t row, uint32_t col) {
    Scene::Object* removed_item = nullptr;
    uint32_t index_to_remove = 0;
    for (uint32_t i = 0; i < placed_items.size(); i++) {
            Scene::Object* item = placed_items[i];
        if (row == (uint32_t)item->transform->position.x && col == (uint32_t)item->transform->position.y) {
            removed_item = item;
            index_to_remove = i;
            break;
        }
    }
    if (removed_item != nullptr) {
        placed_items.erase(placed_items.begin() + index_to_remove);
    }
    return removed_item;
}