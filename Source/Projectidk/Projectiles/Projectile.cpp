// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/Projectile.h"

#include "NiagaraFunctionLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PlayerComps/SAttributeComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Camera/CameraShakeBase.h"
#include "Core/SGameplayFunctionLibrary.h"

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
	if (IsValid(HitActor))
	{
		USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), HitActor, DamageAmount, SweepResult);
		
		// USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(HitActor);
		// if (AttributeComp)
		// {
		// 	AttributeComp->ApplyHealthChange(GetInstigator() ,DamageAmount); 
		// }
	}
	
	UGameplayStatics::PlayWorldCameraShake(GetWorld(), ImpactCameraShake, GetActorLocation(), 0.0f, 1000.0f, 1.0f);
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	Super::Explode_Implementation();
}
