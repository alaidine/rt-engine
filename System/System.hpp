#pragma once
    #include "../Entity/Entity.hpp"
    #include <set>

class System {
    public:
        std::set<Entity> _entities;
};
