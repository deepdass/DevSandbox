// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBaseClass_PickUps.h"
#include "SHealthPotion.generated.h"

UCLASS()
class PROJECTIDK_API ASHealthPotion : public ASBaseClass_PickUps
{
	GENERATED_BODY()


public:
	// Sets default values for this actor's properties
	ASHealthPotion();
	
protected:
	
	void Interact_Implementation(APawn* InstigatorPawn) override;
	
	UPROPERTY(EditDefaultsOnly, Category="Potion")
	float HealAmount;

};
