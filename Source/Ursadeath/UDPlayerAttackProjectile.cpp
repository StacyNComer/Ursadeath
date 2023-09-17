
#include "UDPlayerAttackProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UDPlayerCharacter.h"
#include "UDEnemy.h"

AUDPlayerAttackProjectile::AUDPlayerAttackProjectile()
{
	//Initialize the sphere component and its overlap event.
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	SphereComponent->SetCollisionProfileName(PlayerAttackCollisionProfile);
	/*SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AUDPlayerAttackProjectile::OnSphereBeginOverlap);*/

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
}

void AUDPlayerAttackProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	//Apply any attacks that this enemy overlaps
	AUDEnemy* EnemyHit = Cast<AUDEnemy>(OtherActor);
	if (EnemyHit != nullptr)
	{
		ApplyAttackToEnemy(EnemyHit, AttackStats);
	}

	Destroy();
}