// Copyright © 2024 Mariusz Kurowski. All Rights Reserved.

#pragma once

#include "Event.h"
#include "ObserverHandle.generated.h"

namespace LES
{
	struct FObserverRecord
	{
		FName Channel = NAME_None;
		TWeakObjectPtr<> Observer = nullptr;
		TFunction<void(ULES_Event*)> Callback = nullptr;
	};
}

/**
 * Tracks an observer record within the Event System. You may use a handle to unregister an object from the Event
 * System. If an observer record associated with the handle is ever removed from the Event System, the handle becomes
 * invalid.
 */
USTRUCT(BlueprintType)
struct FLES_ObserverHandle
{
	GENERATED_BODY()

	/** Listened-to event class and the channel */
	TPair<TWeakObjectPtr<UClass>, FName> ObserverKey = {nullptr, NAME_None};

	TWeakPtr<LES::FObserverRecord> ObserverRecord = nullptr;
};
