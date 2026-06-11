// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"

#include "SActionComponent.h"


void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running Action: %s"), *GetNameSafe(this))
	
	USActionComponent* ActionComp = GetOwningComponent();
	
	ActionComp->ActiveGameplayTags.AppendTags(GrantsTags);
	
	bIsActionRunning = true;
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Stopped Action: %s"), *GetNameSafe(this))
	
	ensureAlways(bIsActionRunning);
	
	USActionComponent* ActionComp = GetOwningComponent();
	ActionComp->ActiveGameplayTags.RemoveTags(GrantsTags);
	
	bIsActionRunning = false;
}

UWorld* USAction::GetWorld() const
{
	UActorComponent* Comp = Cast<UActorComponent>(GetOuter());
	if (Comp)
	{
		return Comp->GetWorld();
	}
	return nullptr;
}

USActionComponent* USAction::GetOwningComponent() const
{
	return Cast<USActionComponent>(GetOuter());
}

bool USAction::GetIsActionRunning() const
{
	return bIsActionRunning;
}

bool USAction::CanStartAction_Implementation(AActor* Instigator) 
{
	if (GetIsActionRunning())
	{
		return false;
	}
	
	USActionComponent* ActionComp = GetOwningComponent();
	
	if (ActionComp->ActiveGameplayTags.HasAny(BlockedTags))
	{
		return false;
	}
	return true;
}
