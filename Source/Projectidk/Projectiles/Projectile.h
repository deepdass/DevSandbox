// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Projectiles/SBaseClassProjectile.h"
#include "projectile.generated.h"

class USoundCue;
class UCameraShakeBase;

UCLASS()
class PROJECTIDK_API Aprojectile : public ASBaseClassProjectile
{
	GENERATED_BODY()

public:
	Aprojectile();

protected:
	
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FGameplayTag ParryTag;
	
	UPROPERTY(EditAnywhere, Category="Damage")
	float DamageAmount;
	
	UPROPERTY(EditAnywhere, Category="Projectile")
	TObjectPtr<USoundCue> ImpactSound;
	
	UPROPERTY(EditAnywhere, Category="Projectile")
	TSubclassOf<UCameraShakeBase> ImpactCameraShake;
	
	virtual void BeginPlay() override;
	
	virtual void Explode_Implementation() override;

private:
	
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UNiagaraSystem> SpawnVFX;
	
};