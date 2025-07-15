#pragma once

#include <unordered_map>
#include <cassert>
#include <cstdint>
#include <utility>
#include <queue>
#include <array>
#include <set>
#include <memory>

// Event Bus includes
#include <list>
#include <typeindex>
#include <map>

//@todo: Make the ComponentArrays to Dynamicly change Size, but to be also kept tightly together
//      double up the maxSize, or half if down, if an upper or lower boundary is hit

#define MAX_ENTITIES 1024U            //1024
#define MAX_COMPONENT_TYPES uint8_t(32U)    //64
#define MAX_SYSTEM_ARCHTYPES uint8_t(8U)    //8

// DO NOT EXPECT POLYMORPHYC BEHAIVIOR OF A POLYMORPHYC COMPONENT
// TYPE THAT IS STORED IN THE COMPONENT ARRAYS !!!
// reinterpreted_cast should not be used between const and non-const destinations   

template <uint8_t N>
struct NumSet {
  //typedef int arr[N];
  uint32_t arr[N];

  void reset() {
    for (uint8_t i = 0U; i < N; ++i) {
      arr[i] = 0U;
    }
  }
  NumSet() { reset(); }

  uint32_t& operator[](const uint8_t& id) {
    assert(id < N && "id out of range !");
    return arr[id];
  }
  uint32_t operator[](const uint8_t& id) const {
    assert(id < N && "id out of range !");
    return arr[id];
  }

  void set(const uint8_t& id, const uint32_t& value = 1U) {
    assert(id < N && "id out of range !");
    arr[id] = value;
  }
  uint32_t get(const uint8_t& id) const {
    assert(id < N && "Out of range !");
    return arr[id];
  }

  friend bool operator>(const NumSet<N>& lhs, const NumSet<N>& rhs) {
    for (uint8_t i = 0U; i < N; ++i) {
      if (lhs.arr[i] <= rhs.arr[i]) 
        return false;
    }
    return true;
  }
  friend bool operator<(const NumSet<N>& lhs, const NumSet<N>& rhs) {
    for (uint8_t i = 0U; i < N; ++i) {
      if (lhs.arr[i] >= rhs.arr[i])
        return false;
    }
    return true;
  }
  friend bool operator>=(const NumSet<N>& lhs, const NumSet<N>& rhs) {
    for (uint8_t i = 0U; i < N; ++i) {
      if (lhs.arr[i] < rhs.arr[i])
        return false;
    }
    return true;
  }
  friend bool operator<=(const NumSet<N>& lhs, const NumSet<N>& rhs) {
    for (uint8_t i = 0U; i < N; ++i) {
      if (lhs.arr[i] > rhs.arr[i])
        return false;
    }
    return true;
  }
};

using Signature = NumSet<MAX_COMPONENT_TYPES>;

class EntityManager {
  //https://en.cppreference.com/w/cpp/language/zero_initialization, class member {};
  std::queue<uint32_t> availableEntities{};
  std::array<Signature, MAX_ENTITIES> signatures{};
  uint32_t count{}; //counts the living entities

public:
  EntityManager() {
    for (uint32_t entity = 0U; entity < MAX_ENTITIES; ++entity) {
      availableEntities.push(entity);
    }
  }

  void reset() {
    //availableEntities = queue<Entity>();
    while (!availableEntities.empty()) {
      availableEntities.pop();
    }

    for (uint32_t entity = 0U; entity < MAX_ENTITIES; ++entity) {
      availableEntities.push(entity);
    }

    for (auto& signature : signatures) {
      signature.reset();
    }

    count = 0U;
  }

  uint32_t createEntity() {
    assert(count < MAX_ENTITIES && "Too many entities in existence !");
    uint32_t id = availableEntities.front();
    availableEntities.pop();
    count++;

    return id;
  }
  void destroyEntity(const uint32_t& entity) {
    assert(entity < MAX_ENTITIES && "Entity out of range !");

    signatures[entity].reset();
    availableEntities.push(entity);
    count--;
  }
  void setSignature(const uint32_t& entity, const Signature& signature) {
    assert(entity < MAX_ENTITIES && "Entity out of range !");
    signatures[entity] = signature;
  }
  Signature getSignature(const uint32_t& entity) const {      //Non-Const, because it will be changed after that
    assert(entity < MAX_ENTITIES && "Entity out of range !");
    return signatures[entity];
  }

  //Returns the number of living entities
  uint32_t getCount() const {
    return count;
  }
};

// Entity Array Info
//struct EntityArrayInfo {
//  uint32_t index;
//  uint32_t count;
//};

// Component Array Info
struct ComponentArrayInfo {
  char* ptr{};
  uint32_t size{};  // Total size of valid entries in the array.
  uint32_t maxSize{};

  uint32_t typeSize{};

  //std::unordered_map<uint32_t, EntityArrayInfo> entityToInfo;
  std::unordered_map<uint32_t, uint32_t> entityToIndex; //entity id to first index of the array
  std::unordered_map<uint32_t, uint32_t> entityCount; //number of components for this entity id

  ComponentArrayInfo() {}
  ~ComponentArrayInfo() {}

  ComponentArrayInfo(char* Arr, const uint32_t N, const uint32_t TypeSize) {
    ptr = Arr;
    size = 0U;
    maxSize = N;

    typeSize = TypeSize;
  }

  void reset() {
    size = 0U;
    entityToIndex.clear();
    entityCount.clear();
  }
};

class ComponentManager {
  char* data;
  size_t dataSize;    // in bytes
  size_t maxDataSize;   // in bytes
  std::unordered_map<const char*, ComponentArrayInfo> arrayToInfo;

  uint8_t componentTypesCount;
  std::unordered_map<const char*, uint8_t> nameToIndex;

  void copyFrom(const ComponentManager& other) {
    maxDataSize = other.maxDataSize;
    dataSize = other.dataSize;
    data = new char[maxDataSize];
    memcpy(data, other.data, dataSize);
    arrayToInfo = other.arrayToInfo;

    componentTypesCount = other.componentTypesCount;
    nameToIndex = other.nameToIndex;
  }

public:
  ComponentManager() {
    data = nullptr;
    dataSize = 0U;
    maxDataSize = 0U;
    componentTypesCount = 0U;
  }
  ComponentManager(const ComponentManager& other) {
    copyFrom(other);
  }
  ComponentManager& operator=(const ComponentManager& other) {
    if (this != &other) {
      delete[] data;
      copyFrom(other);
    }

    return *this;
  }
  ~ComponentManager() {
    delete[] data;
  }

  // All Current ComponentArray should have their adress moved too !!!
  void reserve(const uint32_t& ReservedDataSize) {
    if (ReservedDataSize > maxDataSize) {
      char* newData = new char[ReservedDataSize];

      memcpy(newData, data, dataSize); //memcpy on a nullptr at the begining
      //for (uint32_t i = 0U; i < dataSize; i++) {
      //  newData[i] = data[i];
      //}

      // Move all array pointers to the new location !!!
      for (auto& info : arrayToInfo) {
        size_t offset = info.second.ptr - data;
        info.second.ptr = newData + offset;
        //info.second.ptr = newData + info.second.ptr - data;
      }

      delete[] data;
      data = newData;
      maxDataSize = ReservedDataSize;
    }
  }

  /*template<typename T>
  void reset() {
    const char* typeName = typeid(T).name();
    assert(arrayToInfo.find(typeName) != arrayToInfo.end() && "Component not registered !");
    ComponentArrayInfo& info = arrayToInfo[typeName];

    info.reset();
  }*/

  void reset() {
    for (auto const& pair : arrayToInfo) {
      arrayToInfo[pair.first].reset();
    }
  }

  template<typename T>
  void registerComponent(const uint32_t& ArraySize) {
    const char* typeName = typeid(T).name();
    assert(arrayToInfo.find(typeName) == arrayToInfo.end() && "Component is Already Registered !");
    assert(componentTypesCount < MAX_COMPONENT_TYPES && "MAX_COMPONENT_TYPES LIMIT REACHED !");

    size_t bytesCount = sizeof(T) * ArraySize;
    //std::cout << "bytesCount: " << typeid(T).name() << " :" << bytesCount << "\n";
    reserve(dataSize + bytesCount);

    //arrayToInfo[typeName] = ComponentArrayInfo(data + dataSize * sizeof(T), ArraySize, sizeof(T));
    arrayToInfo[typeName] = ComponentArrayInfo(data + dataSize, ArraySize, sizeof(T));
    dataSize += bytesCount;

    nameToIndex[typeName] = componentTypesCount;
    ++componentTypesCount;
  }

  template<typename T>
  //void add(const uint32_t entity, T component = T()) {
  //void add(const uint32_t entity, T& component = T()) {
  void add(const uint32_t entity, const T& component = T()) {
    const char* typeName = typeid(T).name();
    assert(arrayToInfo.find(typeName) != arrayToInfo.end() && "Component not registered !");
    ComponentArrayInfo& info = arrayToInfo[typeName];

    assert(info.size < info.maxSize && "No space left in the component array !");

    if (info.entityToIndex.find(entity) == info.entityToIndex.end()) {
      //There hasnt been components of this type for that entity
      info.entityToIndex[entity] = info.size;
      info.entityCount[entity] = 0U;
      //Places the last entity's data in the back of the array
    }

    // Free Space for the new obj
    char* src = info.ptr + (info.entityToIndex[entity] + info.entityCount[entity]) * info.typeSize;
    char* dst = src + info.typeSize;
    size_t restDataSize = (info.size - info.entityToIndex[entity] - info.entityCount[entity]) * info.typeSize;
    memcpy(dst, src, restDataSize);

    // Place the new object in the array
    memcpy(src, &component, info.typeSize);

    info.entityCount[entity]++;
    info.size++;
  }

  template<typename T>
  void addArr(const uint32_t entity, const T* data, const uint32_t& dataCount) {
    const char* typeName = typeid(T).name();
    assert(arrayToInfo.find(typeName) != arrayToInfo.end() && "Component not registered !");
    ComponentArrayInfo& info = arrayToInfo[typeName];

    assert(info.size + dataCount <= info.maxSize && "Too many components in the array !");  // + typeid(T).name()

    if (info.entityToIndex.find(entity) == info.entityToIndex.end()) {
      //There hasnt been components of this type for that entity
      info.entityToIndex[entity] = info.size;
      info.entityCount[entity] = 0U;    //I think that by default its 0, but just to be sure...
    }

    // Free Space for the new array
    size_t dataBytes = dataCount * info.typeSize;
    char* src = info.ptr + (info.entityToIndex[entity] + info.entityCount[entity]) * info.typeSize;
    char* dst = src + dataBytes;
    size_t restDataSize = (info.size - info.entityToIndex[entity] - info.entityCount[entity]) * info.typeSize;
    memcpy(dst, src, restDataSize);

    // Place the new array into the current array
    memcpy(src, data, dataBytes);

    info.entityCount[entity] += dataCount;
    info.size += dataCount;
  }

  template<typename T>
  void remove(const uint32_t& entity, const uint32_t& HowMany) {
    const char* typeName = typeid(T).name();
    assert(arrayToInfo.find(typeName) != arrayToInfo.end() && "Component not registered !");
    ComponentArrayInfo& info = arrayToInfo[typeName];

    // ?? assert, if check, or clamping the HowMuch value ?? //

    uint32_t n = std::min(HowMany, info.entityCount[entity]);
    if (info.entityToIndex.find(entity) != info.entityToIndex.end()) {
    //if (info.entityToIndex.find(entity) != info.entityToIndex.end() && HowMany <= info.entityCount[entity]) {

      char* src = info.ptr + (info.entityToIndex[entity] + info.entityCount[entity]) * info.typeSize;
      char* dst = src - n * info.typeSize;
      size_t restDataSize = (info.size - info.entityToIndex[entity] - info.entityCount[entity]) * info.typeSize;
      memcpy(dst, src, restDataSize);

      if (n == info.entityCount[entity]) {
        info.entityToIndex.erase(entity);
        info.entityCount.erase(entity);
      }
      else {
        info.entityCount[entity] -= n;
      }

      info.size -= n;
    }
  }

  // This method is the reason to store the typeSize for the component arrays
  void remove(const uint32_t& entity, const char* typeName) {
    assert(arrayToInfo.find(typeName) != arrayToInfo.end() && "Component not registered !");
    ComponentArrayInfo& info = arrayToInfo[typeName];

    if (info.entityToIndex.find(entity) != info.entityToIndex.end()) {  // && info.entityCount[entity] != 0U
      char* src = info.ptr + (info.entityToIndex[entity] + info.entityCount[entity]) * info.typeSize;
      char* dst = src - info.entityCount[entity] * info.typeSize;
      size_t restDataSize = (info.size - info.entityToIndex[entity] - info.entityCount[entity]) * info.typeSize;
      memcpy(dst, src, restDataSize);

      info.size -= info.entityCount[entity];
      info.entityToIndex.erase(entity);
      info.entityCount.erase(entity);
    }
  }

  template<typename T>
  void remove(const uint32_t& entity) {
    const char* typeName = typeid(T).name();
    remove(entity, typeName);
  }

  void entityDestroyed(const uint32_t& entity) {
    for (auto const& pair : arrayToInfo) {
      const char* typeName = pair.first;
      remove(entity, typeName);
    }
  }

  template<typename T>
  //T* get(const uint32_t& entity) {
  T& get(const uint32_t& entity) {
    const char* typeName = typeid(T).name();
    assert(arrayToInfo.find(typeName) != arrayToInfo.end() && "Component not registered !");
    ComponentArrayInfo& info = arrayToInfo[typeName];

    assert(info.entityToIndex.find(entity) != info.entityToIndex.end() && "Retrieving a Non Existent Component !");
    assert(info.entityCount[entity] == 1U && "There is More than One Component for this entity in the component array !");
    return *reinterpret_cast<T*>(info.ptr + info.entityToIndex[entity] * info.typeSize);
  }

  template<typename T>
  T* getArr(const uint32_t& entity) {
    const char* typeName = typeid(T).name();
    assert(arrayToInfo.find(typeName) != arrayToInfo.end() && "Component not registered !");
    ComponentArrayInfo& info = arrayToInfo[typeName];

    //assert(info.entityToIndex.find(entity) != info.entityToIndex.end() && "Retrieving a Non Existent Component !");
    if (info.entityToIndex.find(entity) == info.entityToIndex.end())
      return nullptr;

    return reinterpret_cast<T*>(info.ptr + info.entityToIndex[entity] * info.typeSize);
  }

  template<typename T>
  uint32_t getCount(const uint32_t& entity) { //const
    const char* typeName = typeid(T).name();
    assert(arrayToInfo.find(typeName) != arrayToInfo.end() && "Component not registered !");
    ComponentArrayInfo& info = arrayToInfo[typeName];

    if (info.entityCount.find(entity) != info.entityCount.end())
      return info.entityCount[entity];

    return 0U;
  }

  template<typename T>
  T* getAll() {
    const char* typeName = typeid(T).name();
    assert(arrayToInfo.find(typeName) != arrayToInfo.end() && "Component not registered !");
    ComponentArrayInfo& info = arrayToInfo[typeName];

    T* getPtr = reinterpret_cast<T*>(info.ptr);
    //assert(getPtr != nullptr && "Component Not Assigned !");
    return getPtr;
  }

  template<typename T>
  uint32_t getSize() {  //const
    const char* typeName = typeid(T).name();
    assert(arrayToInfo.find(typeName) != arrayToInfo.end() && "Component not registered !");
    ComponentArrayInfo& info = arrayToInfo[typeName];
    //const ComponentArrayInfo& cArrInfo = arrayToInfo[typeName];
    //const auto& cArrInfo = arrayToInfo[typeName];

    return info.size;
  }

  template<typename T>
  uint8_t getComponentType() {
    const char* typeName = typeid(T).name();
    assert(nameToIndex.find(typeName) != nameToIndex.end() && "Component not registered !");

    return nameToIndex[typeName];
  }

  template<typename T>
  bool doesHave(const uint32_t& entity) { //const
    const char* typeName = typeid(T).name();
    assert(arrayToInfo.find(typeName) != arrayToInfo.end() && "Component not registered !");
    //ComponentArrayInfo& info = arrayToInfo[typeName];
    const ComponentArrayInfo& info = arrayToInfo[typeName];

    return info.entityToIndex.find(entity) != info.entityToIndex.end();
  }

  //uint32_t getSizeInBytes() const { return dataSize; }
  //uint32_t getMaxSizeInBytes() const { return maxDataSize; }
};

//======================================================================================//

class System {
public:
  std::set<uint32_t> entities;
};

class MultSystem {
public:
  std::unordered_map<const char*, std::set<uint32_t>> archtypes;
  //std::array<std::set<uint32_t>, MAX_SYSTEM_ARCHTYPES> archtypes;

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

  void reset() {
    for (auto const& pair : systems) {
      auto const& system = pair.second;
      system->entities.clear();
    }

    for (auto const& pair1 : multSystems) {
      auto const& system = pair1.second;

      for (auto const& pair2 : system->archtypes) {
        auto const& entityType = pair2.first;
        //system->archtypes[entityType].erase(entity);
        system->archtypes[entityType].clear();
      }

    }
  }

  void entityDestroyed(const uint32_t& entity) {
    //Erase a destroyed entity from all systems lists
    //entities is a set so no check needed

    for (auto const& pair : systems) {
      auto const& system = pair.second;
      system->entities.erase(entity);
    }

    for (auto const& pair1 : multSystems) {
      auto const& system = pair1.second;

      for (auto const& pair2 : system->archtypes) {
        auto const& entityType = pair2.first;
        system->archtypes[entityType].erase(entity);
      }

    }
  }

  void entitySignatureChanged(const uint32_t& entity, const Signature& entitySignature) {
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
          system->archtypes[typeName].insert(entity);
        }
        else {
          //Entity signature does not match system signature - erase from set
          system->archtypes[typeName].erase(entity);
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

//==================================================================================//

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

  uint32_t createEntity() {
    return entityManager->createEntity();
  }

  void destroyEntity(const uint32_t& entity) {
    entityManager->destroyEntity(entity);
    componentManager->entityDestroyed(entity);
    systemManager->entityDestroyed(entity);
  }

  uint32_t getEntityCount() const {
    return entityManager->getCount();
  }

  //Component methods ===========================================//

  template<typename T>
  void registerComponent(const uint32_t& MaxElements) {
    componentManager->registerComponent<T>(MaxElements);
  }

  template<typename T>
  void add(const uint32_t& entity, const T& component = T()) {
    componentManager->add<T>(entity, component);

    auto signature = entityManager->getSignature(entity);
    //signature.set(componentManager->getComponentType<T>(), true);
    signature[componentManager->getComponentType<T>()]++;
    entityManager->setSignature(entity, signature);

    systemManager->entitySignatureChanged(entity, signature);
  }

  //Works only for Mult
  template<typename T>
  void addArr(const uint32_t& entity, T* arr, const uint32_t& size) {
    componentManager->addArr<T>(entity, arr, size);

    Signature signature = entityManager->getSignature(entity);
    //signature.set(componentManager->getComponentType<T>(), true);
    signature[componentManager->getComponentType<T>()] += size;
    entityManager->setSignature(entity, signature);

    systemManager->entitySignatureChanged(entity, signature);
  }

  template<typename T>
  void remove(const uint32_t& entity) { //removeComponents(const int entity)
    componentManager->remove<T>(entity);

    auto signature = entityManager->getSignature(entity);
    //signature.set(componentManager->getComponentType<T>(), false);
    signature[componentManager->getComponentType<T>()] = 0;
    entityManager->setSignature(entity, signature);

    systemManager->entitySignatureChanged(entity, signature);
  }

  template<typename T>
  void remove(const uint32_t& entity, const uint32_t& HowMany) {  //removeComponents(const int entity)
    componentManager->remove<T>(entity, HowMany);

    auto signature = entityManager->getSignature(entity);
    //signature.set(componentManager->getComponentType<T>(), false);
    signature[componentManager->getComponentType<T>()] = 0;
    entityManager->setSignature(entity, signature);

    systemManager->entitySignatureChanged(entity, signature);
  }

  // Resets the Scene, but it keeps the registered systems and componentTypes
  void reset() {
    entityManager->reset();
    systemManager->reset();
    componentManager->reset();
  }

  template<typename T>
  T& get(const uint32_t& entity) {
    return componentManager->get<T>(entity);
  }
  template<typename T>
  T* getArr(const uint32_t& entity) { //VNIMAVAI S TEQ CONST, POSLE NQMA DA MOJESH DA PROMENQSH DATATA, KOQTO VZIMASH
    return componentManager->getArr<T>(entity);
  }
  template<typename T>
  uint32_t getCount(const uint32_t& entity) const {
    return componentManager->getCount<T>(entity);
  }

  template<typename T>
  T* getAll() {
    return componentManager->getAll<T>();
  }

  template<typename T>
  uint32_t getSize() const {
    return componentManager->getSize<T>();
  }

  //template<typename T>
  //uint32_t getID(const uint32_t entity) const {
  //  return componentManager->getComponentID;
  //}

  template<typename T>
  uint8_t getComponentType() const {
    return componentManager->getComponentType<T>();
  }

  /*template<typename T>
  bool doesHave(const uint32_t& entity) const {
    return componentManager->doesHave<T>(entity);
  }*/

  template <typename T>
  bool doesHave(const uint32_t& entity, const uint32_t& HowMany = 1U) const {
    const Signature& signature = entityManager->getSignature(entity);
    const uint8_t& compType = getComponentType<T>();

    //return signature[compType] != 0U;
    return signature[compType] >= HowMany;
  }

  //System methods ==============================================//

  template<typename T>
  std::shared_ptr<T> registerSystem() {
    return systemManager->registerSystem<T>();
  }

  template<typename T>
  std::shared_ptr<T> registerSystemMult() {
    return systemManager->registerSystemMult<T>();
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

extern Scene scene;

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
  typedef void (T::* MemberFunction)(EventType*);

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
  void subscribe(T* instance, void (T::* memberFunction)(EventType*)) {
    HandlerList* handlers = subscribers[typeid(EventType)];

    //First time initialization
    if (handlers == nullptr) {
      handlers = new HandlerList();
      subscribers[typeid(EventType)] = handlers;
    }

    handlers->push_back(new MemberFunctionHandler<T, EventType>(instance, memberFunction));
  }
};

extern EventBus eventBus;

//Further Reading: 
//https://www.david-colson.com/2020/02/09/making-a-simple-ecs.html
