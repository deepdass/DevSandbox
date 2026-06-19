// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"
#include "Net\UnrealNetwork.h"
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
	
	
	USActionComponent* ActionComp = GetOwningComponent();
	ActionComp->ActiveGameplayTags.RemoveTags(GrantsTags);
	
	bIsActionRunning = false;
}

UWorld* USAction::GetWorld() const
{
	AActor* Actor = Cast<AActor>(GetOuter());
	if (Actor)
	{
		return Actor->GetWorld();
	}
	return nullptr;
}

USActionComponent* USAction::GetOwningComponent() const
{
	return ActionComponent;
}

void USAction::OnRep_IsRunning()
{
	AActor* Instigator = GetOwningComponent() ? GetOwningComponent()->GetOwner() : nullptr;

	if (bIsActionRunning)
	{
		StartAction(Instigator);
	}
	else
	{
		StopAction(Instigator);
	}
}

void USAction::Initialize(USActionComponent* NewActionComp)
{
	ActionComponent = NewActionComp;
}

bool USAction::GetIsActionRunning() const
{
	return bIsActionRunning;
}

void USAction::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(USAction ,bIsActionRunning);
	DOREPLIFETIME(USAction ,ActionComponent);
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
