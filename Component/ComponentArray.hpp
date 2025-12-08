#pragma once
    #include "../Signature.hpp"
    #include "Component.hpp"
    #include "../Entity/Entity.hpp"
    #include <unordered_map>
    #include <cassert>

class IComponentArray {
    public:
        virtual ~IComponentArray() = default;
        virtual void EntityDestroyed(Entity entity) = 0;
};

template<typename T>
class ComponentArray : public IComponentArray {
    private:
        std::array<T, MAX_ENTITIES> _componentArray;
        std::unordered_map<Entity, size_t> _entityToIndexMap;
        std::unordered_map<size_t, Entity> _indexToEntityMap;
        size_t size;
    public:
        void InsertData(Entity entity, T component) {
            assert(_entityToIndexMap.find(entity) == _entityToIndexMap.end() && "Component added to same entity more than once.");

            size_t newIndex = size;
            _entityToIndexMap[entity] = newIndex;
            _indexToEntityMap[newIndex] = entity;
            _componentArray[newIndex] = component;
            size++;
        }

        void RemoveData(Entity entity) {
            assert(_entityToIndexMap.find(entity) != _entityToIndexMap.end() && "Removing non existence entity.");

            size_t indexOfRemovedEntity = _entityToIndexMap[entity];
            size_t indexOfLastElement = size -1;
            _componentArray[indexOfRemovedEntity] = _componentArray[indexOfLastElement];

            Entity entityOfLastElement = _indexToEntityMap[indexOfLastElement];
            _entityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
            _indexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

            _entityToIndexMap.erase(entity);
            _indexToEntityMap.erase(indexOfLastElement);

            size--;
        }

        T& GetData(Entity entity) {
            assert(_entityToIndexMap.find(entity) != _entityToIndexMap.end() && "Try to get data of non existence entity.");

            return _componentArray[_entityToIndexMap[entity]];
        }

        void EntityDestroyed(Entity entity) override {
            if (_entityToIndexMap.find(entity) != _entityToIndexMap.end())
                RemoveData(entity);
        }
};