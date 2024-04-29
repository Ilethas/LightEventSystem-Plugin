// Copyright © 2024 Mariusz Kurowski. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "EventSystem.h"
#include "BasicEvents.generated.h"

namespace LES
{
	/** Limits \a TEvent to \a ULES_Event subclasses with an assignable \a Value member. */
	template <typename TEvent, typename TValue>
	concept IsBasicEvent =
		TIsDerivedFrom<TEvent, ULES_Event>::Value &&
		!TIsSame<TEvent, ULES_Event>::Value &&
		requires(TEvent* Event, const TValue& Value)
		{
			Event->Value = Value;
		};

	template <typename TEvent, typename TValue>
		requires IsBasicEvent<TEvent, TValue>
	TEvent* Create(const TValue& Value, UObject* Sender, const FName Channel = NAME_Name,
	               UObject* Outer = GetTransientPackage());

	template <typename TEvent, typename TValue>
		requires IsBasicEvent<TEvent, TValue>
	TEvent* Create(const TValue& Value, UObject* Sender, const FName Channel, UObject* Outer)
	{
		const auto Event = NewObject<TEvent>(Outer);
		Event->Channel = Channel;
		Event->Sender = Sender;
		Event->Value = Value;
		return Event;
	}
}

UCLASS()
class LIGHTEVENTSYSTEM_API ULES_BooleanEvent : public ULES_Event
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn), Category = Event)
	bool Value = bool();
};

UCLASS()
class LIGHTEVENTSYSTEM_API ULES_ByteEvent : public ULES_Event
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn), Category = Event)
	uint8 Value = uint8();
};

UCLASS()
class LIGHTEVENTSYSTEM_API ULES_IntegerEvent : public ULES_Event
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn), Category = Event)
	int32 Value = int32();
};

UCLASS()
class LIGHTEVENTSYSTEM_API ULES_Integer64Event : public ULES_Event
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn), Category = Event)
	int64 Value = int64();
};

UCLASS()
class LIGHTEVENTSYSTEM_API ULES_FloatEvent : public ULES_Event
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn), Category = Event)
	float Value = float();
};

UCLASS()
class LIGHTEVENTSYSTEM_API ULES_DoubleEvent : public ULES_Event
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn), Category = Event)
	double Value = double();
};

UCLASS()
class LIGHTEVENTSYSTEM_API ULES_NameEvent : public ULES_Event
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn), Category = Event)
	FName Value = NAME_None;
};

UCLASS()
class LIGHTEVENTSYSTEM_API ULES_StringEvent : public ULES_Event
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn), Category = Event)
	FString Value = FString();
};

UCLASS()
class LIGHTEVENTSYSTEM_API ULES_TextEvent : public ULES_Event
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn), Category = Event)
	FText Value = FText();
};

UCLASS()
class LIGHTEVENTSYSTEM_API ULES_VectorEvent : public ULES_Event
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn), Category = Event)
	FVector Value = FVector();
};

UCLASS()
class LIGHTEVENTSYSTEM_API ULES_RotatorEvent : public ULES_Event
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn), Category = Event)
	FRotator Value = FRotator();
};

UCLASS()
class LIGHTEVENTSYSTEM_API ULES_TransformEvent : public ULES_Event
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn), Category = Event)
	FTransform Value = FTransform();
};

UCLASS()
class LIGHTEVENTSYSTEM_API ULES_ObjectEvent : public ULES_Event
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn), Category = Event)
	UObject* Value = nullptr;
};

UCLASS()
class UBasicEventsConverters : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure,
		Meta = (DisplayName = "To Boolean (BooleanEvent)",
			CompactNodeTitle = "->",
			Keywords="cast convert",
			BlueprintAutocast),
		Category = "Basic Event | Conversion")
	static bool Conv_BooleanEventToBool(ULES_BooleanEvent* Event);

	UFUNCTION(BlueprintPure,
		Meta = (DisplayName = "To Byte (ByteEvent)",
			CompactNodeTitle = "->",
			Keywords="cast convert",
			BlueprintAutocast
		),
		Category = "Basic Event | Conversion")
	static uint8 Conv_ByteEventToByte(ULES_ByteEvent* Event);

	UFUNCTION(BlueprintPure,
		Meta = (DisplayName = "To Integer (IntegerEvent)",
			CompactNodeTitle = "->",
			Keywords="cast convert",
			BlueprintAutocast
		),
		Category = "Basic Event | Conversion")
	static int32 Conv_IntegerEventToInt(ULES_IntegerEvent* Event);

	UFUNCTION(BlueprintPure,
		Meta = (DisplayName = "To Integer64 (Integer64Event)",
			CompactNodeTitle = "->",
			Keywords="cast convert",
			BlueprintAutocast),
		Category = "Basic Event | Conversion")
	static int64 Conv_Integer64EventToInt64(ULES_Integer64Event* Event);

	UFUNCTION(BlueprintPure,
		Meta = (DisplayName = "To Float (FloatEvent)",
			CompactNodeTitle = "->",
			Keywords="cast convert",
			BlueprintAutocast),
		Category = "Basic Event | Conversion")
	static float Conv_FloatEventToFloat(ULES_FloatEvent* Event);

	UFUNCTION(BlueprintPure,
		Meta = (DisplayName = "To Double (DoubleEvent)",
			CompactNodeTitle = "->",
			Keywords="cast convert",
			BlueprintAutocast),
		Category = "Basic Event | Conversion")
	static double Conv_DoubleEventToDouble(ULES_DoubleEvent* Event);

	UFUNCTION(BlueprintPure,
		Meta = (DisplayName = "To Name (NameEvent)",
			CompactNodeTitle = "->",
			Keywords="cast convert",
			BlueprintAutocast
		),
		Category = "Basic Event | Conversion")
	static FName Conv_NameEventToName(ULES_NameEvent* Event);

	UFUNCTION(BlueprintPure,
		Meta = (DisplayName = "To String (StringEvent)",
			CompactNodeTitle = "->",
			Keywords="cast convert",
			BlueprintAutocast),
		Category = "Basic Event | Conversion")
	static FString Conv_StringEventToString(ULES_StringEvent* Event);

	UFUNCTION(BlueprintPure,
		Meta = (DisplayName = "To Text (TextEvent)",
			CompactNodeTitle = "->",
			Keywords="cast convert",
			BlueprintAutocast
		),
		Category = "Basic Event | Conversion")
	static FText Conv_TextEventToText(ULES_TextEvent* Event);

	UFUNCTION(BlueprintPure,
		Meta = (DisplayName = "To Vector (VectorEvent)",
			CompactNodeTitle = "->",
			Keywords="cast convert",
			BlueprintAutocast),
		Category = "Basic Event | Conversion")
	static FVector Conv_VectorEventToVector(ULES_VectorEvent* Event);

	UFUNCTION(BlueprintPure,
		Meta = (DisplayName = "To Rotator (RotatorEvent)",
			CompactNodeTitle = "->",
			Keywords="cast convert",
			BlueprintAutocast),
		Category = "Basic Event | Conversion")
	static FRotator Conv_RotatorEventToRotator(ULES_RotatorEvent* Event);

	UFUNCTION(BlueprintPure,
		Meta = (DisplayName = "To Transform (TransformEvent)",
			CompactNodeTitle = "->",
			Keywords="cast convert",
			BlueprintAutocast),
		Category = "Basic Event | Conversion")
	static FTransform Conv_TransformEventToTransform(ULES_TransformEvent* Event);

	UFUNCTION(BlueprintPure,
		Meta = (DisplayName = "To Object (ObjectEvent)",
			CompactNodeTitle = "->",
			Keywords="cast convert",
			BlueprintAutocast),
		Category = "Basic Event | Conversion")
	static UObject* Conv_ObjectEventToObject(ULES_ObjectEvent* Event);
};
