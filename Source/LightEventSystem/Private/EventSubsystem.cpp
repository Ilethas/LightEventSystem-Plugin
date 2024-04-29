// Copyright © 2024 Mariusz Kurowski. All Rights Reserved.


#include "EventSubsystem.h"

#include "EventSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"

void ULES_EventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	EventSystem = NewObject<ULES_EventSystem>(this);
}

ULES_EventSystem* ULES_EventSubsystem::GetGlobalEventSystem(const UObject* WorldContextObject)
{
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	return GameInstance ? GameInstance->GetSubsystem<ULES_EventSubsystem>()->EventSystem : nullptr;
}
