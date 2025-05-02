#pragma once

#include "SignificanceManager.h"

#include <Components/ActorComponent.h>
#include <CoreMinimal.h>

#include "SEGetSignificanceComponent.generated.h"

struct FActorComponentTickFunction;

UENUM()
enum class ESESignificanceComputationType : uint8
{
    Fixed,
    MaxDistance,
    DistanceThreshold
};

USTRUCT( BlueprintType )
struct FSESignificanceDistance
{
    GENERATED_BODY()

    FSESignificanceDistance() :
        Significance( 0.0f ),
        DistanceThreshold( 0.0f )
    {
    }

    UPROPERTY( EditAnywhere )
    float Significance;

    UPROPERTY( EditAnywhere )
    float DistanceThreshold;
};

UCLASS( Blueprintable, ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ) )
class SIGNIFICANCEEXTENSIONS_API USEGetSignificanceComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    explicit USEGetSignificanceComponent( const FObjectInitializer & object_initializer );

    void EndPlay( const EEndPlayReason::Type end_play_reason ) override;

#if WITH_EDITOR
    EDataValidationResult IsDataValid( FDataValidationContext & context ) const override;
#endif

    void SetSignificanceTag( const FName tag );

protected:
    void BeginPlay() override;

    UFUNCTION( BlueprintNativeEvent, Category = "Significance" )
    float K2_GetSignificance( FName tag, const FTransform & view_transform );

    UFUNCTION( BlueprintNativeEvent, Category = "Significance" )
    void K2_PostSignificanceUpdate( FName tag, float old_significance, float new_significance, bool is_final );

    virtual void PostSignificanceUpdate( FName tag, float old_significance, float new_significance, bool is_final );

    float GetSignificance( const USignificanceManager::FManagedObjectInfo * managed_object_info, const FTransform & view_transform );
    void PostSignificanceUpdate( const USignificanceManager::FManagedObjectInfo * managed_object_info, float old_significance, float new_significance, bool is_final );
    float GetSignificanceByDistanceThreshold( const FTransform & view_transform ) const;
    float GetSignificanceByMaxDistance( const FTransform & view_transform ) const;

    UPROPERTY( EditDefaultsOnly, Category = "Significance" )
    uint8 bUseConcurrentPostUpdate : 1;

    UPROPERTY( EditDefaultsOnly, Category = "Significance" )
    FName SignificanceTag;

    // Defines how to auto-calculate the significance. Note that if the actor or a child class implements ISEGetSignificanceInterface, then there is no auto-calculation
    UPROPERTY( EditDefaultsOnly, Category = "Significance" )
    ESESignificanceComputationType ComputationType;

    UPROPERTY( EditDefaultsOnly, Category = "Significance", meta = ( EditCondition = "ComputationType == ESESignificanceComputationType::Fixed", ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0" ) )
    float FixedSignificance;

    UPROPERTY( EditDefaultsOnly, Category = "Significance", meta = ( EditCondition = "ComputationType == ESESignificanceComputationType::DistanceThreshold" ) )
    TArray< FSESignificanceDistance > SignificanceDistances;

    UPROPERTY( EditDefaultsOnly, Category = "Significance", meta = ( EditCondition = "ComputationType == ESESignificanceComputationType::MaxDistance", ClampMin = "0.0", UIMin = "0.0", ForceUnits = "cm" ) )
    float MaxDistance;

    bool bComponentImplementsGetSignificance;
    bool bComponentImplementsPostSignificanceUpdate;
    bool bOwnerImplementsInterface;
    float MaxDistanceSquared;
};

FORCEINLINE void USEGetSignificanceComponent::SetSignificanceTag( const FName tag )
{
    SignificanceTag = tag;
}