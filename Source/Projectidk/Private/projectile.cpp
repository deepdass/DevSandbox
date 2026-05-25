// Fill out your copyright notice in the Description page of Project Settings.


#include "projectile.h"

#include "NiagaraFunctionLibrary.h"
#include "SAttributeComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

Aprojectile::Aprojectile()
{
	ProjectileMovement->InitialSpeed = 1000.0f;
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
			AttributeComp->ApplyHealthChange(-20.0f);
		}
	}

	Super::Explode_Implementation();
}
