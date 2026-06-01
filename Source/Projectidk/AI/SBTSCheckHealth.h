// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SBTSCheckHealth.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTIDK_API USBTSCheckHealth : public UBTService
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector IsLowHealth;
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
