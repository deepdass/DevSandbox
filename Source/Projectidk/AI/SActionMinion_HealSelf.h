// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionSystem/SAction.h"
#include "SActionMinion_HealSelf.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTIDK_API USActionMinion_HealSelf : public USAction
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditAnywhere)
	float HealAmount = 60.0f;
	
	virtual void StartAction_Implementation(AActor* Instigator) override;

	virtual bool CanStartAction_Implementation(AActor* Instigator) override;
};
