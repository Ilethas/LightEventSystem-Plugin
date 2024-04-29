// Copyright © 2024 Mariusz Kurowski. All Rights Reserved.

#include "TestClasses.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLES_ObserverLifetimeTest, "Light Event System.Observer lifetime",
                                 EAutomationTestFlags::ApplicationContextMask |
                                 EAutomationTestFlags::HighPriority |
                                 EAutomationTestFlags::ProductFilter)

bool FLES_ObserverLifetimeTest::RunTest(const FString& Parameters)
{
	auto EventSystem = TStrongObjectPtr(NewObject<ULES_EventSystem>());
	ULES_TestObserver* TestObserver = NewObject<ULES_TestObserver>();

	const TWeakObjectPtr<> ObserverPtr = TestObserver;
	const auto Handle1 = EventSystem->AddObserver<ULES_TestEvent>(TestObserver, &ULES_TestObserver::OnTestEvent);
	const auto Handle2 = EventSystem->AddObserver<ULES_TestEvent>(TestObserver, [this](const ULES_TestEvent*)
	{
		AddError(TEXT("Event handlers should not be called for garbage-collected objects"));
	});

	TestEqual(TEXT("Observer should be alive before garbage collection"), ObserverPtr.IsValid(), true);
	CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
	TestEqual(TEXT("Observer should be destroyed after garbage collection"), ObserverPtr.IsValid(), false);

	// Should do nothing because the observer was garbage-collected.
	EventSystem->SendEvent(NewObject<ULES_TestEvent>());

	TestEqual(TEXT("Expected to contain 2 observer records"), EventSystem->Num(), 2);
	EventSystem->Clean();
	TestEqual(TEXT("Expected to be empty"), EventSystem->Num(), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLES_AddingObserversTest, "Light Event System.Adding observers",
                                 EAutomationTestFlags::ApplicationContextMask |
                                 EAutomationTestFlags::HighPriority |
                                 EAutomationTestFlags::ProductFilter)

bool FLES_AddingObserversTest::RunTest(const FString& Parameters)
{
	ULES_TestObserver* NullObserver = nullptr;
	ULES_EventSystem* EventSystem = NewObject<ULES_EventSystem>();
	ULES_TestObserver* TestObserver = NewObject<ULES_TestObserver>();

	const FName Channel1 = "Some channel";
	const FName Channel2 = "Some other channel";

	// Adding regular observers.
	TestFalse(TEXT("Shouldn't contain the observer before registration"), EventSystem->ContainsObserver(TestObserver));
	{
		const auto Handle = EventSystem->AddObserver<ULES_TestEvent>(TestObserver, &ULES_TestObserver::OnTestEvent,
		                                                             Channel1);
		TestTrue(TEXT("Adding observers should return valid handles"), ULES_EventSystem::IsHandleValid(Handle));
		TestTrue(TEXT("Should contain the handle"), EventSystem->ContainsValidHandle(Handle));
	}
	{
		const auto Handle = EventSystem->AddObserver<ULES_TestEvent>(TestObserver, [](const ULES_TestEvent*)
		{
			// Nothing needs to be done in the handler, we're interested only in registering the observer.
		}, Channel2);
		TestTrue(TEXT("Adding observers should return valid handles"), ULES_EventSystem::IsHandleValid(Handle));
		TestTrue(TEXT("Should contain the handle"), EventSystem->ContainsValidHandle(Handle));
	}
	{
		const auto Handle = EventSystem->BP_AddObserver_Event(ULES_TestEvent::StaticClass(), TestObserver, {});
		TestTrue(TEXT("Adding observers should return valid handles"), ULES_EventSystem::IsHandleValid(Handle));
		TestTrue(TEXT("Should contain the handle"), EventSystem->ContainsValidHandle(Handle));
	}
	{
		const auto Handle = EventSystem->BP_AddObserver_Function(ULES_TestEvent::StaticClass(),
		                                                         TestObserver,
		                                                         "OnTestEvent");
		TestTrue(TEXT("Adding observers should return valid handles"), ULES_EventSystem::IsHandleValid(Handle));
		TestTrue(TEXT("Should contain the handle"), EventSystem->ContainsValidHandle(Handle));
	}
	TestTrue(TEXT("Should contain the observer after registration"), EventSystem->ContainsObserver(TestObserver));

	TArray<FName> Channels;
	EventSystem->GetChannels(Channels);
	TestEqual(TEXT("Should contain records for 3 channels in total"), Channels.Num(), 3);
	TestTrue(TEXT("Should contain the default channel"), Channels.Contains(NAME_None));
	TestTrue(TEXT("Should contain Channel1"), Channels.Contains(Channel1));
	TestTrue(TEXT("Should contain Channel2"), Channels.Contains(Channel2));
	TestFalse(TEXT("Shoudn't contain an unused channel"), Channels.Contains("An unused channel"));

	// Adding nullptr observers
	{
		const auto Handle = EventSystem->AddObserver<ULES_TestEvent>(NullObserver, &ULES_TestObserver::OnTestEvent);
		TestFalse(TEXT("Registering nullptr observers should return invalid handles"),
		          ULES_EventSystem::IsHandleValid(Handle));
		TestFalse(TEXT("Shouldn't contain the handle"), EventSystem->ContainsValidHandle(Handle));
	}
	{
		const auto Handle = EventSystem->AddObserver<ULES_TestEvent>(NullObserver, [](const ULES_TestEvent*)
		{
			// Nothing needs to be done in the handler, we're interested only in registering the observer.
		});
		TestFalse(TEXT("Registering nullptr observers should return invalid handles"),
		          ULES_EventSystem::IsHandleValid(Handle));
		TestFalse(TEXT("Shouldn't contain the handle"), EventSystem->ContainsValidHandle(Handle));
	}
	{
		const auto Handle = EventSystem->BP_AddObserver_Event(nullptr, NullObserver, {});
		TestFalse(TEXT("Registering nullptr observers should return invalid handles"),
		          ULES_EventSystem::IsHandleValid(Handle));
		TestFalse(TEXT("Shouldn't contain the handle"), EventSystem->ContainsValidHandle(Handle));
	}
	{
		const auto Handle = EventSystem->BP_AddObserver_Function(nullptr, NullObserver, "OnTestEvent");
		TestFalse(TEXT("Registering nullptr observers should return invalid handles"),
		          ULES_EventSystem::IsHandleValid(Handle));
		TestFalse(TEXT("Shouldn't contain the handle"), EventSystem->ContainsValidHandle(Handle));
	}

	// Adding observers by function name (but that function doesn't exist).
	{
		const auto Handle = EventSystem->BP_AddObserver_Function(nullptr, NullObserver, "ThisFunctionDoesNotExist");
		TestFalse(TEXT("Registering non-existent event handlers by name should return invalid handles"),
		          ULES_EventSystem::IsHandleValid(Handle));
		TestFalse(TEXT("Shouldn't contain the handle"), EventSystem->ContainsValidHandle(Handle));
	}

	TestEqual(TEXT("Should contain 4 observer records"), EventSystem->Num(), 4);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLES_RemovingObserversTest, "Light Event System.Removing observers",
                                 EAutomationTestFlags::ApplicationContextMask |
                                 EAutomationTestFlags::HighPriority |
                                 EAutomationTestFlags::ProductFilter)

bool FLES_RemovingObserversTest::RunTest(const FString& Parameters)
{
	ULES_EventSystem* EventSystem = NewObject<ULES_EventSystem>();
	ULES_TestObserver* TestObserver = NewObject<ULES_TestObserver>();
	ULES_TestObserver* TestObserver2 = NewObject<ULES_TestObserver>();

	const auto Handle1 = EventSystem->AddObserver<ULES_TestEvent>(TestObserver, &ULES_TestObserver::OnTestEvent);
	const auto Handle2 = EventSystem->AddObserver<ULES_TestEvent>(TestObserver, [](const ULES_TestEvent*)
	{
		// Nothing needs to be done in the handler, we're interested only in registering the observer.
	});
	const auto Handle3 = EventSystem->AddObserver<ULES_TestEvent>(TestObserver2, &ULES_TestObserver::OnTestEvent);
	const auto Handle4 = EventSystem->AddObserver<ULES_TestEvent>(TestObserver2, &ULES_TestObserver::OnTestEvent);
	int Count = EventSystem->RemoveByObserver(TestObserver2);
	TestTrue(TEXT("Handle1 should be valid"), ULES_EventSystem::IsHandleValid(Handle1));
	TestTrue(TEXT("Handle2 should be valid"), ULES_EventSystem::IsHandleValid(Handle2));
	TestFalse(TEXT("Handle3 should be invalid"), ULES_EventSystem::IsHandleValid(Handle3));
	TestFalse(TEXT("Handle4 should be invalid"), ULES_EventSystem::IsHandleValid(Handle4));
	TestEqual(TEXT("Removed 2 record in total"), Count, 2);

	Count = EventSystem->RemoveByHandle(Handle2);
	TestTrue(TEXT("Handle1 should be valid"), ULES_EventSystem::IsHandleValid(Handle1));
	TestFalse(TEXT("Handle2 should be invalid"), ULES_EventSystem::IsHandleValid(Handle2));
	TestEqual(TEXT("Removed 1 record in total"), Count, 1);

	EventSystem->RemoveAll();
	TestFalse(TEXT("Handle1 should be invalid"), ULES_EventSystem::IsHandleValid(Handle1));
	TestEqual(TEXT("Should be empty"), EventSystem->Num(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLES_SendingEventsTest, "Light Event System.Sending events",
                                 EAutomationTestFlags::ApplicationContextMask |
                                 EAutomationTestFlags::HighPriority |
                                 EAutomationTestFlags::ProductFilter)

bool FLES_SendingEventsTest::RunTest(const FString& Parameters)
{
	ULES_EventSystem* EventSystem = NewObject<ULES_EventSystem>();

	constexpr int NumObservers = 4;
	TArray<ULES_TestObserver*> Observers;
	for (int i = 0; i < NumObservers; i++)
		Observers.Add(NewObject<ULES_TestObserver>());

	EventSystem->AddObserver<ULES_TestEvent>(Observers[0], &ULES_TestObserver::OnTestEvent);
	EventSystem->AddObserver<ULES_TestEvent>(Observers[0], [Observer = Observers[0]](const ULES_TestEvent*)
	{
		Observer->Counter += FIntVector3(0, 0, 1);
	});

	const FName FirstChannel = "Some channel";
	EventSystem->AddObserver<ULES_TestEvent>(Observers[1], &ULES_TestObserver::OnTestEvent);
	EventSystem->AddObserver<ULES_TestEvent>(Observers[2], &ULES_TestObserver::OnTestEvent, FirstChannel);

	const FName SecondChannel = "Yet another channel";
	EventSystem->AddObserver<ULES_TestEvent>(Observers[3], &ULES_TestObserver::OnTestEvent, SecondChannel);
	EventSystem->AddObserver<ULES_DerivedEvent>(Observers[3], &ULES_TestObserver::OnDerivedEvent, SecondChannel);
	EventSystem->AddObserver<ULES_OtherTestEvent>(Observers[3], &ULES_TestObserver::OnOtherTestEvent);

	FLES_EventHandler EventHandler;
	EventHandler.BindDynamic(Observers[3], &ULES_TestObserver::OnEvent);
	EventSystem->BP_AddObserver_Event(ULES_TestEvent::StaticClass(), Observers[3], EventHandler);
	EventSystem->BP_AddObserver_Function(ULES_TestEvent::StaticClass(), Observers[3], "OnTestEvent");

	ULES_DerivedEvent* DerivedEvent = NewObject<ULES_DerivedEvent>();
	DerivedEvent->Channel = SecondChannel;
	EventSystem->SendEvent(DerivedEvent);
	EventSystem->SendEvent(NewObject<ULES_TestEvent>());
	EventSystem->SendEvent(NewObject<ULES_OtherTestEvent>());
	TestEqual(TEXT("Observers[0] should run 2 event handlers"), Observers[0]->Counter, FIntVector3(1, 0, 1));
	TestEqual(TEXT("Observers[1] should run 1 event handler"), Observers[1]->Counter, FIntVector3(1, 0, 0));
	TestEqual(TEXT("Observers[2] should receive no events"), Observers[2]->Counter, FIntVector3::ZeroValue);
	TestEqual(TEXT("Observers[3] should run 3 event handler"), Observers[3]->Counter, FIntVector3(2, 1, 1));

	return true;
}
