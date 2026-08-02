// Fill out your copyright notice in the Description page of Project Settings.


#include "SBTTask_StartAction.h"

#include "AI/SBTTask_StartAction.h"
#include "AIController.h"
#include "ActionSystem/SActionComponent.h"
#include "Core/SGameplayFunctionLibrary.h"


EBTNodeResult::Type USBTTask_StartAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* MyPawn = OwnerComp.GetAIOwner()->GetPawn();
	check(MyPawn);
	
	USActionComponent* ActionComp = MyPawn->FindComponentByClass<USActionComponent>();
	check(ActionComp);

	if (ActionComp->StartActionByName(MyPawn, ActionName))
	{
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;	
}
