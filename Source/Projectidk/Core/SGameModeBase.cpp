// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "AI/SAICharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "PlayerComps/SAttributeComponent.h"

ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 5.0f;
	
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnBot, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
	int32 NoOfAliveBots = 0;
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;
		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(Bot->GetComponentByClass(USAttributeComponent::StaticClass()));
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
		GetWorld()->SpawnActor<AActor>(MinionClass, ResultLocations[0], FRotator::ZeroRotator);
			
		DrawDebugSphere(GetWorld(), ResultLocations[0], 80.0f, 20, FColor::Blue, false, 60.0f);
	}
}