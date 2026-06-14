// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/Projectile.h"

#include "NiagaraFunctionLibrary.h"
#include "ActionSystem/SActionComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Camera/CameraShakeBase.h"
#include "Components/SphereComponent.h"
#include "Core/SGameplayFunctionLibrary.h"

Aprojectile::Aprojectile()
{
	ProjectileMovement->InitialSpeed = 1000.0f;
	DamageAmount = -20.0f;
	
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.0f;
	ProjectileMovement->Friction = 0.0f;
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
		USActionComponent* ActionComp = Cast<USActionComponent>(HitActor->GetComponentByClass(USActionComponent :: StaticClass()));
		if (ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag))
		{
			SphereComp->IgnoreActorWhenMoving(GetInstigator(), false);
			SphereComp->IgnoreActorWhenMoving(HitActor, true);
			
			SetInstigator(Cast<APawn>(HitActor));
			
			ProjectileMovement->SetVelocityInLocalSpace(FVector(-1.f, 0.f, 0.f) * ProjectileMovement->InitialSpeed);

			return;
		}
		USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), HitActor, DamageAmount, SweepResult);
	}
	
	UGameplayStatics::PlayWorldCameraShake(GetWorld(), ImpactCameraShake, GetActorLocation(), 0.0f, 1000.0f, 1.0f);
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	Super::Explode_Implementation();
}
