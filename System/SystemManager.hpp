#pragma once
    #include "System.hpp"
    #include "../Signature.hpp"
    #include <memory>
    #include <unordered_map>

class SystemManager {
    private:
        std::unordered_map<const char *, Signature> _signatures{};
        std::unordered_map<const char *, std::shared_ptr<System>> _systems{};
    public:
        template<typename T>
        std::shared_ptr<T> RegisterSystem() {
            const char *typeName = typeid(T).name();

            assert(_systems.find(typeName) == _systems.end() && "Register system more than once.");

            auto system = std::make_shared<T>();
            _systems.insert({typeName, system});
            return system;
        }

        template<typename T>
        void Setsignature(Signature signature) {
            const char *typeName = typeid(T).name();

            assert(_systems.find(typeName) != _systems.end() && "System used before registered.");

            _signatures.insert({typeName, signature});
        }

        void EntityDestroyed(Entity entity) {
            for (auto const& pair : _systems) {
                auto const& system = pair.second;
                system->_entities.erase(entity);
            }
        }

        void EntitySignatureChanged(Entity entity, Signature signature) {
            for (auto const& pair: _systems) {
                auto const& type = pair.first;
                auto const& system = pair.second;
                auto const& systemSignature = _signatures[type];

                if ((signature & systemSignature) == systemSignature)
                    system->_entities.insert(entity);
                else
                    system->_entities.erase(entity);
            }
        }
};