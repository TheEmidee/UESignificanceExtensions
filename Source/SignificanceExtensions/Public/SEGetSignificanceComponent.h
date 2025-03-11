#pragma once

#include "SignificanceManager.h"

#include <Components/ActorComponent.h>
#include <CoreMinimal.h>

#include "SEGetSignificanceComponent.generated.h"

struct FActorComponentTickFunction;

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

protected:
    void BeginPlay() override;

    UFUNCTION( BlueprintNativeEvent, Category = "Significance" )
    float K2_GetSignificance( FName tag, const FTransform & view_transform );

    UFUNCTION( BlueprintNativeEvent, Category = "Significance" )
    void K2_PostSignificanceUpdate( FName tag, float old_significance, float new_significance, bool is_final );

    virtual void PostSignificanceUpdate( FName tag, float old_significance, float new_significance, bool is_final );

private:
    float GetSignificance( const USignificanceManager::FManagedObjectInfo * managed_object_info, const FTransform & view_transform );
    void PostSignificanceUpdate( const USignificanceManager::FManagedObjectInfo * managed_object_info, float old_significance, float new_significance, bool is_final );
    float GetSignificanceByDistance( const FTransform & view_transform ) const;

    UPROPERTY( EditDefaultsOnly, Category = "Significance" )
    uint8 bUseConcurrentPostUpdate : 1;

    UPROPERTY( EditDefaultsOnly, Category = "Significance" )
    FName SignificanceTag;

    UPROPERTY( EditDefaultsOnly, Category = "Significance" )
    uint8 bUseFixedSignificance : 1;

    UPROPERTY( EditDefaultsOnly, Category = "Significance", meta = ( EditCondition = "bUseFixedSignificance" ) )
    float FixedSignificance;

    // Gives a significance based on an array of distances
    // If empty, the significance will be the distance from the player
    UPROPERTY( EditDefaultsOnly, Category = "Significance", meta = ( EditCondition = "!bUseFixedSignificance" ) )
    TArray< FSESignificanceDistance > SignificanceDistances;

    bool bComponentImplementsGetSignificance;
    bool bComponentImplementsPostSignificanceUpdate;
    bool bOwnerImplementsInterface;
};
