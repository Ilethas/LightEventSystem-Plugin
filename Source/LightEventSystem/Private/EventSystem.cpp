// Copyright © 2024 Mariusz Kurowski. All Rights Reserved.


#include "EventSystem.h"

FLES_ObserverHandle ULES_EventSystem::BP_AddObserver_Event(const TSubclassOf<ULES_Event>& EventClass, UObject* Observer,
                                                           FLES_EventHandler Callback, const FName Channel)
{
	if (!IsValid(Observer) || !IsValid(EventClass)) return {};

	auto CallbackLambda = [Observer = TWeakObjectPtr<>(Observer), Callback](ULES_Event* Event)
	{
		if (Observer.IsValid())
			Callback.ExecuteIfBound(Event);
	};
	return AddObserver_Private(EventClass, Observer, CallbackLambda, Channel);
}

FLES_ObserverHandle ULES_EventSystem::BP_AddObserver_Function(const TSubclassOf<ULES_Event>& EventClass,
                                                              UObject* Observer,
                                                              const FName FunctionName, const FName Channel)
{
	if (!IsValid(Observer) || !IsValid(EventClass)) return {};

	UFunction* Callback = FindCallbackFunction(Observer, FunctionName);
	if (!Callback) return {};

	auto CallbackLambda = [
			Observer = TWeakObjectPtr<>(Observer),
			Callback = TWeakObjectPtr<UFunction>(Callback)
		](ULES_Event* Event)
	{
		if (Observer.IsValid() && Callback.IsValid())
		{
			struct
			{
				ULES_Event* Event;
			} FuncParams;

			FuncParams.Event = Event;
			Observer->ProcessEvent(Callback.Get(), &FuncParams);
		}
	};
	return AddObserver_Private(EventClass, Observer, CallbackLambda, Channel);
}

void ULES_EventSystem::SendEvent(ULES_Event* Event)
{
	if (!IsValid(Event)) return;
	if (!BeforeSend(Event)) return;

	TArray<FRecord> Records;
	ObserverRecords.MultiFind({TStrongObjectPtr(Event->GetClass()), Event->Channel}, Records);
	for (const auto& Record : Records)
	{
		if (!Record->Observer.IsValid()) continue;
		if (BeforeReceive(Event, Record->Observer.Get()))
		{
			Record->Callback(Event);
			AfterReceive(Event, Record->Observer.Get());
		}
	}
	AfterSend(Event);
}

int ULES_EventSystem::Clean()
{
	int Count = 0;
	for (auto It = ObserverRecords.CreateIterator(); It; ++It)
	{
		if (!It.Value()->Observer.IsValid())
		{
			It.RemoveCurrent();
			Count++;
		}
	}
	return Count;
}

int ULES_EventSystem::RemoveByHandle(const FLES_ObserverHandle& ObserverHandle)
{
	if (!IsHandleValid(ObserverHandle)) return 0;

	const FKey Key{ObserverHandle.ObserverKey.Key.Get(), ObserverHandle.ObserverKey.Value};
	return ObserverRecords.RemoveSingle(Key, ObserverHandle.ObserverRecord.Pin());
}

int ULES_EventSystem::RemoveByObserver(const UObject* Observer)
{
	int Count = 0;
	for (auto It = ObserverRecords.CreateIterator(); It; ++It)
	{
		if (It.Value()->Observer == Observer)
		{
			It.RemoveCurrent();
			Count++;
		}
	}
	return Count;
}

void ULES_EventSystem::RemoveAll()
{
	ObserverRecords.Empty();
}

int ULES_EventSystem::Num() const
{
	return ObserverRecords.Num();
}

int ULES_EventSystem::GetChannels(TArray<FName>& OutChannels) const
{
	TArray<FKey> Keys;
	ObserverRecords.GetKeys(Keys);

	OutChannels.Empty();
	for (const auto& [EventClass, Channel] : Keys)
		OutChannels.AddUnique(Channel);
	return OutChannels.Num();
}

bool ULES_EventSystem::ContainsObserver(const UObject* Observer) const
{
	for (const auto& [Key, Record] : ObserverRecords)
	{
		if (Record->Observer == Observer)
		{
			return true;
		}
	}
	return false;
}

bool ULES_EventSystem::ContainsValidHandle(const FLES_ObserverHandle& ObserverHandle) const
{
	if (!IsHandleValid(ObserverHandle)) return false;

	const FKey Key{ObserverHandle.ObserverKey.Key.Get(), ObserverHandle.ObserverKey.Value};
	return ObserverRecords.Contains(Key);
}

bool ULES_EventSystem::IsHandleValid(const FLES_ObserverHandle& ObserverHandle)
{
	return ObserverHandle.ObserverRecord.IsValid() && ObserverHandle.ObserverKey.Key.IsValid();
}

UObject* ULES_EventSystem::GetObserver(const FLES_ObserverHandle& ObserverHandle)
{
	if (ObserverHandle.ObserverRecord.IsValid())
	{
		const TSharedPtr<LES::FObserverRecord> ObserverRecord = ObserverHandle.ObserverRecord.Pin();
		if (ObserverRecord->Observer.IsValid())
			return ObserverRecord->Observer.Get();
	}
	return nullptr;
}

UClass* ULES_EventSystem::GetEventClass(const FLES_ObserverHandle& ObserverHandle)
{
	const TWeakObjectPtr<UClass> EventClass = ObserverHandle.ObserverKey.Key;
	return EventClass.IsValid() ? EventClass.Get() : nullptr;
}

FName ULES_EventSystem::GetChannel(const FLES_ObserverHandle& ObserverHandle)
{
	return ObserverHandle.ObserverKey.Value;
}

bool ULES_EventSystem::BeforeSend_Implementation(ULES_Event* Event)
{
	return true;
}

bool ULES_EventSystem::BeforeReceive_Implementation(ULES_Event* Event, UObject* Observer)
{
	return true;
}

void ULES_EventSystem::AfterReceive_Implementation(ULES_Event* Event, UObject* Observer)
{
}

void ULES_EventSystem::AfterSend_Implementation(ULES_Event* Event)
{
}

UFunction* ULES_EventSystem::FindCallbackFunction(const UObject* Object, const FName FunctionName)
{
	UFunction* FuncPtr = Object->FindFunction(FunctionName);

	TArray<FProperty*> InParams;
	TArray<FProperty*> OutParams;
	for (TFieldIterator<FProperty> It(FuncPtr); It && (It->PropertyFlags & CPF_Parm); ++It)
	{
		if (It->PropertyFlags & CPF_OutParm)
			OutParams.Add(*It);
		else
			InParams.Add(*It);
	}

	if (FuncPtr && InParams.Num() == 1 && OutParams.Num() == 0)
	{
		const FObjectProperty* EventProperty = CastField<FObjectProperty>(InParams[0]);
		if (EventProperty && EventProperty->PropertyClass->IsChildOf(ULES_Event::StaticClass()))
			return FuncPtr;
	}
	return nullptr;
}

FLES_ObserverHandle ULES_EventSystem::AddObserver_Private(const TSubclassOf<ULES_Event>& EventClass, UObject* Observer,
                                                          TFunction<void(ULES_Event*)>&& Callback, const FName Channel)
{
	const LES::FObserverRecord NewRecord{
		.Channel = Channel,
		.Observer = Observer,
		.Callback = Callback,
	};
	const TSharedPtr<LES::FObserverRecord> ObserverRecord = MakeShared<LES::FObserverRecord>(NewRecord);

	ObserverRecords.Add(FKey{EventClass, Channel}, ObserverRecord);
	return {{EventClass, Channel}, ObserverRecord};
}
