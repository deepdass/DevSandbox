// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/SBTS_CheckAttackRange.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Actor.h"


void USBTS_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
    
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    
	if (ensure(IsValid(BlackboardComp)))
	{
		AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
       
		if (IsValid(TargetActor))
		{
			AAIController* MyAIController = OwnerComp.GetAIOwner();
			
			if (ensure(IsValid(MyAIController)))
			{
				APawn* AIPawn = MyAIController->GetPawn();

				if (ensure(IsValid(AIPawn)))
				{
					float DistanceTo = FVector::Distance(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());
					
					float AttackRangeAsFloat = BlackboardComp->GetValueAsFloat(AttackRange.SelectedKeyName);
					bool bWithinRange = DistanceTo < AttackRangeAsFloat;
					
					bool bHasLOS = false; 
					if (bWithinRange)
					{
						bHasLOS = MyAIController->LineOfSightTo(TargetActor);
					}
					
					BlackboardComp->SetValueAsBool(AttackRangeKey.SelectedKeyName, (bWithinRange && bHasLOS));
				}
			}
		}
	}
}