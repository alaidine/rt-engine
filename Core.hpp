#pragma once
    #include "Entity/EntityManager.hpp"
    #include "Component/ComponentManager.hpp"
    #include "System/SystemManager.hpp"
    #include <iostream>

class Core {
    private:
        std::unique_ptr<EntityManager> _entityManager;
        std::unique_ptr<ComponentManager> _componentManager;
        std::unique_ptr<SystemManager> _systemManager;
    public:
         void Init() {
            _entityManager = std::make_unique<EntityManager>();
            _componentManager = std::make_unique<ComponentManager>();
            _systemManager = std::make_unique<SystemManager>();
        }

        //------ENTITY METHODS--------
        Entity CreateEntity() {
            return _entityManager->CreateEntity();
        }

        void DestroyEntity(Entity entity) {
            _entityManager->DestroyEntity(entity);
            _componentManager->EntityDestroyed(entity);
            _systemManager->EntityDestroyed(entity);
        }

        void printSignature(Entity entity) {
            Signature sig = _entityManager->GetSignature(entity);

            std::cout << "Signature => " << sig << std::endl;
        }

        //------- COMPOENET METHODS ----------
        template<typename T>
        void RegisterComponent() {
            _componentManager->RegisterComponent<T>();
        }

        template<typename T>
        void AddComponent(Entity entity, T component) {
            _componentManager->AddComponent<T>(entity, component);

            auto signature = _entityManager->GetSignature(entity);
            signature.set(_componentManager->GetComponentType<T>(), true);
            _entityManager->SetSignature(entity, signature);

            _systemManager->EntitySignatureChanged(entity, signature);
        }

        template<typename T>
        void RemoveComponent(Entity entity) {
            _componentManager->RemoveComponent<T>(entity);

            auto signature = _entityManager->GetSignature(entity);
            signature.set(_componentManager->GetComponentType<T>(), false);
            _entityManager->SetSignature(entity, signature);

            _systemManager->EntitySignatureChanged(entity, signature);
        }

        template<typename T>
        T& GetComponent(Entity entity) {
            return _componentManager->GetComponent<T>(entity);
        }


        template<typename T>
        ComponentType GetComponentType() {
            return _componentManager->GetComponentType<T>();
        }

        //---------- SYSTEM METHODS ----------
        template<typename T>
        std::shared_ptr<T> RegisterSystem() {
            return _systemManager->RegisterSystem<T>();
        }

        template<typename T>
        void SetSystemSignature(Signature signature) {
            _systemManager->Setsignature<T>(signature);
        }

        void UpdateAllSystem() {
            _systemManager->SystemUpdateOrder();
        }
};