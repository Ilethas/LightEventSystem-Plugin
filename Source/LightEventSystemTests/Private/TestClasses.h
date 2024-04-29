// Copyright © 2024 Mariusz Kurowski. All Rights Reserved.

#pragma once

#include "EventSystem.h"
#include "TestClasses.generated.h"

UCLASS(HideDropdown)
class ULES_TestEvent : public ULES_Event
{
	GENERATED_BODY()
};

UCLASS(HideDropdown)
class ULES_DerivedEvent : public ULES_TestEvent
{
	GENERATED_BODY()
};

UCLASS(HideDropdown)
class ULES_OtherTestEvent : public ULES_Event
{
	GENERATED_BODY()
};

UCLASS(HideDropdown)
class ULES_TestObserver : public UObject
{
	GENERATED_BODY()

public:
	FIntVector3 Counter = FIntVector3::ZeroValue;

	UFUNCTION()
	void OnTestEvent(const ULES_TestEvent* TestEvent)
	{
		Counter += FIntVector3(1, 0, 0);
	}

	UFUNCTION()
	void OnDerivedEvent(const ULES_DerivedEvent* DerivedEvent)
	{
		Counter += FIntVector3(0, 1, 0);
	}

	UFUNCTION()
	void OnOtherTestEvent(const ULES_OtherTestEvent* OtherTestEvent)
	{
		Counter += FIntVector3(0, 0, 1);
	}

	UFUNCTION()
	void OnEvent(ULES_Event* Event)
	{
		Counter += FIntVector3(1, 0, 0);
	}
};
