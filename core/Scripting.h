#pragma once

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include <fmt/format.h>

#define GLM_FORCE_RADIANS
#define GLM_FORM_DEPTH_ZERO_TO_ONE
#define GML_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>

namespace Roar {

#define ADD_INTERNAL_CALL(icall) mono_add_internal_call("RoarEngine.InternalCalls::" #icall, (void *)InternalCalls::icall)

template <typename T> using Ref = std::shared_ptr<T>;
template <typename T, typename... Args> constexpr Ref<T> CreateRef(Args &&...args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

class ScriptGlue {
  public:
    static void RegisterFunctions();
};

class ScriptClass {
  public:
    ScriptClass() = default;
    ScriptClass(const std::string &classNamespace, const std::string &className);

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
    ScriptInstance() = default;
    ScriptInstance(Ref<ScriptClass> scriptClass);

    void InvokeOnCreate();
    void InvokeOnUpdate(float ts);

  private:
    Ref<ScriptClass> mScriptClass;

    MonoObject *mInstance = nullptr;
    MonoMethod *mOnCreateMethod = nullptr;
    MonoMethod *mOnUpdateMethod = nullptr;
};

class Scripting {
  public:
    static void Init();
    static void Shutdown();

    static void LoadAssembly(const std::filesystem::path &filepath);
    static MonoObject *InstantiateKlass(MonoClass *klass);

    static std::unordered_map<std::string, Ref<ScriptClass>> GetEntityClasses();
  private:
    static void InitMono();
    static void ShutdownMono();
    static void LoadAssemblyClasses(MonoAssembly *assembly);

    friend class ScriptClass;
};

} // namespace Roar
