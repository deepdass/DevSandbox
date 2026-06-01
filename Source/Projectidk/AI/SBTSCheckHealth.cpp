// Fill out your copyright notice in the Description page of Project Settings.


#include "SBTSCheckHealth.h"
#include "PlayerComps/SAttributeComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void USBTSCheckHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
    
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AAIController* MyAIController = OwnerComp.GetAIOwner();
	
	if (ensure(IsValid(BlackboardComp->GetValueAsObject("TargetActor")))) { return;}
	
	if (!ensure(IsValid(MyAIController))){return;}
			

	APawn* AIPawn = MyAIController->GetPawn();

	if (ensure(IsValid(AIPawn)))
	{
		USAttributeComponent* HealthComp = AIPawn->FindComponentByClass<USAttributeComponent>();
		if (!ensure(IsValid(HealthComp))) {return;}
		
		float CurrentHealth = HealthComp->GetHealth();
		float MaxHealth = HealthComp->GetMaxHealth();
		
		float HealthPercentage = CurrentHealth / MaxHealth;
		BlackboardComp->SetValueAsBool(IsLowHealth.SelectedKeyName, (HealthPercentage <= 0.3f || FMath::IsNearlyEqual(HealthPercentage, 0.3f)));

	}
}
