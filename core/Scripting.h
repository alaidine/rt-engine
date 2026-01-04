#pragma once

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#define GLM_FORCE_RADIANS
#define GLM_FORM_DEPTH_ZERO_TO_ONE
#define GML_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>

#include "Common.h"
#include "Component.h"
#include "ECS.h"
#include "raylib.h"

namespace Roar {

#define ADD_INTERNAL_CALL(icall) mono_add_internal_call("RoarEngine.InternalCalls::" #icall, (void *)InternalCalls::icall)

class ScriptGlue {
  public:
    static void RegisterFunctions();
    static void RegisterComponents();
};

enum class ScriptFieldType { None = 0, Float, Vector2, Int, UInt, Bool, Double, Short, Byte, Entity };

struct ScriptField {
    ScriptFieldType type;
    std::string name;
    MonoClassField *field;
};

class ScriptClass {
  public:
    ScriptClass() = default;
    ScriptClass(const std::string &classNamespace, const std::string &className, bool isCore = false);

    MonoObject *Instantiate();
    MonoMethod *GetMethod(const std::string &name, int parameterCount);
    MonoObject *InvokeMethod(MonoObject *instance, MonoMethod *monoMethod, void **params = nullptr);

    const std::unordered_map<std::string, ScriptField> &GetFields() const { return mFields; }

  private:
    std::string mClassNamespace;
    std::string mClassName;

    std::unordered_map<std::string, ScriptField> mFields;

    MonoClass *mMonoClass = nullptr;

    friend class Scripting;
};

class ScriptInstance {
  public:
    ScriptInstance(Ref<ScriptClass> scriptClass, uint32_t entity);

    void InvokeOnCreate();
    void InvokeOnUpdate(float ts);
    Ref<ScriptClass> GetScriptClass() { return mScriptClass; }

    template <typename T> T GetFieldValue(const std::string &name) {
        bool success = GetFieldValueInternal(name, sFieldValueBuffer);
        if (!success)
            return T();
        return *(T*)sFieldValueBuffer;
    }

    template <typename T> void SetFieldValue(const std::string &name, const T& value) {
        SetFieldValueInternal(name, &value);
    }

  private:
    bool GetFieldValueInternal(const std::string &name, void *buffer);
    bool SetFieldValueInternal(const std::string &name, const void *value);

    Ref<ScriptClass> mScriptClass;

    MonoObject *mInstance = nullptr;
    MonoMethod *mConstructor = nullptr;
    MonoMethod *mOnCreateMethod = nullptr;
    MonoMethod *mOnUpdateMethod = nullptr;

    inline static char sFieldValueBuffer[8];
};

class Scripting {
  public:
    static void Init(bool isEditor, std::string gameName = "");
    static void Shutdown();

    static void LoadAssembly(const std::filesystem::path &filepath);
    static void LoadAppAssembly(const std::filesystem::path &filepath);
    static MonoObject *InstantiateKlass(MonoClass *klass);
    static std::unordered_map<std::string, Ref<ScriptClass>> GetEntityClasses();
    static bool EntityClassExists(const std::string &fullClassName);
    static void OnCreateEntity(uint32_t entity);
    static void OnUpdateEntity(uint32_t entity, float ts);
    static void SetScene(Ref<Scene> scene);
    static void UnsetScene();
    static Ref<Scene> GetSceneContext();
    static MonoImage *GetAssemblyImage();
    static Ref<ScriptInstance> GetEntityScriptInstance(uint32_t entity);

  private:
    static void InitMono(bool isEditor);
    static void ShutdownMono();
    static void LoadAssemblyClasses();

    friend class ScriptClass;
    friend class ScriptGlue;
};

} // namespace Roar
