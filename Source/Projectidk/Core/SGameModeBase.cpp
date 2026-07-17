// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SPlayerState.h"
#include "SSaveGame.h"
#include "AI/SAICharacter.h"
#include "AI/SMonsterData.h"
#include "Engine/AssetManager.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerComps/SAttributeComponent.h"
#include "PlayerComps/SCharacter.h"
#include "PlayerComps/SGameplayInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"


static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable bot spawning via timer."), ECVF_Cheat);



ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 5.0f;
	
	CreditPerKill = 15;
	RagePerKill = 20;
	
	SlotName = "SaveGame01";
}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	
	LoadSaveGame();
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnBot, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
}

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	
	ASPlayerState* PlayerState = NewPlayer->GetPlayerState<ASPlayerState>();
	if (PlayerState)
	{
		PlayerState->LoadPlayerState(CurrentSaveGame);
	}
}

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
    ASCharacter* Player = Cast<ASCharacter>(VictimActor);
    if (IsValid(Player))
    {
        AController* Controller = Player->GetController();
        if (!Controller) { return; }

        if (FTimerHandle* ExistingHandle = RespawnTimerHandles.Find(Controller))
        {
            if (GetWorldTimerManager().IsTimerActive(*ExistingHandle))
            {
                return;
            }
        }

        FTimerHandle& TimerHandle = RespawnTimerHandles.FindOrAdd(Controller);

        FTimerDelegate TimerDelegate;
        TimerDelegate.BindUFunction(this, "RespawnPlayerElapsed", Controller);

        float SpawnDelay = 2.5f;
        GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, SpawnDelay, false);
    }
	
	APawn* KillerPawn = Cast<APawn>(Killer);
	
	if (IsValid(KillerPawn))
	{
		ASPlayerState* PlayerState = KillerPawn->GetPlayerState<ASPlayerState>();
		if (ensure(IsValid(PlayerState)))
		{
			PlayerState->ApplyCreditChange(this, CreditPerKill);
			UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, PickupEnvQuery, KillerPawn, EEnvQueryRunMode::AllMatching, nullptr);
			if (IsValid(QueryInstance))
			{
				QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnPickupSpawnQueryCompleted);
			}
		}
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(KillerPawn);
		if (IsValid(AttributeComp))
		{
			AttributeComp->ApplyRageChange(KillerPawn, RagePerKill);
		}
	}
}

void ASGameModeBase::OnPickupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	
	int32 RandomLocIndex = FMath::RandRange(0, Locations.Num() - 1);
	FVector Location = Locations[RandomLocIndex];
	Location.Z += 50.0f;
	
	int32 RandomPickupIndex = FMath::RandRange(0, PickupArray.Num() - 1);
	TSubclassOf<AActor> PickupClass = PickupArray[RandomPickupIndex];
	
	GetWorld()->SpawnActor<AActor>(PickupClass, Location, FRotator::ZeroRotator);
}

void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
    if (IsValid(Controller))
    {
        RespawnTimerHandles.Remove(Controller);

        Controller->UnPossess();
        RestartPlayer(Controller);
    }
}


void ASGameModeBase::SpawnBotTimerElapsed()
{
	if (!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawning of bot is disabled via CVarSpawnBots"));
		return;
	}
	
	int32 NoOfAliveBots = 0;
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if (AttributeComp && AttributeComp->GetIsAlive())
		{
			NoOfAliveBots++;
		}
	}
	
	float MaxBotCount = 3;
	
	if (ensure(DifficultyCurve))
	{
		MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}
	
	if (NoOfAliveBots >= MaxBotCount){ return;}
		
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, EQSpawnBot, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
	
	if (ensure(IsValid(QueryInstance)))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnQueryFinished);
	}
	
}

void ASGameModeBase::OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance,EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("EQS Query failed with status: %d"), (int32)QueryStatus);
		return;
	}
	
	TArray<FVector> ResultLocations = QueryInstance->GetResultsAsLocations();
	if (ResultLocations.IsValidIndex(0))
	{
		if (MonsterDataTable)
		{
			
			TArray<FMonsterInfoRow*> Rows;
			MonsterDataTable->GetAllRows("", Rows);
			
			int32 RandomIndex = FMath::RandRange(0, Rows.Num() - 1);
			FMonsterInfoRow* SelectedRow = Rows[RandomIndex];
			
			UAssetManager* Manager = UAssetManager::GetIfInitialized();
			if (Manager)
			{
				UE_LOG(LogTemp, Log, TEXT("Monster Loading.."));
				TArray<FName> Bundles;
				FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ASGameModeBase::OnMonsterLoaded, SelectedRow->MonsterId, ResultLocations[0]);
				Manager->LoadPrimaryAsset(SelectedRow->MonsterId, Bundles, Delegate);
			}
		}
	}
}

void ASGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedID, FVector SpawnLocation)
{
	UE_LOG(LogTemp, Log, TEXT("Monster Loaded"));
	UAssetManager* Manager = UAssetManager::GetIfInitialized();
	if (Manager)
	{
		USMonsterData* MonsterData = Cast<USMonsterData>(Manager->GetPrimaryAssetObject(LoadedID));
		if (MonsterData)
		{
			GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator);
		}
	}
	DrawDebugSphere(GetWorld(), SpawnLocation, 80.0f, 20, FColor::Blue, false, 60.0f);
}

void ASGameModeBase::KillAll()
{
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if (AttributeComp && AttributeComp->GetIsAlive())
		{
			AttributeComp->Kill(this);
		}
	}
}

void ASGameModeBase::WriteSaveGame()
{
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
	
	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor->Implements<USGameplayInterface>())
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
}

void ASGameModeBase::LoadSaveGame()
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
		
		
		for (FActorIterator It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;
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
	}
	else
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));
		UE_LOG(LogTemp, Log, TEXT("Created New Save Game %s"), *SlotName);
	}
}
