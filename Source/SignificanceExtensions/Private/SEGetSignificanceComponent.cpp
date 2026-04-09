#include "SEGetSignificanceComponent.h"

#include "Misc/DataValidation.h"
#include "SEGetSignificanceInterface.h"

#include <SignificanceManager.h>

USEGetSignificanceComponent::USEGetSignificanceComponent( const FObjectInitializer & object_initializer ) :
    Super( object_initializer ),
    bUseConcurrentPostUpdate( true ),
    ComputationType( ESESignificanceComputationType::MaxDistance ),
    FixedSignificance( 1.0f ),
    MaxDistance( 3000.0f ),
    bComponentImplementsPostSignificanceUpdate( false ),
    bOwnerImplementsInterface( false ),
    MaxDistanceSquared( 1.0f ),
    LastComputedSignificance( 0.0f )
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
    switch ( ComputationType )
    {
        case ESESignificanceComputationType::Fixed:
        {
        }
        break;
        case ESESignificanceComputationType::MaxDistance:
        {
        }
        break;
        case ESESignificanceComputationType::DistanceThreshold:
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
        break;
        default:
        {
            checkNoEntry();
        };
    }

    return Super::IsDataValid( context );
}
#endif

void USEGetSignificanceComponent::BeginPlay()
{
    Super::BeginPlay();

    MaxDistanceSquared = FMath::Square( MaxDistance );

    bOwnerImplementsInterface = GetOwner()->GetClass()->ImplementsInterface( USEGetSignificanceInterface::StaticClass() );

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
                LastComputedSignificance = GetSignificance( managed_object_info, view_transform );
                return LastComputedSignificance;
            },
            bUseConcurrentPostUpdate ? USignificanceManager::EPostSignificanceType::Concurrent : USignificanceManager::EPostSignificanceType::Sequential,
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

void USEGetSignificanceComponent::PostSignificanceUpdate( FName tag, float old_significance, float new_significance, bool is_final )
{
}

float USEGetSignificanceComponent::GetSignificance( const USignificanceManager::FManagedObjectInfo * managed_object_info, const FTransform & view_transform )
{
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

    switch ( ComputationType )
    {
        case ESESignificanceComputationType::Fixed:
        {
            return FixedSignificance;
        }
        case ESESignificanceComputationType::MaxDistance:
        {
            return GetSignificanceByMaxDistance( view_transform );
        }
        case ESESignificanceComputationType::DistanceThreshold:
        {
            return GetSignificanceByDistanceThreshold( view_transform );
        }
        default:
        {
            checkNoEntry();
            return 0.0f;
        };
    }
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
        return;
    }

    PostSignificanceUpdate( managed_object_info->GetTag(), old_significance, new_significance, is_final );
}

float USEGetSignificanceComponent::GetSignificanceByDistanceThreshold( const FTransform & view_transform ) const
{
    const auto actor_location = GetOwner()->GetActorLocation();
    const auto distance = FVector::Dist( actor_location, view_transform.GetLocation() );

    for ( auto index = SignificanceDistances.Num() - 1; index >= 0; index-- )
    {
        const auto significance_distance = SignificanceDistances[ index ];
        if ( distance >= significance_distance.DistanceThreshold )
        {
            return significance_distance.Significance;
        }
    }

    return 1.0f;
}

float USEGetSignificanceComponent::GetSignificanceByMaxDistance( const FTransform & view_transform ) const
{
    const auto actor_location = GetOwner()->GetActorLocation();
    const auto distance_sqr = FVector::DistSquared( actor_location, view_transform.GetLocation() );

    auto significance = 1.0f - distance_sqr / MaxDistanceSquared;
    significance = FMath::Clamp( 0.0f, significance, 1.0f );

    return significance;
}
