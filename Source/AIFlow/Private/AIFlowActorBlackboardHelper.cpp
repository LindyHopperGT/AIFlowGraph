// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "AIFlowActorBlackboardHelper.h"
#include "AIFlowAsset.h"
#include "AIFlowLogChannels.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blackboard/FlowBlackboardEntryValue.h"
#include "Types/FlowArray.h"
#include "Types/FlowInjectComponentsManager.h"
#include "Types/FlowInjectComponentsHelper.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/GameState.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AIFlowActorBlackboardHelper)

UBlackboardComponent* FAIFlowActorBlackboardHelper::FindOrAddBlackboardComponentOnActor(
	AActor& Actor,
	UFlowInjectComponentsManager* InjectComponentsManager,
	TSubclassOf<UBlackboardComponent> BlackboardComponentClass,
	UBlackboardData* OptionalBlackboardData,
	EActorBlackboardSearchRule SearchRule,
	EActorBlackboardInjectRule InjectRule)
{
	FLOW_ASSERT_ENUM_MAX(EActorBlackboardSearchRule, 5);

	UBlackboardComponent* FoundBlackboardComponent = TryFindBlackboardComponent(*Actor.GetWorld(), SearchRule, &Actor, OptionalBlackboardData);

	if (EActorBlackboardInjectRule_Classifiers::NeedsInjectComponentsManager(InjectRule))
	{
		if (!IsValid(OptionalBlackboardData))
		{
			UE_LOG(LogAIFlow, Error, TEXT("Must specify OptionalBlackboardData if injecting missing a blackboard.  Without it, we don't know what blackboard data to use for the injected component!"));

			return nullptr;
		}

		if (!IsValid(InjectComponentsManager))
		{
			UE_LOG(LogAIFlow, Error, TEXT("Must provide an InjectComponentsManager if injecting missing a blackboard."));

			return nullptr;
		}

		if (!IsValid(BlackboardComponentClass))
		{
			UE_LOG(LogAIFlow, Error, TEXT("Must provide an BlackboardComponentClass if injecting missing a blackboard."));

			return nullptr;
		}
	}

	if (!FoundBlackboardComponent)
	{
		static_assert(static_cast<__underlying_type(EActorBlackboardInjectRule)>(EActorBlackboardInjectRule::Max) == 3, "This code may need updating if the enum values change");
		switch (InjectRule)
		{
		case EActorBlackboardInjectRule::InjectOntoActorIfMissing:
			{
				const FName InstanceBaseName = BlackboardComponentClass->GetFName();
				FoundBlackboardComponent = Cast<UBlackboardComponent>(FFlowInjectComponentsHelper::TryCreateComponentInstanceForActorFromClass(Actor, BlackboardComponentClass, InstanceBaseName));

				if (IsValid(FoundBlackboardComponent))
				{
					FoundBlackboardComponent->InitializeBlackboard(*OptionalBlackboardData);
				}
			}
			break;

		case EActorBlackboardInjectRule::InjectOntoControllerIfMissing:
			{
				APawn* ActorAsPawn = Cast<APawn>(&Actor);
				if (!ActorAsPawn)
				{
					UE_LOG(LogAIFlow, Error, TEXT("Cannot Inject missing blackboard onto Controller for a non-Pawn Actor."));

					break;
				}

				AController* Controller = ActorAsPawn->GetController();
				if (!IsValid(Controller))
				{
					UE_LOG(LogAIFlow, Error, TEXT("Cannot Inject missing blackboard onto Controller, for a Pawn without a Controller."));

					break;
				}

				const FName InstanceBaseName = BlackboardComponentClass->GetFName();
				FoundBlackboardComponent = Cast<UBlackboardComponent>(FFlowInjectComponentsHelper::TryCreateComponentInstanceForActorFromClass(*Controller, BlackboardComponentClass, InstanceBaseName));

				if (IsValid(FoundBlackboardComponent))
				{
					FoundBlackboardComponent->InitializeBlackboard(*OptionalBlackboardData);
				}
			}
			break;

		case EActorBlackboardInjectRule::DoNotInjectIfMissing:
			break;

		default: break;
		}
	}

	return FoundBlackboardComponent;
}

UBlackboardComponent* FAIFlowActorBlackboardHelper::TryFindBlackboardComponent(UWorld& World, EActorBlackboardSearchRule SearchRule, AActor* OptionalActor, UBlackboardData* OptionalBlackboardData)
{
	UBlackboardComponent* FoundBlackboardComponent = nullptr;

	if (IsValid(OptionalActor))
	{
		const bool bCanSearchActor = EActorBlackboardSearchRule_Classifiers::CanSearchActor(SearchRule);
		if (bCanSearchActor)
		{
			FoundBlackboardComponent = UAIFlowAsset::TryFindBlackboardComponentOnActor(*OptionalActor, OptionalBlackboardData);

			if (IsValid(FoundBlackboardComponent))
			{
				return FoundBlackboardComponent;
			}
		}

		const bool bCanSearchAIController = EActorBlackboardSearchRule_Classifiers::CanSearchController(SearchRule);
		if (bCanSearchAIController)
		{
			if (APawn* Pawn = Cast<APawn>(OptionalActor))
			{
				AController* Controller = Pawn->GetController();
				if (IsValid(Controller))
				{
					FoundBlackboardComponent = UAIFlowAsset::TryFindBlackboardComponentOnActor(*OptionalActor, OptionalBlackboardData);

					if (IsValid(FoundBlackboardComponent))
					{
						return FoundBlackboardComponent;
					}
				}
			}
		}
	}

	const bool bCanSearchGameState = EActorBlackboardSearchRule_Classifiers::CanSearchGameState(SearchRule);
	if (bCanSearchGameState)
	{
		AGameStateBase* GameState = World.GetGameState();
		if (IsValid(GameState))
		{
			FoundBlackboardComponent = UAIFlowAsset::TryFindBlackboardComponentOnActor(*GameState, OptionalBlackboardData);

			if (IsValid(FoundBlackboardComponent))
			{
				return FoundBlackboardComponent;
			}
		}
	}

	return nullptr;
}

void FAIFlowActorBlackboardHelper::ApplyBlackboardEntries(UBlackboardComponent& BlackboardComponent, const TArray<UFlowBlackboardEntryValue*>& EntriesToApply)
{
	for (const UFlowBlackboardEntryValue* Entry : EntriesToApply)
	{
		if (IsValid(Entry))
		{
			Entry->SetOnBlackboardComponent(&BlackboardComponent);
		}
	}
}

void FAIFlowActorBlackboardHelper::ApplyBlackboardOptionsToBlackboardComponent(
	UBlackboardComponent& BlackboardComponent,
	EPerActorOptionsAssignmentMethod ApplicationMethod,
	const FAIFlowConfigureBlackboardOption& EntriesForEveryActor,
	const TArray<FAIFlowConfigureBlackboardOption>* PerActorOptions)
{
	if (!EntriesForEveryActor.Entries.IsEmpty())
	{
		ApplyBlackboardEntries(BlackboardComponent, EntriesForEveryActor.Entries);
	}

	if (PerActorOptions && !PerActorOptions->IsEmpty())
	{
		const int32 PerActorOptionIndex = ChooseNextBlackboardOptionIndex(ApplicationMethod, (*PerActorOptions));

		if (PerActorOptionIndex != INDEX_NONE)
		{
			const FAIFlowConfigureBlackboardOption& Option = (*PerActorOptions)[PerActorOptionIndex];

			ApplyBlackboardEntries(BlackboardComponent, Option.Entries);
		}
	}
}

TArray<UBlackboardComponent*> FAIFlowActorBlackboardHelper::FindOrAddBlackboardComponentOnActors(
	const TArray<AActor*>& Actors,
	UFlowInjectComponentsManager* InjectComponentsManager,
	TSubclassOf<UBlackboardComponent> BlackboardComponentClass,
	UBlackboardData* OptionalBlackboardData,
	EActorBlackboardSearchRule SearchRule,
	EActorBlackboardInjectRule InjectRule)
{
	TArray<UBlackboardComponent*> BlackboardComponents;

	BlackboardComponents.Reserve(Actors.Num());

	for (AActor* Actor : Actors)
	{
		if (!IsValid(Actor))
		{
			UE_LOG(LogAIFlow, Error, TEXT("Null Actor supplied to FindOrAddBlackboardComponentOnActors.  Filter out the nulls before calling this function."));

			continue;
		}

		UBlackboardComponent* FoundBlackboardComponent =
			FindOrAddBlackboardComponentOnActor(
				*Actor,
				InjectComponentsManager,
				BlackboardComponentClass,
				OptionalBlackboardData,
				SearchRule,
				InjectRule);

		if (FoundBlackboardComponent)
		{
			BlackboardComponents.Add(FoundBlackboardComponent);
		}
	}

	return BlackboardComponents;
}

int32 FAIFlowActorBlackboardHelper::ChooseNextBlackboardOptionIndex(
	EPerActorOptionsAssignmentMethod ApplicationMethod, 
	const TArray<FAIFlowConfigureBlackboardOption>& PerActorOptions)
{
	static_assert(static_cast<uint8>(EPerActorOptionsAssignmentMethod::Max) == 3, "update this code when changing enum");

	if (PerActorOptions.IsEmpty())
	{
		return INDEX_NONE;
	}

	EnsureOrderedOptionIndices(PerActorOptions.Num());

	const int32 OrderedOptionIndicesLast = OrderedOptionIndices.Num() - 1;

	if (OrderedOptionIndex < 0 || OrderedOptionIndex >= OrderedOptionIndicesLast)
	{
		switch (ApplicationMethod)
		{
		case EPerActorOptionsAssignmentMethod::ShuffledWithWrapping:
			{
				if (OrderedOptionIndex == INDEX_NONE)
				{
					FRandomStream RandomStream(FMath::RandHelper(INT_MAX));
					FlowArray::ShuffleArray(OrderedOptionIndices, RandomStream);
				}
			}
			break;

		case EPerActorOptionsAssignmentMethod::ShuffledWithReshuffling:
			{
				FRandomStream RandomStream(FMath::RandHelper(INT_MAX));
				FlowArray::ShuffleArray(OrderedOptionIndices, RandomStream);
			}
			break;

		case EPerActorOptionsAssignmentMethod::InOrderWithWrapping:
			break;

		default: break;
		}

		OrderedOptionIndex = 0;
	}
	else
	{
		++OrderedOptionIndex;
	}

	// Choose the index in the PerActorOptions array from the current entry the OrderedOptionIndices array.
	const int32 NextOptionIndex = OrderedOptionIndices[OrderedOptionIndex];

	return NextOptionIndex;
}

void FAIFlowActorBlackboardHelper::EnsureOrderedOptionIndices(int32 OptionNum)
{
	if (OrderedOptionIndices.IsEmpty())
	{
		OrderedOptionIndices.Reserve(OptionNum);

		for (int32 Index = 0; Index < OptionNum; ++Index)
		{
			OrderedOptionIndices.Add(Index);
		}

		OrderedOptionIndex = INDEX_NONE;
	}
}

#if WITH_EDITOR
void FAIFlowActorBlackboardHelper::AppendBlackboardOptions(
	const TArray<FAIFlowConfigureBlackboardOption>& PerActorOptions,
	FTextBuilder& InOutTextBuilder)
{
	for (int32 Index = 0; Index < PerActorOptions.Num(); ++Index)
	{
		const FAIFlowConfigureBlackboardOption& Option = PerActorOptions[Index];
		InOutTextBuilder.AppendLine(FString::Printf(TEXT("Config Option #%d:"), Index));

		for (const UFlowBlackboardEntryValue* PerActorEntry : Option.Entries)
		{
			if (IsValid(PerActorEntry))
			{
				InOutTextBuilder.AppendLine(FString(TEXT("  ")) + PerActorEntry->BuildNodeConfigText().ToString());
			}
		}
	}
}
#endif // WITH_EDITOR

EFlowDataPinResolveResult FAIFlowActorBlackboardHelper::TryProvideFlowDataPinPropertyFromBlackboardEntry(
	const FName& BlackboardKeyName,
	const UBlackboardKeyType* BlackboardKeyType,
	UBlackboardComponent* OptionalBlackboardComponent,
	TInstancedStruct<FFlowDataPinValue>& OutFlowDataPinProperty)
{
	if (!BlackboardKeyType)
	{
		return EFlowDataPinResolveResult::FailedMismatchedType;
	}

	FBlackboard::FKey BlackboardKeyID = FBlackboard::InvalidKey;

	if (IsValid(OptionalBlackboardComponent))
	{
		BlackboardKeyID = OptionalBlackboardComponent->GetKeyID(BlackboardKeyName);

		if (BlackboardKeyID == FBlackboard::InvalidKey)
		{
			return EFlowDataPinResolveResult::FailedUnknownPin;
		}

		const TSubclassOf<UBlackboardKeyType> FoundKeyType = OptionalBlackboardComponent->GetKeyType(BlackboardKeyID);
		if (!BlackboardKeyType->IsA(FoundKeyType))
		{
			UE_LOG(
				LogAIFlow,
				Error,
				TEXT("Blackboard key %s, key type %s, is not the desired key type %s, on blackboard %s."),
				*BlackboardKeyName.ToString(),
				*FoundKeyType->GetName(),
				*BlackboardKeyType->GetName(),
				*OptionalBlackboardComponent->GetName());

			return EFlowDataPinResolveResult::FailedMismatchedType;
		}
	}

	// TODO (gtaylor) Ideally, we optimize this lookup at some point, perhaps with a map?

	// Slow lookup through all of the subclasses until we find one that can do the conversion.
	const TArray<TWeakObjectPtr<UClass>> Subclasses = UFlowBlackboardEntryValue::EnsureBlackboardEntryValueSubclassArray();

	for (TWeakObjectPtr<UClass> SubclassPtr : Subclasses)
	{
		UClass* Subclass = SubclassPtr.Get();
		if (!IsValid(Subclass))
		{
			continue;
		}

		const UFlowBlackboardEntryValue* TypedSubclassCDO = Cast<UFlowBlackboardEntryValue>(Subclass->GetDefaultObject());

		if (TypedSubclassCDO->TryProvideFlowDataPinPropertyFromBlackboardEntry(
			BlackboardKeyName,
			*BlackboardKeyType,
			OptionalBlackboardComponent,
			OutFlowDataPinProperty))
		{
			return EFlowDataPinResolveResult::Success;
		}
	}

	return EFlowDataPinResolveResult::FailedMismatchedType;
}

// FAIFlowCachedBlackboardReference ---

bool FAIFlowCachedBlackboardReference::TryCacheBlackboardReference(const UFlowNodeBase& FlowNodeBase, UBlackboardData* OptionalSpecificBlackboardData, EActorBlackboardSearchRule SpecificBlackboardSearchRule)
{
	if (::IsValid(OptionalSpecificBlackboardData))
	{
		// Try to find a specific blackboard that isn't the default blackboard for this FlowAsset
		AActor* OwnerActor = FlowNodeBase.TryGetRootFlowActorOwner();
		if (!::IsValid(OwnerActor))
		{
			return false;
		}

		constexpr UFlowInjectComponentsManager* InjectComponentsManager = nullptr;

		UBlackboardComponent* FoundBlackboardComponent =
			FAIFlowActorBlackboardHelper::FindOrAddBlackboardComponentOnActor(
				*OwnerActor,
				InjectComponentsManager,
				UBlackboardComponent::StaticClass(),
				OptionalSpecificBlackboardData,
				SpecificBlackboardSearchRule,
				EActorBlackboardInjectRule::DoNotInjectIfMissing);

		if (!::IsValid(FoundBlackboardComponent))
		{
			return false;
		}

		BlackboardComponent = FoundBlackboardComponent;
		BlackboardData = FoundBlackboardComponent->GetBlackboardAsset();

		return true;
	}

	// Use the default blackboard for this flow node
	const IFlowBlackboardInterface* FlowBlackboardInterface = Cast<IFlowBlackboardInterface>(&FlowNodeBase);
	if (!FlowBlackboardInterface)
	{
		return false;
	}

	UBlackboardComponent* FoundBlackboardComponent = FlowBlackboardInterface->GetBlackboardComponent();
	if (!::IsValid(FoundBlackboardComponent))
	{
		return false;
	}

	BlackboardComponent = FoundBlackboardComponent;
	BlackboardData = FoundBlackboardComponent->GetBlackboardAsset();

	return IsValid();
}

bool FAIFlowCachedBlackboardReference::IsValid() const
{
	return ::IsValid(BlackboardComponent) && ::IsValid(BlackboardData);
}
