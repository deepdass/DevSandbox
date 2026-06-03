// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SBTT_RangedAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTIDK_API USBTT_RangedAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override; 
	
public:
	USBTT_RangedAttack();
	
protected:
	
	UPROPERTY(EditAnywhere, Category="AI")
	FVector2D PitchMaxBulletSpread;
	
	UPROPERTY(EditAnywhere, Category="AI")
	FVector2D YawMaxBulletSpread;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<AActor> ProjectileClass;
	
};
