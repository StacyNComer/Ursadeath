
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

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
}

void AUDPlayerAttackProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	//Apply the attack to an ememy that it overlaps.
	AUDEnemy* EnemyHit = Cast<AUDEnemy>(OtherActor);
	if (EnemyHit != nullptr)
	{
		ApplyAttackExclusive(EnemyHit, AttackStats);
	}

	NotifyOnProjectileHit(OtherActor);

	//Destroy the project whenever it hits something. The projectile's collision settings should prevent it from hitting anything other than an enemy or wall.
	Destroy();
}

void AUDPlayerAttackProjectile::NotifyOnProjectileHit(AActor* ActorHit)
{
	GetOwningPlayer()->NotifyOnPlayerProjectileHit(this, ActorHit);
}