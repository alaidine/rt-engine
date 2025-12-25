#pragma once

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include <fstream>
#include <string>

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

} // namespace rt
