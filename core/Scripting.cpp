#include "Scripting.h"
#include <iostream>

namespace rt {

struct ScriptingData {
    MonoDomain *RootDomain = nullptr;
    MonoDomain *AppDomain = nullptr;
    MonoAssembly *AppAssembly = nullptr;
};

static ScriptingData *sData = nullptr;

Scripting::Scripting() {}

Scripting::~Scripting() {}

void Scripting::Init() {
    sData = new ScriptingData;
    InitMono();
}

void Scripting::Shutdown() {
    ShutdownMono();
    delete sData;
}

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

void Scripting::InitMono() {
    mono_set_assemblies_path("mono/lib/4.5/");

    MonoDomain *rootDomain = mono_jit_init("MyScriptRuntime");
    if (rootDomain == nullptr) {
        return;
    }

    // Store the root domain pointer
    sData->RootDomain = rootDomain;

    // Create an App Domain
    sData->AppDomain = mono_domain_create_appdomain((char *)"MyAppDomain", nullptr);
    mono_domain_set(sData->AppDomain, true);

    mono_add_internal_call("RTEngine.Main::NativeLog", NativeLog);
    mono_add_internal_call("RTEngine.Main::NativeLogVector2", NativeLogVector2);
    mono_add_internal_call("RTEngine.Main::NativeLogVectorDot", NativeLogVectorDot);

    sData->AppAssembly = LoadCSharpAssembly("./rtmodule.dll");
    PrintAssemblyTypes(sData->AppAssembly);

    MonoImage *assemblyImage = mono_assembly_get_image(sData->AppAssembly);
    MonoClass *monoClass = mono_class_from_name(assemblyImage, "RTEngine", "Main");

    // 1. Create an object (and call constructor)
    MonoObject *instance = mono_object_new(sData->AppDomain, monoClass);
    mono_runtime_object_init(instance);

    // 2. Call function
    MonoMethod *printMessageFunc = mono_class_get_method_from_name(monoClass, "PrintMessage", 0);
    mono_runtime_invoke(printMessageFunc, instance, nullptr, nullptr);

    // 3. Call function with param
    MonoMethod *printIntFunc = mono_class_get_method_from_name(monoClass, "PrintInt", 1);

    int value = 3;
    void *param = &value;

    mono_runtime_invoke(printIntFunc, instance, &param, nullptr);

    MonoMethod *printIntsFunc = mono_class_get_method_from_name(monoClass, "PrintInts", 2);

    int value1 = 1;
    int value2 = 2;
    void *params[2] = {
        &value1,
        &value2,
    };

    mono_runtime_invoke(printIntsFunc, instance, params, nullptr);

    MonoString *monoString = mono_string_new(sData->AppDomain, "Hello World from C++!!!");
    MonoMethod *printCustomMessageFunc = mono_class_get_method_from_name(monoClass, "PrintCustomMessage", 1);

    void *stringParam = monoString;
    mono_runtime_invoke(printCustomMessageFunc, instance, &stringParam, nullptr);
}

void Scripting::ShutdownMono() {
    // Mono is a little confusing to shutdown.
    // mono_domain_unload(sData->AppDomain);
    // mono_jit_cleanup(sData->RootDomain);

    sData->AppDomain = nullptr;
    sData->RootDomain = nullptr;
}

char *ReadBytes(const std::string &filepath, uint32_t *outSize) {
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

MonoAssembly *LoadCSharpAssembly(const std::string &assemblyPath) {
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

    MonoAssembly *assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
    mono_image_close(image);

    // Don't forget to free the file data
    delete[] fileData;

    return assembly;
}

void PrintAssemblyTypes(MonoAssembly *assembly) {
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

MonoClass *Scripting::GetClassInAssembly(MonoAssembly *assembly, const char *namespaceName, const char *className) {
    MonoImage *image = mono_assembly_get_image(assembly);
    MonoClass *klass = mono_class_from_name(image, namespaceName, className);

    if (klass == nullptr) {
        // Log error here
        return nullptr;
    }

    return klass;
}

MonoObject *Scripting::InstantiateClass(const char *namespaceName, const char *className) {
    // Get a reference to the class we want to instantiate
    MonoClass *testingClass = GetClassInAssembly(sData->AppAssembly, "", "CSharpTesting");

    // Allocate an instance of our class
    MonoObject *classInstance = mono_object_new(sData->AppDomain, testingClass);

    if (classInstance == nullptr) {
        // Log error here and abort
    }

    // Call the parameterless (default) constructor
    mono_runtime_object_init(classInstance);
    return classInstance;
}

void Scripting::CallPrintFloatVarMethod(MonoObject *objectInstance) {
    // Get the MonoClass pointer from the instance
    MonoClass *instanceClass = mono_object_get_class(objectInstance);

    // Get a reference to the method in the class
    MonoMethod *method = mono_class_get_method_from_name(instanceClass, "PrintFloatVar", 0);

    if (method == nullptr) {
        // No method called "PrintFloatVar" with 0 parameters in the class, log error or something
        return;
    }

    // Call the C# method on the objectInstance instance, and get any potential exceptions
    MonoObject *exception = nullptr;
    mono_runtime_invoke(method, objectInstance, nullptr, &exception);

    // TODO: Handle the exception
}

void Scripting::CallIncrementFloatVarMethod(MonoObject *objectInstance, float value) {
    // Get the MonoClass pointer from the instance
    MonoClass *instanceClass = mono_object_get_class(objectInstance);

    // Get a reference to the method in the class
    MonoMethod *method = mono_class_get_method_from_name(instanceClass, "IncrementFloatVar", 1);

    if (method == nullptr) {
        // No method called "IncrementFloatVar" with 1 parameter in the class, log error or something
        return;
    }

    // Call the C# method on the objectInstance instance, and get any potential exceptions
    MonoObject *exception = nullptr;
    void *params[] = {&value};

    mono_runtime_invoke(method, objectInstance, params, &exception);

    // TODO: Handle the exception
}

} // namespace rt
