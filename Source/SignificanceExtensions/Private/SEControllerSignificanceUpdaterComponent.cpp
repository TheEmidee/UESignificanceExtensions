#include "SEControllerSignificanceUpdaterComponent.h"

#include <SignificanceManager.h>

USEControllerSignificanceUpdaterComponent::USEControllerSignificanceUpdaterComponent( const FObjectInitializer & object_initializer ) :
    Super( object_initializer )
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}

void USEControllerSignificanceUpdaterComponent::TickComponent( const float delta_time, const ELevelTick tick_type, FActorComponentTickFunction * this_tick_function )
{
    Super::TickComponent( delta_time, tick_type, this_tick_function );

    if ( auto * significance_manager = USignificanceManager::Get< USignificanceManager >( GetWorld() ) )
    {
        FVector view_location;
        FRotator view_rotation;

        GetPlayerController()->GetPlayerViewPoint( view_location, view_rotation );

        TArray< FTransform > view_points;
        view_points.Emplace( view_rotation, view_location, FVector::OneVector );

        significance_manager->Update( view_points );
    }
}

APlayerController * USEControllerSignificanceUpdaterComponent::GetPlayerController() const
{
    return Cast< APlayerController >( GetOwner() );
}
