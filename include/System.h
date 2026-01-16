#pragma once

#include "Entity.h"
#include <set>

class System {
    public:
        std::set<Entity> _entities;
        int order = 0;
    public:
        virtual void Update() = 0;
};
