// Copyright © 2024 Mariusz Kurowski. All Rights Reserved.

#pragma once

#include "ObserverHandle.h"
#include "Templates/SubclassOf.h"
#include "UObject/StrongObjectPtr.h"
#include "EventSystem.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FLES_EventHandler, ULES_Event*, Event);

namespace LES
{
	/** Constrains the \a Callback to be a method of the \a Observer with 1 parameter of the \a TEvent type. */
	template <typename TObserver, typename TCallback, typename TEvent>
	concept IsMethodEventHandler = requires(TObserver Observer, TCallback Callback, TEvent* Event)
	{
		(Observer.*Callback)(Event);
	};

	/** Constrains the \a Callback to be a callable with 1 parameter of the \a TEvent type. */
	template <typename TCallback, typename TEvent>
	concept IsFunctorEventHandler = requires(TCallback Callback, TEvent* Event)
	{
		Callback(Event);
	};
}

/**
 * Object responsible for tracking which observer listens for what kind of events, and on what channel. Note that the
 * Event System, after adding an observer, doesn't keep it alive. It means that if nothing in the game references the
 * observer, except the for the Event System, it will be garbage-collected.
 */
UCLASS(Blueprintable)
class LIGHTEVENTSYSTEM_API ULES_EventSystem : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Adds the \a Observer to the Event System and marks it as listening for events of \a TEvent type, that are sent on
	 * the specified \a Channel. Example usage:
	 *
	 * UObserver* Observer = NewObject<UObserver>();\n
	 * EventSystem->AddObserver<UMyEvent>(Observer, &UObserver::OnMyEvent, "Some channel");\n
	 *
	 * UObserver should have a handler method defined like follows:\n
	 * void UObserver::OnMyEvent(const UMyEvent* Event)\n
	 * {\n
	 *		// Your code handling the event.\n
	 * }
	 *
	 * The \a Callback will not be called if an \a Event is sent and the \a Observer has already been garbage-collected.
	 * 
	 * @tparam TEvent The type of the event you want the \a Observer to listen for. You should explicitly specify this
	 * type, like in the example above.
	 * @tparam TObserver The type of the \a Observer object.
	 * @tparam TCallback The type of the \a Callback method.
	 * @param Observer The object that will be notified when events of \a TEvent type are sent on the \a Channel.
	 * @param Callback The event handler that will be called when the event is received.
	 * @param Channel Determines the channel the event will be sent on. Observers are notified only about the events
	 * sent on the channel they're listening on.
	 * @return A handle to the newly created observer record in the Event System. You may use this handle later to
	 * remove this particular observer record from the Event System.
	 */
	template <typename TEvent, typename TObserver, typename TCallback>
		requires TIsDerivedFrom<TObserver, UObject>::Value &&
		TIsDerivedFrom<TEvent, ULES_Event>::Value &&
		!TIsSame<TEvent, ULES_Event>::Value &&
		LES::IsMethodEventHandler<TObserver, TCallback, TEvent>
	FLES_ObserverHandle AddObserver(TObserver* Observer, TCallback Callback, const FName Channel = NAME_None);

	/**
	 * Adds the \a Observer to the Event System and marks it as listening for events of \a TEvent type, that are sent on
	 * the specified \a Channel. Example usage:
	 *
	 * UObserver* Observer = NewObject<UObserver>();\n
	 * EventSystem->AddObserver<UMyEvent>(Observer, [](UMyEvent* MyEvent)\n
	 * {\n
	 *		// Your code handling the event.\n
	 * }, "Some channel");\n
	 *
	 * The \a Callback will not be called if an \a Event is sent and the \a Observer has already been garbage-collected.
	 * 
	 * @tparam TEvent The type of the event you want the \a Observer to listen for. You should explicitly specify this
	 * type, like in the example above.
	 * @tparam TObserver The type of the \a Observer object.
	 * @tparam TCallback The type of the \a Callback functor.
	 * @param Observer The object that will be notified when events of \a TEvent type are sent on the \a Channel.
	 * @param Callback The event handler that will be called when the event is received.
	 * @param Channel Determines the channel the event will be sent on. Observers are notified only about the events
	 * sent on the channel they're listening on.
	 * @return A handle to the newly created observer record in the Event System. You may use this handle later to
	 * remove this particular observer record from the Event System.
	 */
	template <typename TEvent, typename TObserver, typename TCallback>
		requires TIsDerivedFrom<TObserver, UObject>::Value &&
		TIsDerivedFrom<TEvent, ULES_Event>::Value &&
		!TIsSame<TEvent, ULES_Event>::Value &&
		LES::IsFunctorEventHandler<TCallback, TEvent>
	FLES_ObserverHandle AddObserver(TObserver* Observer, TCallback Callback, const FName Channel = NAME_None);

	/**
	 * Adds the \a Observer to the Event System and marks it as listening for events of \a EventClass type, that are
	 * sent on the specified \a Channel. The \a Callback will not be called if an \a Event is sent and the \a Observer
	 * has already been garbage-collected.
	 * 
	 * @param EventClass The static class of the events the Observer should listen for.
	 * @param Observer The object that will be notified when events of \a EventClass type are sent on the \a Channel.
	 * @param Callback The event handler that will be called when the event is received.
	 * @param Channel Determines the channel the event will be sent on. Observers are notified only about the events
	 * sent on the channel they're listening on.
	 * @return A handle to the newly created observer record in the Event System. You may use this handle later to
	 * remove this particular observer record from the Event System.
	 */
	UFUNCTION(
		BlueprintCallable,
		Meta = (DisplayName = "Add Observer (Event)", DefaultToSelf = "Observer", AutoCreateRefTerm = "EventClass"),
		Category = "Event System")
	FLES_ObserverHandle BP_AddObserver_Event(
		UPARAM(Meta = (AllowAbstract = "false")) const TSubclassOf<ULES_Event>& EventClass, UObject* Observer,
		FLES_EventHandler Callback, const FName Channel = NAME_None);

	/**
	 * Adds the \a Observer to the Event System and marks it as listening for events of \a EventClass type, that are
	 * sent on the specified \a Channel. The event handler will not be called if an \a Event is sent and the \a Observer
	 * has already been garbage-collected.
	 * 
	 * @param EventClass The static class of the events the Observer should listen for.
	 * @param Observer The object that will be notified when events of \a TEvent type are sent on the \a Channel.
	 * @param FunctionName The name of the event handler function that will be called when the event is received. This
	 * function should take 1 parameter of the type of your event and return no values.
	 * @param Channel Determines the channel the event will be sent on. Observers are notified only about the events
	 * sent on the channel they're listening on.
	 * @return A handle to the newly created observer record in the Event System. You may use this handle later to
	 * remove this particular observer record from the Event System. If \a FunctionName is not the name of a
	 * blueprint-callable member function of the \a Observer, an invalid handle is returned.
	 */
	UFUNCTION(
		BlueprintCallable,
		Meta = (DisplayName = "Add Observer (Function)", DefaultToSelf = "Observer", AutoCreateRefTerm = "EventClass"),
		Category = "Event System")
	FLES_ObserverHandle BP_AddObserver_Function(
		UPARAM(Meta = (AllowAbstract = "false")) const TSubclassOf<ULES_Event>& EventClass, UObject* Observer,
		const FName FunctionName, const FName Channel = NAME_None);

	/**
	 * Sends the \a Event to all observers listening for this type of event on the channel.
	 * 
	 * @param Event Event object that will be sent.
	 */
	UFUNCTION(BlueprintCallable, Category = "Event System")
	void SendEvent(ULES_Event* Event);

	/**
	 * Removes all observer records that are associated with garbage-collected Observers.
	 * 
	 * @return Amount of observer records removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Event System")
	int Clean();

	/**
	 * Removes the observer record referenced by the \a ObserverHandle.
	 * 
	 * @param ObserverHandle Handle used to remove the observer record.
	 * @return Amount of observer records removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Event System")
	int RemoveByHandle(const FLES_ObserverHandle& ObserverHandle);

	/**
	 * Removes all observer records associated with the \a Observer.
	 * 
	 * @param Observer The Observer object that should be entirely removed from the Event System.
	 * @return Amount of observer records removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Event System")
	int RemoveByObserver(const UObject* Observer);

	/** Removes all observer records in the Event System. */
	UFUNCTION(BlueprintCallable, Category = "Event System")
	void RemoveAll();

	/** Returns the total amount of observer records in the Event System. */
	UFUNCTION(BlueprintPure, Category = "Event System")
	int Num() const;

	/**
	 * Fetches all the channels the observers are currently listening on.
	 * 
	 * @param OutChannels Unique list of channels used in the Event System.
	 * @return Amount of unique channels.
	 */
	UFUNCTION(BlueprintPure, Category = "Event System")
	int GetChannels(TArray<FName>& OutChannels) const;

	/** Returns true if \a Observer has been added to the Event System. */
	UFUNCTION(BlueprintPure, Category = "Event System")
	bool ContainsObserver(const UObject* Observer) const;

	/**
	 * Returns true if \a ObserverHandle is valid and references an observer record contained in this particular Event
	 * System object.
	 */
	UFUNCTION(BlueprintPure, Category = "Event System | Observer Handle")
	bool ContainsValidHandle(const FLES_ObserverHandle& ObserverHandle) const;

	/**
	 * Checks if the \a ObserverHandle is valid. A Handle can only become invalid if the Event System no longer contains
	 * an observer record referenced by this handle.
	 */
	UFUNCTION(BlueprintPure, Category = "Event System | Observer Handle")
	static bool IsHandleValid(const FLES_ObserverHandle& ObserverHandle);

	/** Retrieve the Observer associated with this \a ObserverHandle. Returns nullptr if the handle is invalid. */
	UFUNCTION(BlueprintPure, Category = "Event System | Observer Handle")
	static UObject* GetObserver(const FLES_ObserverHandle& ObserverHandle);

	/** Retrieve the event class associated with this \a ObserverHandle. Returns nullptr if the handle is invalid. */
	UFUNCTION(BlueprintPure, Category = "Event System | Observer Handle")
	static UClass* GetEventClass(const FLES_ObserverHandle& ObserverHandle);

	/** Returns the channel associated with this \a ObserverHandle. */
	UFUNCTION(BlueprintPure, Category = "Event System | Observer Handle")
	static FName GetChannel(const FLES_ObserverHandle& ObserverHandle);

	/**
	 * A hook method that is called for each \a Event before its sent. You may override it in a subclass to add your own
	 * custom logic. If \a BeforeSend returns false, the Event will not be sent. By default, all events are sent.
	 * 
	 * @param Event The event object that should be sent.
	 * @return Indicates whether the \a Event will be sent or not.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Event System | Hooks")
	bool BeforeSend(ULES_Event* Event);

	/**
	 * A hook method that is called for each \a Observer that should receive the \a Event. You may override it in a
	 * subclass to add your own custom logic. If \a BeforeReceive returns false, the \a Observer will not receive this
	 * particular \a Event. By default, all events are received.
	 * 
	 * @param Event The event object that should be received.
	 * @param Observer The observer object that should receive the event.
	 * @return Indicates whether the \a Event should be received by the \a Observer.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Event System | Hooks")
	bool BeforeReceive(ULES_Event* Event, UObject* Observer);

	/**
	 * A hook method that is called for each \a Observer that received the \a Event. You may override it in a subclass
	 * to add your own custom logic. A typical use-case would be to perform some kind of cleanup or logging.
	 * 
	 * @param Event The event object that was just received by the observer.
	 * @param Observer The observer that just received the event.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Event System | Hooks")
	void AfterReceive(ULES_Event* Event, UObject* Observer);

	/**
	 * A hook method that is called after all observers have received the \a Event, if any. You may override it in a
	 * subclass to add your own custom logic. A typical use-case would be to perform some king of cleanup or logging.
	 * 
	 * @param Event The event object that was sent.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Event System | Hooks")
	void AfterSend(ULES_Event* Event);

private:
	using FKey = TPair<TStrongObjectPtr<UClass>, FName>;
	using FRecord = TSharedPtr<LES::FObserverRecord>;

	TMultiMap<FKey, FRecord> ObserverRecords;

	/** Looks for blueprint-callable method called \a FunctionName in the \a Object. Returns nullptr if not found. */
	static UFunction* FindCallbackFunction(const UObject* Object, const FName FunctionName);

	FLES_ObserverHandle AddObserver_Private(const TSubclassOf<ULES_Event>& EventClass, UObject* Observer,
	                                        TFunction<void(ULES_Event*)>&& Callback, const FName Channel);
};

template <typename TEvent, typename TObserver, typename TCallback>
	requires TIsDerivedFrom<TObserver, UObject>::Value &&
	TIsDerivedFrom<TEvent, ULES_Event>::Value &&
	!TIsSame<TEvent, ULES_Event>::Value &&
	LES::IsMethodEventHandler<TObserver, TCallback, TEvent>
FLES_ObserverHandle ULES_EventSystem::AddObserver(TObserver* Observer, TCallback Callback, const FName Channel)
{
	if (!IsValid(Observer)) return {};

	auto CallbackLambda = [Observer = TWeakObjectPtr<TObserver>(Observer), Callback](ULES_Event* Event)
	{
		if (Observer.IsValid())
			(Observer.Get()->*Callback)(static_cast<TEvent*>(Event));
	};
	return AddObserver_Private(TEvent::StaticClass(), Observer, CallbackLambda, Channel);
}

template <typename TEvent, typename TObserver, typename TCallback>
	requires TIsDerivedFrom<TObserver, UObject>::Value &&
	TIsDerivedFrom<TEvent, ULES_Event>::Value &&
	!TIsSame<TEvent, ULES_Event>::Value &&
	LES::IsFunctorEventHandler<TCallback, TEvent>
FLES_ObserverHandle ULES_EventSystem::AddObserver(TObserver* Observer, TCallback Callback, const FName Channel)
{
	if (!IsValid(Observer)) return {};

	auto CallbackLambda = [Observer = TWeakObjectPtr<TObserver>(Observer), Callback](ULES_Event* Event)
	{
		if (Observer.IsValid())
			Callback(static_cast<TEvent*>(Event));
	};
	return AddObserver_Private(TEvent::StaticClass(), Observer, CallbackLambda, Channel);
}
