// Copyright © 2024 Mariusz Kurowski. All Rights Reserved.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "EventSubsystem.generated.h"

class ULES_EventSystem;

UCLASS()
class LIGHTEVENTSYSTEM_API ULES_EventSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Transient, Category = "Event Subsystem")
	TObjectPtr<ULES_EventSystem> EventSystem;
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(
		BlueprintPure,
		Meta = (WorldContext = "WorldContextObject", CompactNodeTitle = "Global Event System"),
		Category = "Event Subsystem")
	static ULES_EventSystem* GetGlobalEventSystem(const UObject* WorldContextObject);
};
