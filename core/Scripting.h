#pragma once

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#define GLM_FORCE_RADIANS
#define GLM_FORM_DEPTH_ZERO_TO_ONE
#define GML_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>

namespace rt {

#define ADD_INTERNAL_CALL(icall) mono_add_internal_call("RTEngine.InternalCalls::" #icall, (void *)InternalCalls::icall)

class Scripting {
  public:
    static void Init();
    static void Shutdown();

    static void LoadAssembly(const std::filesystem::path &filepath);
    static MonoObject* InstantiateKlass(MonoClass* klass);

  private:
    static void InitMono();
    static void ShutdownMono();

    friend class ScriptClass;
};

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

} // namespace rt
