// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

class USSaveGame;
class ASPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnCreditChanged, AActor*, InstigatorActor, ASPlayerState*, OwningState, int32, Credit, int32, Delta);

USTRUCT()
struct FCreditData
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* Instigator = nullptr;

	UPROPERTY()
	int32 Credit = 0;
};

UCLASS()
class PROJECTIDK_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	ASPlayerState();

	UFUNCTION(BlueprintCallable, Category = "Credit")
	bool ApplyCreditChange(AActor* InstigatorActor, int32 Credit);

	UFUNCTION(BlueprintCallable, Category = "Credit")
	int32 GetCredit() const { return CreditData.Credit;}

	UPROPERTY(BlueprintAssignable)
	FOnCreditChanged OnCreditChanged;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(USSaveGame* SaveObject);
	
	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(USSaveGame* SaveObject);
	
protected:

	// Whether this player is allowed to earn/spend credit at all
	UPROPERTY(EditDefaultsOnly, Category = "Credit")
	bool CanEarnCredit;

	UPROPERTY(ReplicatedUsing = "OnRep_CreditData")
	FCreditData CreditData;

	UFUNCTION()
	void OnRep_CreditData(FCreditData OldCreditData);
	
};