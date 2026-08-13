// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SSaveGameSubsystem.generated.h"

class USSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameSignature, class USSaveGame*, SaveObject);

/**
 * 
 */
UCLASS()
class PROJECTIDK_API USSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
protected:
	USSaveGameSubsystem();
	
	FString SlotName;
	
	UPROPERTY()
	TObjectPtr<USSaveGame> CurrentSaveGame;
	
public:
	
	void HandleStartingNewPlayer(AController* NewPlayer);
	
	UFUNCTION(BlueprintCallable, Category= "SaveGame")
	void WriteSaveGame();
	
	void LoadSaveGame();
	
	
	// If Needed
	UPROPERTY(BlueprintAssignable)
	FOnSaveGameSignature OnSaveGameLoaded;

	UPROPERTY(BlueprintAssignable)
	FOnSaveGameSignature OnSaveGameWritten;
};
