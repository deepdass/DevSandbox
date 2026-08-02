// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionSystem/SAction.h"
#include "SActionMinion_ProjectileAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTIDK_API USActionMinion_ProjectileAttack : public USAction
{
	GENERATED_BODY()
	
	USActionMinion_ProjectileAttack();
	
protected:
	
	UPROPERTY(EditAnywhere, Category="AI")
	FVector2D PitchMaxBulletSpread;
	
	UPROPERTY(EditAnywhere, Category="AI")
	FVector2D YawMaxBulletSpread;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<AActor> ProjectileClass;
	
	virtual void StartAction_Implementation(AActor* Instigator) override;

	virtual bool CanStartAction_Implementation(AActor* Instigator) override;
};
