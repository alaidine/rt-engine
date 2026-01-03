#include "Scripting.h"
#include <filesystem>

namespace Roar {

std::unordered_map<MonoType *, std::function<bool(uint32_t)>> sEntityHasComponentFuncs;

namespace Utils {

static void PrintAssemblyTypes(MonoAssembly *assembly) {
    MonoImage *image = mono_assembly_get_image(assembly);
    const MonoTableInfo *typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
    int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

    for (int32_t i = 0; i < numTypes; i++) {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

        const char *nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
        const char *name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

        printf("%s.%s\n", nameSpace, name);
    }
}

static char *ReadBytes(const std::filesystem::path &filepath, uint32_t *outSize) {
    std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

    if (!stream) {
        // Failed to open the file
        return nullptr;
    }

    std::streampos end = stream.tellg();
    stream.seekg(0, std::ios::beg);
    uint32_t size = end - stream.tellg();

    if (size == 0) {
        // File is empty
        return nullptr;
    }

    char *buffer = new char[size];
    stream.read((char *)buffer, size);
    stream.close();

    *outSize = size;
    return buffer;
}

static MonoAssembly *LoadCSharpAssembly(const std::filesystem::path &assemblyPath) {
    uint32_t fileSize = 0;
    char *fileData = ReadBytes(assemblyPath, &fileSize);

    // NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to
    // the assembly
    MonoImageOpenStatus status;
    MonoImage *image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

    if (status != MONO_IMAGE_OK) {
        const char *errorMessage = mono_image_strerror(status);
        // Log some error message using the errorMessage data
        return nullptr;
    }

    MonoAssembly *assembly = mono_assembly_load_from_full(image, assemblyPath.string().c_str(), &status, 0);
    mono_image_close(image);

    // Don't forget to free the file data
    delete[] fileData;

    return assembly;
}

} // namespace Utils

struct ScriptingData {
    MonoDomain *RootDomain = nullptr;
    MonoDomain *AppDomain = nullptr;

    MonoAssembly *CoreAssembly = nullptr;
    MonoImage *CoreAssemblyImage = nullptr;

    MonoAssembly *AppAssembly = nullptr;
    MonoImage *AppAssemblyImage = nullptr;

    ScriptClass EntityClass;
    std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;
    std::unordered_map<uint32_t, Ref<ScriptInstance>> EntityInstances;
    Ref<Scene> SceneContext;
};

static ScriptingData *sData = nullptr;

void Scripting::Init(bool isEditor, std::string gameName) {
    sData = new ScriptingData;
    InitMono(isEditor);

    if (isEditor == false) {
        std::string projectName = std::filesystem::current_path().filename().string();
        std::filesystem::path gameData = std::filesystem::current_path() / "GameData";
        std::string corePath = (gameData / "RoarScriptCore.dll").string();
        std::string gamePath = (gameData / (projectName + ".dll")).string();

        LoadAssembly(corePath);
        LoadAppAssembly(gamePath);
    }
    else {
        LoadAssembly("RoarScriptCore.dll");
        LoadAppAssembly("RoarSandbox.dll");
    }

    LoadAssemblyClasses();

    ScriptGlue::RegisterComponents();
    ScriptGlue::RegisterFunctions();

    sData->EntityClass = ScriptClass("RoarEngine", "Entity", true);
#if 0

    // 2. Call function
    MonoMethod *printMessageFunc = sData->EntityClass.GetMethod("PrintMessage", 0);
    mono_runtime_invoke(printMessageFunc, instance, nullptr, nullptr);
    sData->EntityClass.InvokeMethod(instance, printMessageFunc);

    // 3. Call function with param
    MonoMethod *printIntFunc = sData->EntityClass.GetMethod("PrintInt", 1);

    int value = 3;
    void *param = &value;

    sData->EntityClass.InvokeMethod(instance, printIntFunc, &param);
    MonoMethod *printIntsFunc = sData->EntityClass.GetMethod("PrintInts", 2);

    int value1 = 1;
    int value2 = 2;
    void *params[2] = {
        &value1,
        &value2,
    };

    sData->EntityClass.InvokeMethod(instance, printIntsFunc, params);

    MonoString *monoString = mono_string_new(sData->AppDomain, "Hello World from C++!!!");
    MonoMethod *printCustomMessageFunc = sData->EntityClass.GetMethod("PrintCustomMessage", 1);

    void *stringParam = monoString;
    sData->EntityClass.InvokeMethod(instance, printCustomMessageFunc, &stringParam);
#endif
}

void Scripting::LoadAssemblyClasses() {
    sData->EntityClasses.clear();

    const MonoTableInfo *typeDefinitionsTable = mono_image_get_table_info(sData->AppAssemblyImage, MONO_TABLE_TYPEDEF);
    int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
    MonoClass *entityClass = mono_class_from_name(sData->CoreAssemblyImage, "RoarEngine", "Entity");

    for (int32_t i = 0; i < numTypes; i++) {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

        const char *nameSpace = mono_metadata_string_heap(sData->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
        const char *name = mono_metadata_string_heap(sData->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);
        std::string fullName;

        if (strlen(nameSpace) != 0) {
            fullName = fmt::format("{}.{}", nameSpace, name);
        } else {
            fullName = name;
        }

        MonoClass *monoClass = mono_class_from_name(sData->AppAssemblyImage, nameSpace, name);

        if (monoClass == entityClass) {
            continue;
        }

        bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
        if (isEntity) {
            sData->EntityClasses[fullName] = CreateRef<ScriptClass>(nameSpace, name);
        }

        printf("%s.%s\n", nameSpace, name);
    }
}

MonoImage *Scripting::GetAssemblyImage() { return sData->CoreAssemblyImage; }

void Scripting::Shutdown() {
    ShutdownMono();
    delete sData;
}

void Scripting::LoadAssembly(const std::filesystem::path &filepath) {
    // Create an App Domain
    sData->AppDomain = mono_domain_create_appdomain((char *)"MyAppDomain", nullptr);
    mono_domain_set(sData->AppDomain, true);

    sData->CoreAssembly = Utils::LoadCSharpAssembly(filepath.c_str());
    sData->CoreAssemblyImage = mono_assembly_get_image(sData->CoreAssembly);

    Utils::PrintAssemblyTypes(sData->CoreAssembly);
}

void Scripting::LoadAppAssembly(const std::filesystem::path &filepath) {
    sData->AppAssembly = Utils::LoadCSharpAssembly(filepath.c_str());
    sData->AppAssemblyImage = mono_assembly_get_image(sData->AppAssembly);
}

MonoObject *Scripting::InstantiateKlass(MonoClass *klass) {
    MonoObject *instance = mono_object_new(sData->AppDomain, klass);
    mono_runtime_object_init(instance);
    return instance;
}

std::unordered_map<std::string, Ref<ScriptClass>> Scripting::GetEntityClasses() { return sData->EntityClasses; }

void Scripting::InitMono(bool isEditor) {
    if (isEditor == false) {
        std::string projectName = std::filesystem::current_path().filename().string();
        std::filesystem::path gameData = std::filesystem::current_path() / "GameData";

        mono_set_dirs((gameData / "mono/lib/").string().c_str(), (gameData / "mono/etc/").string().c_str());
    } else {
        mono_set_dirs("mono/lib/", "mono/etc/");
    }

    MonoDomain *rootDomain = mono_jit_init("MyScriptRuntime");
    if (rootDomain == nullptr) {
        return;
    }

    // Store the root domain pointer
    sData->RootDomain = rootDomain;
}

void Scripting::ShutdownMono() {
    // Mono is a little confusing to shutdown.
    // mono_domain_unload(sData->AppDomain);
    // mono_jit_cleanup(sData->RootDomain);

    sData->AppDomain = nullptr;
    sData->RootDomain = nullptr;
}

bool Scripting::EntityClassExists(const std::string &fullClassName) {
    return sData->EntityClasses.find(fullClassName) != sData->EntityClasses.end();
}

void Scripting::OnCreateEntity(uint32_t entity) {
    const auto &sc = sData->SceneContext->GetComponent<ScriptComponent>(entity);
    if (EntityClassExists(sc.name)) {
        Ref<ScriptInstance> instance = sData->EntityInstances[entity] =
            CreateRef<ScriptInstance>(sData->EntityClasses[sc.name], entity);
        sData->EntityInstances[entity] = instance;
        instance->InvokeOnCreate();
    }
}

void Scripting::OnUpdateEntity(uint32_t entity, float ts) {
    Ref<ScriptInstance> instance = sData->EntityInstances[entity];
    instance->InvokeOnUpdate(ts);
}

void Scripting::SetScene(Ref<Scene> scene) { sData->SceneContext = scene; }

void Scripting::UnsetScene() {
    sData->SceneContext = nullptr;
    sData->EntityInstances.clear();
}

Ref<Scene> Scripting::GetSceneContext() { return sData->SceneContext; }

namespace InternalCalls {

static void NativeLog(MonoString *string, int parameter) {
    char *cStr = mono_string_to_utf8(string);
    std::string str(cStr);
    mono_free(cStr);
    std::cout << str << ", " << parameter << std::endl;
}

static void NativeLogVector2(glm::vec2 *vec, glm::vec2 *out) {
    std::cout << vec->x << ", " << vec->y << std::endl;
    *out = glm::vec2(42.0f, 42.0f);
}

static float NativeLogVectorDot(glm::vec2 *vec) { return glm::dot(*vec, *vec); }

static void TransformComponent_GetTranslation(uint32_t entity, glm::vec2 *outTranslation) {
    Ref<Scene> scene = Scripting::GetSceneContext();
    TransformComponent &transform = sData->SceneContext->GetComponent<TransformComponent>(entity);
    *outTranslation = transform.pos;
}

static void TransformComponent_SetTranslation(uint32_t entity, glm::vec2 *translation) {
    Ref<Scene> scene = Scripting::GetSceneContext();
    sData->SceneContext->GetComponent<TransformComponent>(entity).pos = *translation;
}

static bool Input_IsKeyDown(int keycode) { return IsKeyDown(keycode); }

static bool Entity_HasComponent(uint32_t entity, MonoReflectionType *componentType) {
    MonoType *managedType = mono_reflection_type_get_type(componentType);

    return sEntityHasComponentFuncs.at(managedType)(entity);
}

} // namespace InternalCalls

void ScriptGlue::RegisterFunctions() {
    ADD_INTERNAL_CALL(NativeLog);
    ADD_INTERNAL_CALL(NativeLogVector2);
    ADD_INTERNAL_CALL(NativeLogVectorDot);
    ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
    ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
    ADD_INTERNAL_CALL(Input_IsKeyDown);
    ADD_INTERNAL_CALL(Entity_HasComponent);
}

template <typename Component> static void RegisterComponent() {
    std::string_view typeName = typeid(Component).name();
    size_t pos = typeName.find_last_of(':');
    std::string_view structName = typeName.substr(pos + 1);
    std::string managedTypename = fmt::format("RoarEngine.{}", structName);
    MonoType *managedType = mono_reflection_type_from_name(managedTypename.data(), Scripting::GetAssemblyImage());
    sEntityHasComponentFuncs[managedType] = [](uint32_t entity) {
        Ref<Scene> scene = Scripting::GetSceneContext();
        Signature signature = scene->EntityGetSignature(entity);
        return signature.test(scene->GetComponentType<Component>());
    };
}

void ScriptGlue::RegisterComponents() { RegisterComponent<TransformComponent>(); }

ScriptClass::ScriptClass(const std::string &classNamespace, const std::string &className, bool isCore)
    : mClassNamespace(classNamespace), mClassName(className) {
    mMonoClass = mono_class_from_name(isCore ? sData->CoreAssemblyImage : sData->AppAssemblyImage, mClassNamespace.c_str(),
                                      mClassName.c_str());
}

MonoObject *ScriptClass::Instantiate() { return Scripting::InstantiateKlass(mMonoClass); }

MonoMethod *ScriptClass::GetMethod(const std::string &name, int parameterCount) {
    return mono_class_get_method_from_name(mMonoClass, name.c_str(), parameterCount);
}

MonoObject *ScriptClass::InvokeMethod(MonoObject *instance, MonoMethod *monoMethod, void **params) {
    return mono_runtime_invoke(monoMethod, instance, params, nullptr);
}

ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, uint32_t entity) : mScriptClass(scriptClass) {
    mInstance = mScriptClass->Instantiate();
    mConstructor = sData->EntityClass.GetMethod(".ctor", 1);
    mOnCreateMethod = mScriptClass->GetMethod("OnCreate", 0);
    mOnUpdateMethod = mScriptClass->GetMethod("OnUpdate", 1);

    {
        void *param = &entity;
        mScriptClass->InvokeMethod(mInstance, mConstructor, &param);
    }
}

void ScriptInstance::InvokeOnCreate() { mScriptClass->InvokeMethod(mInstance, mOnCreateMethod); }

void ScriptInstance::InvokeOnUpdate(float ts) {
    void *param = &ts;
    mScriptClass->InvokeMethod(mInstance, mOnUpdateMethod, &param);
}

} // namespace Roar
