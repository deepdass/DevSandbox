// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, Health, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnCreditChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, int, Credit, int, Delta);

USTRUCT()
struct FHealthChangeData
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* Instigator = nullptr;

	UPROPERTY()
	float Health = 0.0f;

	UPROPERTY()
	float MaxHealth = 0.0f;
};

USTRUCT()
struct FCreditChangeData
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* Instigator = nullptr;

	UPROPERTY()
	int Credit = 0;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTIDK_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Attribute", meta = (DisplayName = "GetAttributeComp"))
	static USAttributeComponent* GetAttributes(AActor* FromActor);

	UFUNCTION(BlueprintCallable, Category = "Attribute", meta = (DisplayName = "IsAlive"))
	static bool GetIsActorAlive(AActor* FromActor);

	USAttributeComponent();

protected:

	UPROPERTY(ReplicatedUsing = "OnRep_HealthData")
	FHealthChangeData HealthData;

	UFUNCTION()
	void OnRep_HealthData(FHealthChangeData OldHealthData);
	
	UPROPERTY(EditDefaultsOnly, Category="Credit")
	bool CanEarnCredit;

	UPROPERTY(ReplicatedUsing = "OnRep_CreditData")
	FCreditChangeData CreditData;

	UFUNCTION()
	void OnRep_CreditData(FCreditChangeData OldCreditData);

public:

	UFUNCTION(BlueprintCallable)
	bool Kill(AActor* InstigatorActor);

	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnCreditChanged OnCreditChanged;

	UFUNCTION(BlueprintCallable, Category = "Attribute")
	bool ApplyHealthChange(AActor* InstigatorActor, float Delta);
	
	UFUNCTION(BlueprintCallable, Category = "Attribute")
	bool ApplyCreditChange(AActor* InstigatorActor, int Credit);

	UFUNCTION(BlueprintCallable)
	bool GetIsAlive() const;

	UFUNCTION(BlueprintCallable)
	float GetHealth() const { return HealthData.Health; }

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const { return HealthData.MaxHealth; }
};