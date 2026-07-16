// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, Health, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnRageChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, Rage, float, Delta);

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
struct FRageChangeData
{
	GENERATED_BODY()

	UPROPERTY()
	float Rage = 0.0f;

	UPROPERTY()
	float MaxRage = 0.0f;
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
	
	UPROPERTY()
	FRageChangeData RageData;

public:

	UFUNCTION(BlueprintCallable)
	bool Kill(AActor* InstigatorActor);

	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnRageChanged OnRageChanged;

	UFUNCTION(BlueprintCallable, Category = "Attribute")
	bool ApplyHealthChange(AActor* InstigatorActor, float Delta);
	
	UFUNCTION(BlueprintCallable, Category = "Attribute")
	bool ApplyRageChange(AActor* InstigatorActor, float Delta);

	UFUNCTION(BlueprintCallable)
	bool GetIsAlive() const;

	UFUNCTION(BlueprintCallable)
	float GetHealth() const { return HealthData.Health; }

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const { return HealthData.MaxHealth; }
	
	UFUNCTION(BlueprintCallable)
    float GetRage() const { return RageData.Rage; }
	
    UFUNCTION(BlueprintCallable)
    float GetMaxRage() const { return RageData.MaxRage; }
};