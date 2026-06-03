// Fill out your copyright notice in the Description page of Project Settings.


#include "SBTS_CheckHealth.h"
#include "PlayerComps/SAttributeComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void USBTS_CheckHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (ensure(IsValid(AIPawn)))
	{
		USAttributeComponent* HealthComp = AIPawn->FindComponentByClass<USAttributeComponent>();
		if (!ensure(IsValid(HealthComp))) {return;}
		
		float CurrentHealth = HealthComp->GetHealth();
		float MaxHealth = HealthComp->GetMaxHealth();
		
		float HealthFraction = CurrentHealth / MaxHealth;
		
		UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
		BlackboardComp->SetValueAsBool(IsLowHealth.SelectedKeyName, (HealthFraction <= 0.3f || FMath::IsNearlyEqual(HealthFraction, 0.3f)));

	}
}
