// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionMinion_HealSelf.h"

#include "SAICharacter.h"
#include "ActionSystem/SActionComponent.h"
#include "PlayerComps/SAttributeComponent.h"

void USActionMinion_HealSelf::StartAction_Implementation(AActor* Instigator)
{
	ASAICharacter* MyPawn = CastChecked<ASAICharacter>(GetOwningComponent()->GetOwner());
	
	Super::StartAction_Implementation(Instigator);
	
	if (!IsValid(MyPawn)){ return; }

	USAttributeComponent* MyAttributeComp = MyPawn->FindComponentByClass<USAttributeComponent>();
	if (!ensure(IsValid(MyAttributeComp))) {return; }

	MyAttributeComp->ApplyHealthChange(MyPawn, HealAmount);
	
	StopAction(Instigator);
}

bool USActionMinion_HealSelf::CanStartAction_Implementation(AActor* Instigator)
{
	if (Super::CanStartAction_Implementation(Instigator))
	{
		ASAICharacter* MyPawn = CastChecked<ASAICharacter>(GetOwningComponent()->GetOwner());
		AActor* TargetActor = MyPawn->GetTarget();
		
		return TargetActor && USAttributeComponent::GetIsActorAlive(TargetActor);
	}

	return false;
}
