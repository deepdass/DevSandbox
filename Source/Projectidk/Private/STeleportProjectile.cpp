// Fill out your copyright notice in the Description page of Project Settings.


#include "STeleportProjectile.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
ASTeleportProjectile::ASTeleportProjectile()
{
	TeleportDelay = 0.2f;
	DetonateDelay = 0.2f;
	
	ProjectileMovement->InitialSpeed = 6000.0f;
}

void ASTeleportProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(TimerHandle_DetonateDelay, this, &ASTeleportProjectile::Explode, DetonateDelay);
}

void ASTeleportProjectile::Explode_Implementation()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_DetonateDelay);
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());
	
	EffectComp->DeactivateImmediate();
	
	ProjectileMovement->StopMovementImmediately();
	SetActorEnableCollision(false);
	
	FTimerHandle TimerHandle_TeleportDelay;
	GetWorldTimerManager().SetTimer(TimerHandle_TeleportDelay, this, &ASTeleportProjectile::TeleportInstigator, TeleportDelay);
	
}

void ASTeleportProjectile::TeleportInstigator()
{
	AActor* ActorToTeleport = GetInstigator();
	
	if (ensure(IsValid(ActorToTeleport)))
	{
		ActorToTeleport->TeleportTo(GetActorLocation(),ActorToTeleport->GetActorRotation());
	}
}



