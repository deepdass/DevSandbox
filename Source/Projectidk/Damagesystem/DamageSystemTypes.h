// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageSystemTypes.generated.h"

UENUM(BlueprintType)
enum class EDamageType : uint8
{
	None UMETA(DisplayName = "None"),
	Physical UMETA(DisplayName = "Physical"),
	Elemental UMETA(DisplayName = "Elemental"),
	Enviorment UMETA(DisplayName = "Enviormant"),
};

UENUM(BlueprintType)
enum class EDamageResponse : uint8
{
	None UMETA(DisplayName = "None"), 
	HitReaction UMETA(DisplayName = "HitReaction"),
	Knockback UMETA(DisplayName = "Knockback"),
	Stagger UMETA(DisplayName = "Stagger"),
	ElementalEff UMETA(DisplayName = "ElementalEff"),
};

USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Damage")
	float DamageAmount;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Damage")
	AActor* DamageCauser;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Damage")
	bool CanBeBloacked = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Damage")
	bool CanBeParried = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Damage")
	bool ShouldDamageInvincible = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Damage")
	bool ShouldForceInterrupt = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Damage")
	EDamageType DamageType = EDamageType::None;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Damage")
	EDamageResponse DamageResponse = EDamageResponse::None; 
};

