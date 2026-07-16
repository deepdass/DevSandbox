// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAction.h"
#include "SActionSkill.generated.h"

class ASBaseClassProjectile;

/**
 *
 */
UCLASS()
class PROJECTIDK_API USActionSkill : public USAction
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<ASBaseClassProjectile> ProjectileClass;

	UPROPERTY(VisibleAnywhere, Category = "Attack")
	FName HandSocketName = "Muzzle_01";

	FTimerHandle TimerHandle_AttackDelay;

	UPROPERTY(EditDefaultsOnly)
	float AttackAnimDelay;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TObjectPtr<UAnimMontage> AttackAnim;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float TimeBetweenAttacks = 0.6f;

	float NextAttackAllowedTime = 0.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float ActionCost;

	UFUNCTION()
	void AttackDelay_Elapsed(ACharacter* InstigatorCharacter);

public:

	virtual void StartAction_Implementation(AActor* Instigator) override;

	USActionSkill();

	void SetPrimaryProjectile(TSubclassOf<ASBaseClassProjectile>);

};