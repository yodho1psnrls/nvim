#pragma once

// ECS includes
#include <iostream>
#include <array>
#include <unordered_map>
#include <cassert>
#include <utility>  //std::pair
#include <queue>
#include <set>
#include <memory>
//#include <stdint.h> //The C99 header stdint.h defines types that do not depend on architecture or compiler.

// Event Bus includes
#include <list>
#include <typeindex>
#include <map>


//@todo: Figure out how to reset it
//     Figure out how to save and load the game state 

//Entity Component System, but modified to store multiple components per entity
//and systems can store multiple sets of different entity types
//Also you can define an entity type by the specific number of a component type, that it needs to have
//https://austinmorlan.com/posts/entity_component_system/ //main post
//https://savas.ca/nomad                  //referenced

using Entity = std::uint32_t;
using ComponentType = std::uint8_t;

const Entity MAX_ENTITIES = 256U;       //1024
const ComponentType MAX_COMPONENT_TYPES = 32U;  //64
const ComponentType MAX_MULT_COMPONENTS = 48U;  //64
//const int MAX_SYSTEM_ENTITY_TYPES = 10;

template <ComponentType N>
struct NumSet {
  //typedef int arr[N];
  ComponentType arr[N];

  void reset() {
    for (ComponentType i = 0; i < N; i++) {
      arr[i] = 0;
    }
  }
  NumSet() { reset(); }

  ComponentType& operator[](const ComponentType id) {
    assert(id < N && "id out of range !");
    return arr[id];
  }
  ComponentType operator[](const ComponentType id) const {
    assert(id < N && "id out of range !");
    return arr[id];
  }

  void set(const ComponentType id, const ComponentType value = 1U) {
    assert(id < N && "id out of range !");
    arr[id] = value;
  }
  ComponentType get(const ComponentType id) const {
    assert(id < N && "Out of range !");
    return arr[id];
  }

  friend bool operator>(const NumSet<N>& lhs, const NumSet<N>& rhs) {
    for (ComponentType i = 0; i < N; i++) {
      if (lhs.arr[i] <= rhs.arr[i]) return false;
    }
    return true;
  }
  friend bool operator<(const NumSet<N>& lhs, const NumSet<N>& rhs) {
    for (ComponentType i = 0; i < N; i++) {
      if (lhs.arr[i] >= rhs.arr[i]) return false;
    }
    return true;
  }
  friend bool operator>=(const NumSet<N>& lhs, const NumSet<N>& rhs) {
    for (ComponentType i = 0; i < N; i++) {
      if (lhs.arr[i] < rhs.arr[i]) return false;
    }
    return true;
  }
  friend bool operator<=(const NumSet<N>& lhs, const NumSet<N>& rhs) {
    for (ComponentType i = 0; i < N; i++) {
      if (lhs.arr[i] > rhs.arr[i]) return false;
    }
    return true;
  }
};

using Signature = NumSet<MAX_COMPONENT_TYPES>;

class EntityManager {
  //https://en.cppreference.com/w/cpp/language/zero_initialization, class member {};
  std::queue<Entity> availableEntities{};
  std::array<Signature, MAX_ENTITIES> signatures{};
  Entity count{}; //counts the living entities

public:
  EntityManager() {
    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
      availableEntities.push(entity);
    }
  }
  Entity createEntity() {
    assert(count < MAX_ENTITIES && "Too many entities in existence !");
    Entity id = availableEntities.front();
    availableEntities.pop();
    count++;

    return id;
  }
  void destroyEntity(const Entity entity) {
    assert(entity < MAX_ENTITIES && "Entity out of range !");

    signatures[entity].reset();
    availableEntities.push(entity);
    --count;
  }
  void setSignature(const Entity entity, const Signature& signature) {
    assert(entity < MAX_ENTITIES && "Entity out of range !");
    signatures[entity] = signature;
  }
  Signature getSignature(const Entity entity) const {     //Non-Const, because it will be changed after that
    assert(entity < MAX_ENTITIES && "Entity out of range !");
    return signatures[entity];
  }
};

class IComponentArray {
public:
  virtual ~IComponentArray() = default;
  virtual void entityDestroyed(const Entity) = 0;
};

template<typename T>
class ComponentArrayUnique : public IComponentArray {
  std::array<T, MAX_ENTITIES> arr;

  std::unordered_map<Entity, Entity> entityToIndex;
  std::unordered_map<Entity, Entity> indexToEntity;

  Entity size{};  //{} - means it is equal to 0

public:
  void add(const Entity entity, T component) {
    assert(entityToIndex.find(entity) == entityToIndex.end() && "Component added more than once !");

    entityToIndex[entity] = size;
    indexToEntity[size] = entity;
    arr[size] = component;
    size++;
  }

  void remove(const Entity entity) {
    assert(entityToIndex.find(entity) != entityToIndex.end() && "Removing non-existent component");

    //Copy element at end into deleted element's place to maintain density
    Entity indexOfRemovedEntity = entityToIndex[entity];
    Entity indexOfLastElement = size - 1;
    arr[indexOfRemovedEntity] = arr[indexOfLastElement];

    //Update map to point to moved spot
    Entity entityOfLastElement = indexToEntity[indexOfLastElement];
    entityToIndex[entityOfLastElement] = indexOfRemovedEntity;
    indexToEntity[indexOfRemovedEntity] = entityOfLastElement;

    entityToIndex.erase(entity);
    indexToEntity.erase(indexOfLastElement);

    size--;
  }

  T& get(const Entity entity) {
    assert(entityToIndex.find(entity) != entityToIndex.end() && "Retrieving non-existent component data !");

    return arr[entityToIndex[entity]];
  }

  //T* getAll() {
  //  return arr;
  //}

  void entityDestroyed(const Entity entity) override {
    if (entityToIndex.find(entity) != entityToIndex.end()) {
      remove(entity);
    }
  }
};

const Entity MAX_MULT_COMPONENTS_OF_THE_SAME_TYPE = MAX_ENTITIES * MAX_MULT_COMPONENTS;

template<typename T>
class ComponentArrayMult : public IComponentArray {
  std::array<T, MAX_MULT_COMPONENTS_OF_THE_SAME_TYPE> arr;

  std::unordered_map<Entity, Entity> entityToIndex; //entity id to first index of the array
  std::unordered_map<Entity, Entity> entityCount; //number of components for this entity id
  //std::unordered_map<int, std::pair<size_t, size_t>> entityToArr; //entityToIndex and entityCount combined
  std::unordered_map<Entity, Entity> indexToEntity;   //first index of array to entity id map

  Entity size{};  // Total size of valid entries in the array.

  void remove(const Entity entity, const Entity howMuch = 1) {
    assert(entityToIndex.find(entity) != entityToIndex.end() && "Removing components from non-existent entity !");
    assert(howMuch <= entityCount[entity] && "Too many components to be removed !");

    Entity n = entityToIndex[entity] + entityCount[entity];
    for (Entity i = n; i < size; i++) {
      arr[i - howMuch] = arr[i];

      if (indexToEntity.find(i) != indexToEntity.end()) {
        Entity cEntity = indexToEntity[i];
        //std::cout <<"|||"<< cEntity << "|||\n";
        indexToEntity.erase(i);
        indexToEntity[i - howMuch] = cEntity;
        entityToIndex[cEntity] -= howMuch;
      }
    }

    if (howMuch == entityCount[entity]) {
      //already filled with other entity's index, if howMuch==entityCount
      //if (entityToIndex[entity] + howMuch - 1 == size - 1)
      if (entityToIndex[entity] + howMuch == size)  //if last item in the array
        indexToEntity.erase(entityToIndex[entity]);
      entityToIndex.erase(entity);
      entityCount.erase(entity);
    }
    else {
      //arr[entityToIndex[entity] + entityCount[entity]] = obj;
      entityCount[entity] -= howMuch;
    }
    size -= howMuch;
  }

public:
  //What if this entity hasnt had any object of this type before that ? 
  void addOne(const Entity entity, T obj) {
    assert(size <= MAX_MULT_COMPONENTS_OF_THE_SAME_TYPE && "No space left in the component array !");

    if (entityToIndex.find(entity) == entityToIndex.end()) {
      //There hasnt been components of this type for that entity
      entityToIndex[entity] = size;
      indexToEntity[size] = entity;
      entityCount[entity] = 0;
      //Places the last entity's data in the back of the array
    }

    //every data after that move by one and change the entityToIndex and entity
    Entity newElementIndex = entityToIndex[entity] + entityCount[entity];
    for (Entity i = size - 1; i > newElementIndex - 1; i--) { //i = 0; i>0 should not execute
      arr[i + 1] = arr[i];

      if (indexToEntity.find(i) != indexToEntity.end()) {
        Entity cEntity = indexToEntity[i];
        indexToEntity.erase(i);
        indexToEntity[i + 1] = cEntity;
        entityToIndex[cEntity] += 1;
      }
    }

    arr[entityToIndex[entity] + entityCount[entity]] = obj;
    entityCount[entity]++;
    ++size;
  }

  void clear(const Entity entity) {
    assert(entityToIndex.find(entity) != entityToIndex.end() && "No entity to be cleared !");
    remove(entity, entityCount[entity]);
  }

  //void insertArr(const int entity, const std::array<T, MAX_SAME_COMPONENTS>& data) {
  void addArr(const Entity entity, T* data, const Entity dataSize) {
    assert(size + dataSize <= MAX_MULT_COMPONENTS_OF_THE_SAME_TYPE && "Too many components of the same type !");

    if (entityToIndex.find(entity) == entityToIndex.end()) {
      //There hasnt been components of this type for that entity
      entityToIndex[entity] = size;
      indexToEntity[size] = entity;
      entityCount[entity] = 0;    //I think that by default its 0, but just to be sure...
    }

    //Move the rest of the array to the right by dataSize
    //int n = entityToIndex[entity] + entityCount[entity] - 1;
    Entity firstNewElementIndex = entityToIndex[entity] + entityCount[entity];
    for (Entity i = size - 1; i > firstNewElementIndex - 1; i--) {  //i = 0; i>0 should not execute
      arr[i + dataSize] = arr[i];

      if (indexToEntity.find(i) != indexToEntity.end()) {
        Entity cEntity = indexToEntity[i];
        indexToEntity.erase(i);
        indexToEntity[i + dataSize] = cEntity;
        entityToIndex[cEntity] += dataSize;
      }
    }

    //Write the new data into the now free space of the array
    for (Entity i = 0; i < dataSize; i++) {
      arr[firstNewElementIndex + i] = data[i];
    }

    entityCount[entity] += dataSize;
    size += dataSize;

  }

  T* getArr(const Entity entity) {
    return &arr[entityToIndex[entity]];
  }
  Entity getCount(const Entity entity) {
    return entityCount[entity];
  }

  T* getAll() {
    return &arr[0];
  }

  Entity getSize() {
    return size;
  }

  //int localToGlobalIndex(const int id) {
  //  return entity
  //}

  void entityDestroyed(const Entity entity) override {
    if (entityToIndex.find(entity) != entityToIndex.end()) clear(entity);
  }

  void print() const {
    for (Entity i = 0; i < size; i++) {
      std::cout << arr[i] << ", ";
    }
    std::cout << "\n";
    std::cout << "\n";

    std::cout << "entity -> index: \n";
    for (auto x : entityToIndex) {
      std::cout << x.first << " " << x.second << "\n";
    }
    std::cout << "\n";

    std::cout << "entity -> count: \n";
    for (auto x : entityCount) {
      std::cout << x.first << " " << x.second << "\n";
    }
    std::cout << "\n";
    std::cout << "index -> entity: \n";
    for (auto x : indexToEntity) {
      std::cout << x.first << " " << x.second << "\n";
    }
  }
};

class ComponentManager {
  //Map from a string pointer to a component type
  std::unordered_map<const char*, ComponentType> componentTypes{};
  //Map from a string pointer to a component array
  std::unordered_map<const char*, std::shared_ptr<IComponentArray>> componentArrays;
  //The component type to be assigned to the next registered component, starting at 0
  ComponentType nextComponentType{};

  template<typename T>
  std::shared_ptr<ComponentArrayUnique<T>> getComponentArrayUnique() {
    const char* typeName = typeid(T).name();
    assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered !");
    return std::static_pointer_cast<ComponentArrayUnique<T>>(componentArrays[typeName]);
  }
  template<typename T>
  std::shared_ptr<ComponentArrayMult<T>> getComponentArrayMult() {
    const char* typeName = typeid(T).name();
    assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered !");
    return std::static_pointer_cast<ComponentArrayMult<T>>(componentArrays[typeName]);
  }

public:
  template<typename T>
  void registerComponentUnique() {
    const char* typeName = typeid(T).name();
    assert(componentTypes.find(typeName) == componentTypes.end() && "Registering component type more than once !");
    //Add this component type to the component type map
    componentTypes.insert({ typeName, nextComponentType });
    //Create a componentArray pointer and add it to the component arrays map
    componentArrays.insert({ typeName , std::make_shared<ComponentArrayUnique<T>>() });
    //Increment the value so that the next component registered will be different
    nextComponentType++;
  }
  template<typename T>
  void registerComponentMult() {
    const char* typeName = typeid(T).name();
    assert(componentTypes.find(typeName) == componentTypes.end() && "Registering component type more than once !");
    //Add this component type to the component type map
    componentTypes.insert({ typeName, nextComponentType });
    //Create a componentArray pointer and add it to the component arrays map
    componentArrays.insert({ typeName , std::make_shared<ComponentArrayMult<T>>() });
    //Increment the value so that the next component registered will be different
    nextComponentType++;
  }

  template <typename T>
  ComponentType getComponentType() {  //if const, it blows up
    const char* typeName = typeid(T).name();
    assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered");
    return componentTypes[typeName];
  }

  //@todo: getComponentMultOrUnique

  template<typename T>
  void add(const Entity entity, T component) {
    getComponentArrayUnique<T>()->add(entity, component);
  }

  template<typename T>
  void addOne(const Entity entity, T component) { //addOne
    //Add a component to the array for an entity
    getComponentArrayMult<T>()->addOne(entity, component);
  }

  template<typename T>
  void addArr(const Entity entity, T* data, const Entity dataSize) {
    getComponentArrayMult<T>()->addArr(entity, data, dataSize);
  }

  template<typename T>
  void remove(const Entity entity) {
    getComponentArrayUnique<T>()->remove(entity);
    //ComponentArrayUnique(*getComponentArray<T>()).remove(entity);
  }

  template<typename T>
  void clear(const Entity entity) {
    getComponentArrayMult<T>()->clear(entity);
    //ComponentArrayMult(*getComponentArray<T>()).clear(entity);
  }

  template<typename T>
  T& get(const Entity entity) {       //Only from Unique container
    return getComponentArrayUnique<T>()->get(entity);
    //return &ComponentArrayUnique(*getComponentArray<T>()).get(entity);
  }

  template <typename T>
  T* getArr(const Entity entity) {      //Only from Mult container
    return getComponentArrayMult<T>()->getArr(entity);
  }

  template<typename T>
  Entity getCount(const Entity entity) {  //Only from Mult container //Non-const
    return getComponentArrayMult<T>()->getCount(entity);
  }

  template<typename T>
  T* getAll() {
    return getComponentArrayMult<T>()->getAll();
  }

  template<typename T>
  Entity getSize() {
    return getComponentArrayMult<T>()->getSize();
  }

  void entityDestroyed(const Entity entity) {
    //Notify each component array that an entity has been destroyed
    //If it there are components for that entity, it will remove them
    for (auto const& pair : componentArrays) {
      auto const& componentArray = pair.second;
      componentArray->entityDestroyed(entity);
    }
  }
};

class System {
public:
  std::set<Entity> entities;
};

class MultSystem {
public:
  std::unordered_map<const char*, std::set<Entity>> entities;

  //USE CASE:
  //for(auto const& entity: entities["with points"]) {
  //  auto& health = getComponent<Health>(entity);
  //  auto* pos1 = getComponents<pos1<(entity);
  //  int pSize = getCount<pos1>(entity);
  //  for(int i=0;i<pSize;i++) {
  //    drawPoint(pos1[i]);

};

class SystemManager {
  //Map from system type string pointer to a signature
  //std::unordered_map<const char*, Signature> signatures{};
  std::unordered_map<const char*, Signature> signatures{};
  std::unordered_map<const char*, std::unordered_map<const char*, Signature>> multSignatures{};
  //Map from a system type string pointer to a system pointer
  std::unordered_map<const char*, std::shared_ptr<System>> systems{};
  std::unordered_map<const char*, std::shared_ptr<MultSystem>> multSystems{};

public:
  template<typename T>
  std::shared_ptr<T> registerSystem() {
    const char* typeName = typeid(T).name();
    assert(systems.find(typeName) == systems.end() && "Registering system more than once !");

    //Create a pointer to the system and return it so it can be used externally
    auto system = std::make_shared<T>();
    systems.insert({ typeName, system });
    return system;
  }

  template<typename T>
  std::shared_ptr<T> registerSystemMult() {
    const char* typeName = typeid(T).name();
    assert(systems.find(typeName) == systems.end() && "Registering system more than once !");

    //Create a pointer to the system and return it so it can be used externally
    auto system = std::make_shared<T>();
    multSystems.insert({ typeName, system });
    return system;
  }

  template<typename T>
  void setSignature(const Signature& signature) {
    const char* typeName = typeid(T).name();
    assert(systems.find(typeName) != systems.end() && "System used before registered.");

    //Set the signature for this system
    signatures[typeName] = signature;
  }

  template<typename T>
  void setSignatureMult(const Signature& signature, const char* entityType) {
    const char* typeName = typeid(T).name();
    assert(systems.find(typeName) != systems.end() && "System used before registered.");

    //Set the signature for this system
    multSignatures[typeName][entityType] = signature;
  }

  void entityDestroyed(const Entity entity) {
    //Erase a destroyed entity from all systems lists
    //entities is a set so no check needed

    for (auto const& pair : systems) {
      auto const& system = pair.second;
      system->entities.erase(entity);
    }

    for (auto const& pair : multSystems) {
      auto const& system = pair.second;

      for (auto const& pair : system->entities) {
        auto const& entityType = pair.first;
        system->entities[entityType].erase(entity);
      }
      
    }
  }
  void entitySignatureChanged(const Entity entity, const Signature& entitySignature) {
    //Notify each system that an entity's signature changed

    for (auto const& pair : systems) {
      auto const& systemName = pair.first;
      auto const& system = pair.second;
      auto const& systemSignature = signatures[systemName];

      if (entitySignature >= systemSignature) {
        system->entities.insert(entity);
      }
      else {
        //Entity signature does not match system signature - erase from set
        system->entities.erase(entity);
      }
    }

    for (auto const& pairOne : multSystems) {
      auto const& systemName = pairOne.first;
      auto const& system = pairOne.second;
      for (auto const& pairTwo : multSignatures[systemName]) {
        auto const& typeName = pairTwo.first;
        //auto const& systemTypeSignature = signatures[systemName][type];
        auto const& typeSignature = pairTwo.second;

        //Entity signature matches system signature - insert into set

        //if ((entitySignature & systemSignature) == systemSignature) {
        if (entitySignature >= typeSignature) {
          system->entities[typeName].insert(entity);
        }
        else {
          //Entity signature does not match system signature - erase from set
          system->entities[typeName].erase(entity);
        }
      }

      //USE CASE: (WITHOUT a coordinator class)
      //int player = entityManager.CreateEntity();
      //componentManager.addComponent<Transform>(player);
      //RenderSystem renderSystem = systemManager.registerSystem<RenderSystem>();

      //USE CASE: (WITH a coordinator class)
      //int player = coordinator.createEntity();
      //coordinator.addComponent<Transform>(player);
      //RenderSystem renderSystem = coordinator.registerSystem<RenderSystem>();
    }
  }
};

class Scene {
  std::unique_ptr<ComponentManager> componentManager;
  std::unique_ptr<EntityManager> entityManager;
  std::unique_ptr<SystemManager> systemManager;

public:
  void init() {
    //Create pointers to each manager
    componentManager = std::make_unique<ComponentManager>();
    entityManager = std::make_unique<EntityManager>();
    systemManager = std::make_unique<SystemManager>();
  }

  //Entity Methods ==============================================//

  Entity createEntity() {
    return entityManager->createEntity();
  }

  void destroyEntity(const Entity entity) {
    entityManager->destroyEntity(entity);
    componentManager->entityDestroyed(entity);
    systemManager->entityDestroyed(entity);
  }

  //Component methods ===========================================//

  template<typename T>
  void registerComponent() {
    componentManager->registerComponentUnique<T>();
  }

  template<typename T>
  void registerComponentMult() {
    componentManager->registerComponentMult<T>();
  }

  template<typename T>
  void add(const Entity entity, T component = T()) {
    componentManager->add<T>(entity, component);

    auto signature = entityManager->getSignature(entity);
    //signature.set(componentManager->getComponentType<T>(), true);
    signature[componentManager->getComponentType<T>()]++;
    entityManager->setSignature(entity, signature);

    systemManager->entitySignatureChanged(entity, signature);
  }

  template<typename T>
  void addOne(const Entity entity, T component = T()) {
    componentManager->addOne<T>(entity, component);

    Signature signature = entityManager->getSignature(entity);
    //signature.set(componentManager->getComponentType<T>(), true);
    signature[componentManager->getComponentType<T>()]++;
    entityManager->setSignature(entity, signature);

    systemManager->entitySignatureChanged(entity, signature);
  }

  //Works only for Mult
  template<typename T>
  void addArr(const Entity entity, T* arr, const Entity size) {
    componentManager->addArr<T>(entity, arr, size);

    Signature signature = entityManager->getSignature(entity);
    //signature.set(componentManager->getComponentType<T>(), true);
    signature[componentManager->getComponentType<T>()] += size;
    entityManager->setSignature(entity, signature);

    systemManager->entitySignatureChanged(entity, signature);
  }

  template<typename T>
  void remove(const Entity entity) {  //removeComponents(const int entity)
    componentManager->remove<T>(entity);

    auto signature = entityManager->getSignature(entity);
    //signature.set(componentManager->getComponentType<T>(), false);
    signature[componentManager->getComponentType<T>()] = 0;
    entityManager->setSignature(entity, signature);

    systemManager->entitySignatureChanged(entity, signature);
  }

  template<typename T>
  void clear(const Entity entity) { //removeComponents(const int entity)
    componentManager->clear<T>(entity);

    auto signature = entityManager->getSignature(entity);
    //signature.set(componentManager->getComponentType<T>(), false);
    signature[componentManager->getComponentType<T>()] = 0;
    entityManager->setSignature(entity, signature);

    systemManager->entitySignatureChanged(entity, signature);
  }

  template<typename T>
  T& get(const Entity entity) {
    return componentManager->get<T>(entity);
  }
  template<typename T>
  T* getArr(const Entity entity) {  //VNIMAVAI S TEQ CONST, POSLE NQMA DA MOJESH DA PROMENQSH DATATA, KOQTO VZIMASH
    return componentManager->getArr<T>(entity);
  }
  template<typename T>
  Entity getCount(const Entity entity) {
    return componentManager->getCount<T>(entity);
  }

  // Only For Mult Component Arrays !
  template<typename T>
  T* getAll() {
    return componentManager->getAll<T>();
  }

  template<typename T>
  Entity getSize() {
    return componentManager->getSize<T>();
  }

  //template<typename T>
  //uint32_t getID(const uint32_t entity) const {
  //  return componentManager->getComponentID;
  //}

  template<typename T>
  Entity getComponentType() {
    return componentManager->getComponentType<T>();
  }

  //System methods ==============================================//

  template<typename T>
  std::shared_ptr<T> registerSystem() {
    return systemManager->registerSystem<T>();
  }

  template<typename T>
  void setSystemSignature(const Signature& signature) {
    return systemManager->setSignature<T>(signature);
  }

  template<typename T>
  void setSystemSignatureMult(const Signature& signature, const char* typeName) {
    return systemManager->setSignatureMult<T>(signature, typeName);
  }

};

//You can make the engine inherit from class Scene and make all the other system classes - friend classes
//Do you care if system signature requerements need to know the number of components for an entity
//Are the entities components iterated in the same order as in the component array ?
//Za sega nqma proverki za tova dali get-va ili set-va values ot Mult ili Unique


//Event System //Private Communication Between Different Systems
//https://medium.com/@savas/nomad-game-engine-part-7-the-event-system-45a809ccb68f    //main post
//https://www.gamedev.net/articles/programming/general-and-gameplay-programming/effective-event-handling-in-c-r2459/  //referenced

class Event
{
protected:
  virtual ~Event() {};
};

class iFunctionHandler {
  virtual void call(Event* evnt) = 0;

public:
  //Call the member function
  void exec(Event* evnt) {
    call(evnt);
  }
};

template<class T, class EventType>
class MemberFunctionHandler : public iFunctionHandler {
public:
  typedef void (T::*MemberFunction)(EventType*);

private:
  T* instance;  //Pointer to a class instance
  MemberFunction memberFunction;  //Pointer to a member function

public:

  MemberFunctionHandler(T* instance, MemberFunction memberFunction) : instance{ instance }, memberFunction{ memberFunction } {};

  void call(Event* evnt) {
    //Cast event to the correct type and call member function
    (instance->*memberFunction)(static_cast<EventType*>(evnt));
  }
};

typedef std::list<iFunctionHandler*> HandlerList;
class EventBus {
  std::map<std::type_index, HandlerList*> subscribers;

public:
  template<typename EventType>
  void publish(EventType* evnt) {
    HandlerList* handlers = subscribers[typeid(EventType)];

    if (handlers == nullptr) return;

    for (auto& handler : *handlers) {
      if (handler != nullptr) {
        handler->exec(evnt);
      }
    }
  }

  template<class T, class EventType>
  void subscribe(T* instance, void (T::*memberFunction)(EventType*)) {
    HandlerList* handlers = subscribers[typeid(EventType)];

    //First time initialization
    if (handlers == nullptr) {
      handlers = new HandlerList();
      subscribers[typeid(EventType)] = handlers;
    }

    handlers->push_back(new MemberFunctionHandler<T, EventType>(instance, memberFunction));
  }
};


extern Scene scene;
//extern Scene stackScene;  // Procedurally Generated Entities with their components are stored here, before loading
//inline Scene scene;

extern EventBus eventBus;

//Further Reading: 
//https://www.david-colson.com/2020/02/09/making-a-simple-ecs.html
