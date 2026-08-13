// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "SCheatManager.generated.h"

UENUM(BlueprintType)
enum class EMoveAxis : uint8
{
	X,
	Y,
	Z
};

/**
 * 
 */
UCLASS()
class PROJECTIDK_API USCheatManager : public UCheatManager
{
	GENERATED_BODY()
	
	
	
public:
	
	UFUNCTION(Exec)
	void HealSelf(float Amount = -1.0f);
	
	UFUNCTION(Exec)
	void DamageSelf(float Amount = -1.0f);
	
	UFUNCTION(Exec)
	void GrantCoin(int32 Amount = 1000);

	UFUNCTION(Exec)
	void MoveInDirectionBy(float ByCm = 200, EMoveAxis Axis = EMoveAxis::Z);
	
	UFUNCTION(Exec)
	void KillAll();
	
	UFUNCTION(Exec)
	void DeleteSaveGame();
};
