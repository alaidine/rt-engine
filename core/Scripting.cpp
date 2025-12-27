#include "Scripting.h"

namespace rt {

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
    MonoAssembly *AppAssembly = nullptr;
    MonoImage *AppAssemblyImage = nullptr;
    ScriptClass EntityClass;
};

static ScriptingData *sData = nullptr;

void Scripting::Init() {
    sData = new ScriptingData;
    InitMono();
    LoadAssembly("rtmodule.dll");

    ScriptGlue::RegisterFunctions();

    // 1. Create an object (and call constructor)
    sData->EntityClass = ScriptClass("RTEngine", "Entity");
    MonoObject *instance = sData->EntityClass.Instantiate();

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
}

void Scripting::Shutdown() {
    ShutdownMono();
    delete sData;
}

void Scripting::LoadAssembly(const std::filesystem::path &filepath) {
    // Create an App Domain
    sData->AppDomain = mono_domain_create_appdomain((char *)"MyAppDomain", nullptr);
    mono_domain_set(sData->AppDomain, true);

    sData->AppAssembly = Utils::LoadCSharpAssembly(filepath.c_str());
    sData->AppAssemblyImage = mono_assembly_get_image(sData->AppAssembly);

    // PrintAssemblyTypes(sData->AppAssembly);
}

MonoObject *Scripting::InstantiateKlass(MonoClass *klass) {
    MonoObject *instance = mono_object_new(sData->AppDomain, klass);
    mono_runtime_object_init(instance);
    return instance;
}

void Scripting::InitMono() {
    mono_set_assemblies_path("mono/lib/4.5");

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

} // namespace InternalCalls

void ScriptGlue::RegisterFunctions() {
    ADD_INTERNAL_CALL(NativeLog);
    ADD_INTERNAL_CALL(NativeLogVector2);
    ADD_INTERNAL_CALL(NativeLogVectorDot);
}

ScriptClass::ScriptClass(const std::string &classNamespace, const std::string &className)
    : mClassNamespace(classNamespace), mClassName(className) {
    mMonoClass = mono_class_from_name(sData->AppAssemblyImage, mClassNamespace.c_str(), mClassName.c_str());
}

MonoObject *ScriptClass::Instantiate() { return Scripting::InstantiateKlass(mMonoClass); }

MonoMethod *ScriptClass::GetMethod(const std::string &name, int parameterCount) {
    return mono_class_get_method_from_name(mMonoClass, name.c_str(), parameterCount);
}

MonoObject *ScriptClass::InvokeMethod(MonoObject *instance, MonoMethod *monoMethod, void **params) {
    return mono_runtime_invoke(monoMethod, instance, params, nullptr);
}

} // namespace rt
