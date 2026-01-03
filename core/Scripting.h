#pragma once

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <functional>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#define GLM_FORCE_RADIANS
#define GLM_FORM_DEPTH_ZERO_TO_ONE
#define GML_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>

#include "Common.h"
#include "ECS.h"
#include "Component.h"
#include "raylib.h"

namespace Roar {

#define ADD_INTERNAL_CALL(icall) mono_add_internal_call("RoarEngine.InternalCalls::" #icall, (void *)InternalCalls::icall)

class ScriptGlue {
  public:
    static void RegisterFunctions();
    static void RegisterComponents();
};

class ScriptClass {
  public:
    ScriptClass() = default;
    ScriptClass(const std::string &classNamespace, const std::string &className, bool isCore = false);

    MonoObject *Instantiate();
    MonoMethod *GetMethod(const std::string &name, int parameterCount);
    MonoObject *InvokeMethod(MonoObject *instance, MonoMethod *monoMethod, void **params = nullptr);

  private:
    std::string mClassNamespace;
    std::string mClassName;
    MonoClass *mMonoClass = nullptr;
};

class ScriptInstance {
  public:
    ScriptInstance(Ref<ScriptClass> scriptClass, uint32_t entity);

    void InvokeOnCreate();
    void InvokeOnUpdate(float ts);

  private:
    Ref<ScriptClass> mScriptClass;

    MonoObject *mInstance = nullptr;
    MonoMethod *mConstructor = nullptr;
    MonoMethod *mOnCreateMethod = nullptr;
    MonoMethod *mOnUpdateMethod = nullptr;
};

class Scripting {
  public:
    static void Init(bool isEditor, std::string gameName = "");
    static void Shutdown();

    static void LoadAssembly(const std::filesystem::path &filepath);
    static void LoadAppAssembly(const std::filesystem::path &filepath);
    static MonoObject *InstantiateKlass(MonoClass *klass);
    static std::unordered_map<std::string, Ref<ScriptClass>> GetEntityClasses();
    static bool EntityClassExists(const std::string& fullClassName);
    static void OnCreateEntity(uint32_t entity);
    static void OnUpdateEntity(uint32_t entity, float ts);
    static void SetScene(Ref<Scene> scene);
    static void UnsetScene();
    static Ref<Scene> GetSceneContext();
    static MonoImage *GetAssemblyImage();
  private:
    static void InitMono();
    static void ShutdownMono();
    static void LoadAssemblyClasses();

    friend class ScriptClass;
    friend class ScriptGlue;
};

} // namespace Roar
