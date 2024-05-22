#include "SEGetSignificanceComponent.h"

#include "Misc/DataValidation.h"
#include "SEGetSignificanceInterface.h"

#include <SignificanceManager.h>

USEGetSignificanceComponent::USEGetSignificanceComponent( const FObjectInitializer & object_initializer ) :
    Super( object_initializer ),
    bUseFixedSignificance( false ),
    FixedSignificance( 1.0f ),
    bOwnerImplementsInterface( false )
{
    PrimaryComponentTick.bCanEverTick = false;

    auto implemented_in_blueprint = [ & ]( const FName function_name ) -> bool {
        const UFunction * function = GetClass()->FindFunctionByName( function_name );

        return function != nullptr && ensure( function->GetOuter() ) && function->GetOuter()->IsA( UBlueprintGeneratedClass::StaticClass() );
    };

    {
        static auto func_name = FName( TEXT( "K2_GetSignificance" ) );
        bComponentImplementsGetSignificance = implemented_in_blueprint( func_name );
    }
    {
        static auto func_name = FName( TEXT( "K2_PostSignificanceUpdate" ) );
        bComponentImplementsPostSignificanceUpdate = implemented_in_blueprint( func_name );
    }
}

void USEGetSignificanceComponent::EndPlay( const EEndPlayReason::Type end_play_reason )
{
    if ( auto * significance_manager = USignificanceManager::Get< USignificanceManager >( GetWorld() ) )
    {
        significance_manager->UnregisterObject( this );
    }

    Super::EndPlay( end_play_reason );
}

#if WITH_EDITOR
EDataValidationResult USEGetSignificanceComponent::IsDataValid( FDataValidationContext & context ) const
{
    if ( !bUseFixedSignificance )
    {
        if ( SignificanceDistances.IsEmpty() )
        {
            context.AddError( FText::FromString( TEXT( "SignificanceDistances is empty and you don't use a fixed significance" ) ) );
        }
        else
        {
            auto previous_distance = SignificanceDistances[ 0 ].DistanceThreshold;
            for ( auto index = 1; index < SignificanceDistances.Num(); ++index )
            {
                const auto current_distance = SignificanceDistances[ index ].DistanceThreshold;
                if ( current_distance <= previous_distance )
                {
                    context.AddError( FText::FromString( TEXT( "Distances in SignificanceDistances must be sorted from closest to farthest" ) ) );
                    break;
                }

                previous_distance = current_distance;
            }
        }
    }

    return Super::IsDataValid( context );
}
#endif

void USEGetSignificanceComponent::BeginPlay()
{
    Super::BeginPlay();

    bOwnerImplementsInterface = Cast< ISEGetSignificanceInterface >( GetOwner() ) != nullptr;

    if ( auto * significance_manager = USignificanceManager::Get< USignificanceManager >( GetWorld() ) )
    {
        auto tag = SignificanceTag;
        if ( tag == NAME_None )
        {
            tag = GetOwner()->GetClass()->GetFName();
        }

        significance_manager->RegisterObject(
            this,
            tag,
            [ & ]( const USignificanceManager::FManagedObjectInfo * managed_object_info, const FTransform & view_transform ) {
                return GetSignificance( managed_object_info, view_transform );
            },
            USignificanceManager::EPostSignificanceType::Sequential,
            [ & ]( const USignificanceManager::FManagedObjectInfo * managed_object_info, float old_significance, float new_significance, bool is_final ) {
                PostSignificanceUpdate( managed_object_info, old_significance, new_significance, is_final );
            } );
    }
}

float USEGetSignificanceComponent::K2_GetSignificance_Implementation( FName /*tag*/, const FTransform & /*transform*/ view_transform )
{
    return 1.0f;
}

void USEGetSignificanceComponent::K2_PostSignificanceUpdate_Implementation( FName /*tag*/, float /*old_significance*/, float /*new_significance*/, bool /*is_final*/ )
{
}

float USEGetSignificanceComponent::GetSignificance( const USignificanceManager::FManagedObjectInfo * managed_object_info, const FTransform & view_transform )
{
    if ( bUseFixedSignificance )
    {
        return FixedSignificance;
    }

    if ( bOwnerImplementsInterface )
    {
        float significance;
        if ( ISEGetSignificanceInterface::Execute_GetSignificance( GetOwner(), significance, managed_object_info->GetTag(), view_transform ) )
        {
            return significance;
        }
    }

    if ( bComponentImplementsGetSignificance )
    {
        return K2_GetSignificance( managed_object_info->GetTag(), view_transform );
    }

    return GetSignificanceByDistance( view_transform );
}

void USEGetSignificanceComponent::PostSignificanceUpdate( const USignificanceManager::FManagedObjectInfo * managed_object_info, float old_significance, float new_significance, bool is_final )
{
    if ( bOwnerImplementsInterface )
    {
        if ( ISEGetSignificanceInterface::Execute_PostSignificanceUpdate( GetOwner(), managed_object_info->GetTag(), old_significance, new_significance, is_final ) )
        {
            return;
        }
    }

    if ( bComponentImplementsPostSignificanceUpdate )
    {
        K2_PostSignificanceUpdate( managed_object_info->GetTag(), old_significance, new_significance, is_final );
    }
}

float USEGetSignificanceComponent::GetSignificanceByDistance( const FTransform & view_transform ) const
{
    const auto actor_location = GetOwner()->GetActorLocation();
    const auto distance_squared = FVector::DistSquared( actor_location, view_transform.GetLocation() );

    for ( auto index = SignificanceDistances.Num() - 1; index >= 0; index-- )
    {
        const auto significance_distance = SignificanceDistances[ index ];
        if ( distance_squared >= FMath::Square( significance_distance.DistanceThreshold ) )
        {
            return significance_distance.Significance;
        }
    }

    return 1.0f;
}
