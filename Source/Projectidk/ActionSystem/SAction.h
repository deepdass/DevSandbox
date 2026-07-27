// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "SAction.generated.h"

class UWorld;

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTIDK_API USAction : public UObject
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(Replicated)
	USActionComponent* ActionComponent;
	
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
	
public:
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack) override;
	
	void Initialize(USActionComponent* NewActionComp);
	
	UFUNCTION(BlueprintCallable, Category = "Action")
	bool GetIsActionRunning() const;
	
	UFUNCTION(BlueprintNativeEvent, Category="Action")
	bool CanStartAction(AActor* Instigator);
	
	UPROPERTY(EditDefaultsOnly, Category="Action")
	bool bAutoStartAction = false;
	
	UFUNCTION(BlueprintNativeEvent, Category="Action")
	void StartAction(AActor* Instigator);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Action")
	void StopAction(AActor* Instigator);
	
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	FName ActionName;
	
	UWorld* GetWorld() const override;
	
	bool IsSupportedForNetworking() const override
	{
		return true;
	}
	
};
