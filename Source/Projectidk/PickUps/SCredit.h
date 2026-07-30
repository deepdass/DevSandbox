// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBaseClass_PickUps.h"
#include "SCredit.generated.h"

UCLASS()
class PROJECTIDK_API ASCredit : public ASBaseClass_PickUps
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASCredit();
	
protected:
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
	
	UPROPERTY(EditDefaultsOnly, Category="Credit")
	int CreditAmount;
	
	UFUNCTION(BlueprintCallable)
	void AddCredit(APawn* InstigatorPawn);
};
