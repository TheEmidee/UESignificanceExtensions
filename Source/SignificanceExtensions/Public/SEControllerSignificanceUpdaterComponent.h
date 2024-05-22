#pragma once

#include <CoreMinimal.h>

#include "SEControllerSignificanceUpdaterComponent.generated.h"

UCLASS()
class SIGNIFICANCEEXTENSIONS_API USEControllerSignificanceUpdaterComponent final : public UActorComponent
{
    GENERATED_BODY()

public:
    explicit USEControllerSignificanceUpdaterComponent( const FObjectInitializer & object_initializer );

    void BeginPlay() override;
    void TickComponent( float delta_time, ELevelTick tick_type, FActorComponentTickFunction * this_tick_function ) override;

private:
    APlayerController * GetPlayerController() const;
};
