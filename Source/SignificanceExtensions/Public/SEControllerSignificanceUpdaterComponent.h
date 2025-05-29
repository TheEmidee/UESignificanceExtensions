#pragma once

#include <CoreMinimal.h>

#include "SEControllerSignificanceUpdaterComponent.generated.h"

UCLASS()
class SIGNIFICANCEEXTENSIONS_API USEControllerSignificanceUpdaterComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    explicit USEControllerSignificanceUpdaterComponent( const FObjectInitializer & object_initializer );

    void TickComponent( float delta_time, ELevelTick tick_type, FActorComponentTickFunction * this_tick_function ) override;

protected:
    APlayerController * GetPlayerController() const;
};
