#pragma once

#include <CoreMinimal.h>
#include <UObject/Interface.h>

#include "SEGetSignificanceInterface.generated.h"

UINTERFACE( MinimalAPI )
class USEGetSignificanceInterface : public UInterface
{
    GENERATED_BODY()
};

class SIGNIFICANCEEXTENSIONS_API ISEGetSignificanceInterface
{
    GENERATED_BODY()

public:
    UFUNCTION( BlueprintNativeEvent, Category = "Significance" )
    bool GetSignificance( float & significance, FName tag, const FTransform & view_transform );

    UFUNCTION( BlueprintNativeEvent, Category = "Significance" )
    bool PostSignificanceUpdate( FName tag, float old_significance, float new_significance, bool is_final );
};