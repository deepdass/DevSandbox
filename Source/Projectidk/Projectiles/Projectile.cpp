// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/Projectile.h"

#include "NiagaraFunctionLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PlayerComps/SAttributeComponent.h"

Aprojectile::Aprojectile()
{
	ProjectileMovement->InitialSpeed = 1000.0f;
	DamageAmount = -20.0f;
}

void Aprojectile::BeginPlay()
{
	Super::BeginPlay();
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, SpawnVFX, GetActorLocation(), GetActorRotation());

}

void ASBaseClassProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void Aprojectile::Explode_Implementation()
{
	if (HitActor)
	{
		USAttributeComponent* AttributeComp = HitActor->FindComponentByClass<USAttributeComponent>();
		if (AttributeComp)
		{
			AttributeComp->ApplyHealthChange(DamageAmount);
		}
	}

	Super::Explode_Implementation();
}
