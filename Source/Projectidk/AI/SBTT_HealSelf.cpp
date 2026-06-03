// Fill out your copyright notice in the Description page of Project Settings.


#include "SBTT_HealSelf.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PlayerComps/SAttributeComponent.h"


EBTNodeResult::Type USBTT_HealSelf::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	APawn* MyPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (!IsValid(MyPawn)) return EBTNodeResult::Failed;

	USAttributeComponent* MyAttributeComp = MyPawn->FindComponentByClass<USAttributeComponent>();
	if (!ensure(IsValid(MyAttributeComp))) return EBTNodeResult::Failed;

	MyAttributeComp->ApplyHealthChange(60.0f);

	return EBTNodeResult::Succeeded;
}
