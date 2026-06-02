// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SBTS_CheckHealth.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTIDK_API USBTS_CheckHealth : public UBTService
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector IsLowHealth;
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
