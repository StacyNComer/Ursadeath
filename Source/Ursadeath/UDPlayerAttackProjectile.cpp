
#include "UDPlayerAttackProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AUDPlayerAttackProjectile::AUDPlayerAttackProjectile()
{
	//Initialize the sphere component and its overlap event.
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	SphereComponent->SetCollisionProfileName(PlayerAttackCollisionProfile);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AUDPlayerAttackProjectile::OnSphereBeginOverlap);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
}

void AUDPlayerAttackProjectile::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Destroy the projectile if it overlaps with something. Projectiles should be prevented from overlapping players and their attack via collision presets.
	Destroy();
}