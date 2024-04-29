// Copyright © 2024 Mariusz Kurowski. All Rights Reserved.


#include "BasicEvents.h"

bool UBasicEventsConverters::Conv_BooleanEventToBool(ULES_BooleanEvent* Event)
{
	return Event->Value;
}

uint8 UBasicEventsConverters::Conv_ByteEventToByte(ULES_ByteEvent* Event)
{
	return Event->Value;
}

int32 UBasicEventsConverters::Conv_IntegerEventToInt(ULES_IntegerEvent* Event)
{
	return Event->Value;
}

int64 UBasicEventsConverters::Conv_Integer64EventToInt64(ULES_Integer64Event* Event)
{
	return Event->Value;
}

float UBasicEventsConverters::Conv_FloatEventToFloat(ULES_FloatEvent* Event)
{
	return Event->Value;
}

double UBasicEventsConverters::Conv_DoubleEventToDouble(ULES_DoubleEvent* Event)
{
	return Event->Value;
}

FName UBasicEventsConverters::Conv_NameEventToName(ULES_NameEvent* Event)
{
	return Event->Value;
}

FString UBasicEventsConverters::Conv_StringEventToString(ULES_StringEvent* Event)
{
	return Event->Value;
}

FText UBasicEventsConverters::Conv_TextEventToText(ULES_TextEvent* Event)
{
	return Event->Value;
}

FVector UBasicEventsConverters::Conv_VectorEventToVector(ULES_VectorEvent* Event)
{
	return Event->Value;
}

FRotator UBasicEventsConverters::Conv_RotatorEventToRotator(ULES_RotatorEvent* Event)
{
	return Event->Value;
}

FTransform UBasicEventsConverters::Conv_TransformEventToTransform(ULES_TransformEvent* Event)
{
	return Event->Value;
}

UObject* UBasicEventsConverters::Conv_ObjectEventToObject(ULES_ObjectEvent* Event)
{
	return Event->Value;
}
