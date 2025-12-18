#pragma once

#include <cstdint>
#include <bitset>
#include <queue>
#include <array>
#include <cassert>
#include <unordered_map>
#include <memory>
#include <set>

namespace rt {

	using Entity = std::uint32_t;
	const Entity MAX_ENTITIES = 5000;

	using ComponentType = std::uint8_t;
	const ComponentType MAX_COMPONENTS = 32;

	using Signature = std::bitset<MAX_COMPONENTS>;

	class EntityManager
	{
	public:
		EntityManager();

		Entity CreateEntity();

		void DestroyEntity(Entity entity);
		void SetSignature(Entity entity, Signature signature);
		Signature GetSignature(Entity entity);

	private:
		// Queue of unused entity IDs
		std::queue<Entity> mAvailableEntities{};
		// Array of signatures where the index corresponds to the entity ID
		std::array<Signature, MAX_ENTITIES> mSignatures{};
		// Total living entities - used to keep limits on how many exist
		uint32_t mLivingEntityCount{};
	};

	// The one instance of virtual inheritance in the entire implementation.
	// An interface is needed so that the ComponentManager (seen later)
	// can tell a generic ComponentArray that an entity has been destroyed
	// and that it needs to update its array mappings.
	class IComponentArray
	{
	public:
		virtual ~IComponentArray() = default;
		virtual void EntityDestroyed(Entity entity) = 0;
	};

	template<typename T>
	class ComponentArray : public IComponentArray
	{
	public:
		void InsertData(Entity entity, T component);
		void RemoveData(Entity entity);
		T& GetData(Entity entity);

		void EntityDestroyed(Entity entity) override;

	private:
		// The packed array of components (of generic type T),
		// set to a specified maximum amount, matching the maximum number
		// of entities allowed to exist simultaneously, so that each entity
		// has a unique spot.
		std::array<T, MAX_ENTITIES> mComponentArray;

		// Map from an entity ID to an array index.
		std::unordered_map<Entity, size_t> mEntityToIndexMap;

		// Map from an array index to an entity ID.
		std::unordered_map<size_t, Entity> mIndexToEntityMap;

		// Total size of valid entries in the array.
		size_t mSize;
	};

	class ComponentManager
	{
	public:
		template<typename T>
		void RegisterComponent();
		template<typename T>
		ComponentType GetComponentType();
		template<typename T>
		void AddComponent(Entity entity, T component);
		template<typename T>
		void RemoveComponent(Entity entity);
		template<typename T>
		T& GetComponent(Entity entity);
		void EntityDestroyed(Entity entity);

	private:
		// Map from type string pointer to a component type
		std::unordered_map<const char*, ComponentType> mComponentTypes{};

		// Map from type string pointer to a component array
		std::unordered_map<const char*, std::shared_ptr<IComponentArray>> mComponentArrays{};

		// The component type to be assigned to the next registered component - starting at 0
		ComponentType mNextComponentType{};

		// Convenience function to get the statically casted pointer to the ComponentArray of type T.
		template<typename T>
		std::shared_ptr<ComponentArray<T>> GetComponentArray();
	};

	class System
	{
	public:
		std::set<Entity> mEntities;
	};

	class SystemManager
	{
	public:
		template<typename T>
		std::shared_ptr<T> RegisterSystem();

		template<typename T>
		void SetSignature(Signature signature);

		void EntityDestroyed(Entity entity);

		void EntitySignatureChanged(Entity entity, Signature entitySignature);

	private:
		// Map from system type string pointer to a signature
		std::unordered_map<const char*, Signature> mSignatures{};

		// Map from system type string pointer to a system pointer
		std::unordered_map<const char*, std::shared_ptr<System>> mSystems{};
	};

	class Coordinator
	{
	public:
		void Init();

		// Entity methods
		Entity CreateEntity();

		void DestroyEntity(Entity entity);

		// Component methods
		template<typename T>
		void RegisterComponent();

		template<typename T>
		void AddComponent(Entity entity, T component);

		template<typename T>
		void RemoveComponent(Entity entity);

		template<typename T>
		T& GetComponent(Entity entity);

		template<typename T>
		ComponentType GetComponentType();

		// System methods
		template<typename T>
		std::shared_ptr<T> RegisterSystem();

		template<typename T>
		void SetSystemSignature(Signature signature);

	private:
		std::unique_ptr<ComponentManager> mComponentManager;
		std::unique_ptr<EntityManager> mEntityManager;
		std::unique_ptr<SystemManager> mSystemManager;
	};

}
