// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBaseClass_PickUps.h"
#include "SHealthPotion.generated.h"

UCLASS()
class PROJECTIDK_API ASHealthPotion : public ASBaseClass_PickUps
{
	GENERATED_BODY()
	
	void Interact_Implementation(APawn* InstigatorPawn) override;

public:
	// Sets default values for this actor's properties
	ASHealthPotion();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Potion")
	float HealAmount;

};
