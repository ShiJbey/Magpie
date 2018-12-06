#pragma once

#include "Scene.hpp"
#include "Clickable.hpp"
#include "Player.hpp"
#include "Door.hpp"

namespace Magpie {

    // Items can be any in game object that the player interacts with
    // For example, guard keys, key card, and even the items they are
    // stealing
    class Item {
    public:
        static uint32_t instance_count;
        Item(Scene::Object* obj);
        void set_scene_object(Scene::Object* obj);
        Scene::Object* get_scene_object();
    protected:
        Scene::Object* scene_object;
        uint32_t instance_id;
    };

    // Defines anything that can be stolen in the game (e.g. gems and paintings)
    class Stealable {
    public:
        uint32_t get_selling_price();
        void set_selling_price(uint32_t price);
        // Increments the player's score by the black markey value
        void steal(Player* player);
    protected:
        // Value added to the player's score
        uint32_t selling_price = 0;
    };


    class Gem : public Item, public Clickable, public Stealable, public AnimatedModel {
    public:
        const uint32_t SELLING_PRICE = 10;

        Gem();
        Gem(Scene::Object* obj);
        BoundingBox* get_boundingbox();
        void on_click();
        virtual void update_animation(float elapsed);
    protected:
        float osc_tick = 0.0f;

        
    };

    class Painting: public Item, public Clickable, public Stealable, public AnimatedModel {
    public:

        const uint32_t SELLING_PRICE = 10;

        Painting ();
        Painting (Scene::Object* obj_ptr);

        BoundingBox* get_boundingbox();

        void on_click();
        virtual void update_animation(float elapsed);
    protected:
        float osc_tick = 0.0f;
    };

    class Geode: public Item, public Stealable, public AnimatedModel {
    public:

        const uint32_t SELLING_PRICE = 30;
        
        Geode ();
        Geode (Scene::Object* obj_ptr);

        virtual void update_animation(float elapsed);
    protected:
        float osc_tick = 0.0f;

        
    };

    class KeyCard : public Item, public Clickable, public AnimatedModel {
    public:

        KeyCard(Door::ACCESS_LEVEL acces_level);
        KeyCard(Door::ACCESS_LEVEL acces_level, Scene::Object* obj);

        BoundingBox* get_boundingbox();
        void on_click();
        virtual void update_animation(float elapsed);

        Door::ACCESS_LEVEL get_access_level();

    protected:
        float osc_tick = 0.0f;
        Door::ACCESS_LEVEL access_level;

        
    };

    class DogTreat : public Item, public Clickable, public AnimatedModel {
    public:

        DogTreat();
        DogTreat(Scene::Object* obj);

        BoundingBox* get_boundingbox();
        void on_click();
        virtual void update_animation(float elapsed);

    protected:
        float osc_tick = 0.0f;
                
    };

    class CardboardBox : public Item, public Clickable, public AnimatedModel {
    public:

        CardboardBox();
        CardboardBox(Scene::Object* obj);

        BoundingBox* get_boundingbox();
        void on_click();
        virtual void update_animation(float elapsed);

    protected:
        float osc_tick = 0.0f;
                
    };

}