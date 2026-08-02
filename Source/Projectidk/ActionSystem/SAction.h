// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "SAction.generated.h"

class UWorld;
class USActionComponent;

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTIDK_API USAction : public UObject
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(Replicated)
	TObjectPtr<USActionComponent> ActionComponent;
	
	UFUNCTION(BlueprintCallable, Category="Action")
	USActionComponent* GetOwningComponent() const;
	
	UPROPERTY(EditDefaultsOnly, Category="Tags")
	FGameplayTagContainer GrantsTags;
	
	UPROPERTY(EditDefaultsOnly, Category="Tags")
	FGameplayTagContainer BlockedTags;
	
	UPROPERTY(ReplicatedUsing="OnRep_IsRunning")
	bool bIsActionRunning = false;
	
	UFUNCTION()
	void OnRep_IsRunning();
	
	UPROPERTY(EditDefaultsOnly, Category="Action")
    bool bAutoStartAction = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	FGameplayTag NameTag;
	
public:
	
	UFUNCTION(BlueprintPure)
	FGameplayTag GetNameTag() const
	{
		return NameTag;
	}
	
	void Initialize(USActionComponent* NewActionComp);
	
	UFUNCTION(BlueprintCallable, Category="Action")
	bool GetAutoStartAction() const 
	{
		return bAutoStartAction;
	}
	
	UFUNCTION(BlueprintCallable, Category = "Action")
	bool GetIsActionRunning() const;
	
	UFUNCTION(BlueprintNativeEvent, Category="Action")
	bool CanStartAction(AActor* Instigator);
	
	UFUNCTION(BlueprintNativeEvent, Category="Action")
	void StartAction(AActor* Instigator);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Action")
	void StopAction(AActor* Instigator);
	
	virtual  UWorld* GetWorld() const override;
	
	virtual bool IsSupportedForNetworking() const override
	{
		return true;
	}
	
};
