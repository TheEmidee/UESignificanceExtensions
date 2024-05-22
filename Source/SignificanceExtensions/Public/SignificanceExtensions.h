#pragma once

#include <CoreMinimal.h>
#include <Modules/ModuleManager.h>

class FSignificanceExtensionsModule final : public IModuleInterface
{
public:
    void StartupModule() override;
    void ShutdownModule() override;
};
