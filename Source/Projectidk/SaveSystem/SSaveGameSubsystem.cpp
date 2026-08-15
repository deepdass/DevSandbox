// Fill out your copyright notice in the Description page of Project Settings.


#include "SSaveGameSubsystem.h"

#include "EngineUtils.h"
#include "Core/SPlayerState.h"
#include "SaveSystem/SSaveGame.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerComps/SGameplayInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"


USSaveGameSubsystem::USSaveGameSubsystem()
{
	SlotName = "SaveGame01";
}

void USSaveGameSubsystem::HandleStartingNewPlayer(AController* NewPlayer)
{
	ASPlayerState* PS = NewPlayer->GetPlayerState<ASPlayerState>();
	if (ensure(PS))
	{
		PS->LoadPlayerState(CurrentSaveGame);
	}
}


void USSaveGameSubsystem::WriteSaveGame()
{
	AGameStateBase* GameState = GetWorld()->GetGameState();
	check(GameState);
	
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		ASPlayerState* PlayerState = Cast<ASPlayerState>(GameState->PlayerArray[i]);
		if (PlayerState)
		{
			PlayerState->SavePlayerState(CurrentSaveGame);
			break; //SinglePlayer
		}
	}
	
	CurrentSaveGame->SavedActors.Empty();
	
	for (AActor* Actor : TActorRange<AActor>(GetWorld()))
	{
		if (!IsValid(Actor) || !Actor->Implements<USGameplayInterface>())
		{
			continue;
		}
		
		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetName();
		ActorData.ActorTransform = Actor->GetActorTransform();
		
		FMemoryWriter MemWriter(ActorData.ByteData);
		
		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
		Ar.ArIsSaveGame = true;
		
		Actor->Serialize(Ar);
		
		CurrentSaveGame->SavedActors.Add(ActorData);
	}
	
	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);
	OnSaveGameWritten.Broadcast(CurrentSaveGame);
}

void USSaveGameSubsystem::LoadSaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if (CurrentSaveGame == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't Load Save Game %s"), *SlotName);
			return;
		}
		UE_LOG(LogTemp, Log, TEXT("Loaded Save Game %s"), *SlotName);
		
		
		for (AActor* Actor : TActorRange<AActor>(GetWorld()))
		{
			if (!Actor->Implements<USGameplayInterface>())
			{
				continue;
			}
		
			for (FActorSaveData ActorData : CurrentSaveGame->SavedActors)
			{
				if (ActorData.ActorName == Actor->GetName())
				{
					Actor->SetActorTransform(ActorData.ActorTransform);
					
					FMemoryReader MemReader(ActorData.ByteData);
					
					FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
					Ar.ArIsSaveGame = true;
		
					Actor->Serialize(Ar);
					
					ISGameplayInterface::Execute_OnActorLoaded(Actor);
					
					break;
				}
			}
		}
		OnSaveGameLoaded.Broadcast(CurrentSaveGame);
	}
	else
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));
		UE_LOG(LogTemp, Log, TEXT("Created New Save Game %s"), *SlotName);
	}
}


