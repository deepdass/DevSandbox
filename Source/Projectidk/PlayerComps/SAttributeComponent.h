// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAttributeChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewValue, float, Delta);

USTRUCT(BlueprintType)
struct FAttribute
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* Instigator = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Attribute")
	float Value;

	UPROPERTY(EditAnywhere, Category = "Attribute")
	float MaxValue;
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
	
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = "OnRep_HealthData", Category = "Health")
	FAttribute HealthData;

	UFUNCTION()
	void OnRep_HealthData(FAttribute OldHealthData);

	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = "OnRep_RageData", Category = "Rage")
	FAttribute RageData;

	UFUNCTION()
	void OnRep_RageData(FAttribute OldRageData);

public:

	UFUNCTION(BlueprintCallable)
	bool Kill(AActor* InstigatorActor);

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnRageChanged;

	UFUNCTION(BlueprintCallable, Category = "Attribute")
	bool ApplyHealthChange(AActor* InstigatorActor, float Delta);

	UFUNCTION(BlueprintCallable, Category = "Attribute")
	bool ApplyRageChange(AActor* InstigatorActor, float Delta);

	UFUNCTION(BlueprintCallable)
	bool GetIsAlive() const;

	UFUNCTION(BlueprintCallable)
	float GetHealth() const { return HealthData.Value; }

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const { return HealthData.MaxValue; }

	UFUNCTION(BlueprintCallable)
	float GetRage() const { return RageData.Value; }

	UFUNCTION(BlueprintCallable)
	float GetMaxRage() const { return RageData.MaxValue; }

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};