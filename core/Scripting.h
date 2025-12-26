#pragma once

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>

#include <fstream>
#include <string>

#define GLM_FORCE_RADIANS
#define GLM_FORM_DEPTH_ZERO_TO_ONE
#define GML_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>

namespace rt {

class Scripting {

  public:
    Scripting();
    ~Scripting();

    static void InitMono();
    static void ShutdownMono();
    static void Init();
    static void Shutdown();

    MonoClass *GetClassInAssembly(MonoAssembly *assembly, const char *namespaceName, const char *className);
    MonoObject *InstantiateClass(const char *namespaceName, const char *className);
    void CallPrintFloatVarMethod(MonoObject *objectInstance);
    void CallIncrementFloatVarMethod(MonoObject *objectInstance, float value);
};


char *ReadBytes(const std::string &filepath, uint32_t *outSize);
MonoAssembly *LoadCSharpAssembly(const std::string &assemblyPath);
void PrintAssemblyTypes(MonoAssembly *assembly);

} // namespace rt
