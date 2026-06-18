// Fill out your copyright notice in the Description page of Project Settings.


#include "SBaseClassProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"

// Sets default values
ASBaseClassProjectile::ASBaseClassProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    	
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionProfileName("Projectile");
	SphereComp->OnComponentHit.AddDynamic(this, &ASBaseClassProjectile::OnActorHit);
	RootComponent = SphereComp;
    	
	EffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ParticleComp"));
	EffectComp->SetupAttachment(SphereComp);
    	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 1000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bInitialVelocityInLocalSpace = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	
	SetReplicates(true);
}


void ASBaseClassProjectile::BeginPlay()
{
	Super::BeginPlay();
	SphereComp->IgnoreActorWhenMoving(GetInstigator() ,true);
}

void ASBaseClassProjectile::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority()) return;
	if (OtherActor && OtherActor != GetInstigator())
	{
		HitActor = OtherActor;
		SweepResult = Hit;
		Explode();
	} 
}

void ASBaseClassProjectile::Explode_Implementation()
{
	if (ensure(IsValid(this)))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());
		
		Destroy();
	}
}



