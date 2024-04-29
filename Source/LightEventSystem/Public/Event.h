// Copyright © 2024 Mariusz Kurowski. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Event.generated.h"

/**
* Base class for all events. To use the Event System, you should create a subclass of this class with uproperties and
 * other data relevant for your needs.
 */
UCLASS(Abstract, Blueprintable)
class LIGHTEVENTSYSTEM_API ULES_Event : public UObject
{
	GENERATED_BODY()

public:
	/** The Channel that the Event has been sent on. */
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn), Category = Event)
	FName Channel = NAME_None;

	/** The Object that sent the event. */
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn), Category = Event)
	TObjectPtr<UObject> Sender = nullptr;
};
