// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionComponent.h"

#include "SAction.h"


// Sets default values for this component's properties
USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void USActionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	for (TSubclassOf<USAction> ActionClass : DefaultActions)
	{
		AddAction(ActionClass);
	}
}

void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	FString DebugMsg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, DebugMsg);
}

void USActionComponent::AddAction(TSubclassOf<USAction> ActionClass)
{
	if (!ensure(ActionClass))
	{
		return;
	}
	
	TObjectPtr<USAction> NewAction = NewObject<USAction>(this, ActionClass);
	if (ensure(NewAction))
	{
		Actions.Add(NewAction);
	}
}

bool USActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for (USAction* Action : Actions)
	{
		if (IsValid(Action) && Action->ActionName == ActionName)
		{
			if (!Action->CanStartAction(Instigator))
			{
				FString FailedMsg = FString :: Printf(TEXT("Failed to run: %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor :: Red, FailedMsg);
				continue;
			}
			
			Action->StartAction(Instigator);
			return true;
		}	
	}
	return false;
}

bool USActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for (USAction* Action : Actions)
	{
		if (IsValid(Action) && Action->ActionName == ActionName)
		{
			if (Action->GetIsActionRunning())
            {
                Action->StopAction(Instigator);
                return true;
            }
		}	
	}
	return false;
}

USAction* USActionComponent::GetActionByName(FName ActionName) const
{
	for (USAction* Action : Actions)
	{
		if (IsValid(Action) && Action->ActionName == ActionName)
		{
			return Action;
		}
	}
	return nullptr;
}




