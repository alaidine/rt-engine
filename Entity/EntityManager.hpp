#pragma once
    #include "Entity.hpp"
    #include "../Signature.hpp"
    #include <queue>
    #include <array>
    #include <cassert>

class EntityManager {
    private:
        std::queue<Entity> _entityAvailable{};
        std::array<Signature, MAX_ENTITIES> _signatures{};
        uint32_t _livingEntity{};
    public:
        EntityManager() {
            for (Entity entity = 0; entity < MAX_ENTITIES; entity++)
                _entityAvailable.push(entity);
        }

        Entity CreateEntity() {
            assert(_livingEntity < MAX_ENTITIES && "Too many entities in existence.");

            Entity id = _entityAvailable.front();
            _entityAvailable.pop();
            _livingEntity++;
            return id;
        }

        void DestroyEntity(Entity entity) {
            assert(entity < MAX_ENTITIES && "Entity out of range.");

            _signatures[entity].reset();
            _entityAvailable.push(entity);
            _livingEntity--;
        }

        void SetSignature(Entity entity ,Signature signature) {
            assert(entity < MAX_ENTITIES && "Entity out of range.");

            _signatures[entity] = signature;
        }

        Signature GetSignature(Entity entity) {
            assert(entity < MAX_ENTITIES && "Entity out of range.");

            return _signatures[entity];
        }
};