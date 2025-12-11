// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Nodes/FlowNode_SetBlackboardValues.h"
#include "Blackboard/FlowBlackboardEntryValue.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Types/FlowAutoDataPinsWorkingData.h"
#include "Types/FlowInjectComponentsManager.h"
#include "Types/FlowDataPinValue.h"
#include "AIFlowAsset.h"
#include "AIFlowTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowNode_SetBlackboardValues)

UFlowNode_SetBlackboardValues::UFlowNode_SetBlackboardValues()
	: Super()
{
#if WITH_EDITOR
	NodeDisplayStyle = FlowNodeStyle::Deprecated;
	Category = TEXT("Deprecated");
	bNodeDeprecated = true;
#endif
}

void UFlowNode_SetBlackboardValues::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);

	// Refresh EntriesForEveryActor from data pin input values
	for (UFlowBlackboardEntryValue* BlackboardEntry : EntriesForEveryActor.Entries)
	{
		if (!IsValid(BlackboardEntry))
		{
			LogError(TEXT("Found null BlackboardEntry.  This is unexpected."), EFlowOnScreenMessageType::Temporary);

			continue;
		}

		(void) BlackboardEntry->TrySetValueFromInputDataPin(BlackboardEntry->Key.KeyName, *this);
	}

	// Create the InjectComponentsManager sub-object if necessary 
	// (to track created components and ensure they are cleaned up)
	const bool bMayInjectComponent = EActorBlackboardInjectRule_Classifiers::NeedsInjectComponentsManager(InjectRule);
	if (bMayInjectComponent)
	{
		EnsureInjectComponentsManager();
	}

	// Get the Per-Actor Options from a subclass (if any)
	const TArray<FAIFlowConfigureBlackboardOption>* PerActorOptions = nullptr;
	EPerActorOptionsAssignmentMethod PerActorOptionsAssignmentMethod = EPerActorOptionsAssignmentMethod::Invalid;
	(void) TryGetPerActorOptions(PerActorOptions, PerActorOptionsAssignmentMethod);

	// Apply the values to the blackboards
	const TArray<UBlackboardComponent*> BlackboardComponents = GetBlackboardComponentsToApplyTo();
	if (!BlackboardComponents.IsEmpty())
	{
		for (UBlackboardComponent* BlackboardComponent : BlackboardComponents)
		{
			if (IsValid(BlackboardComponent))
			{
				ActorBlackboardHelper.ApplyBlackboardOptionsToBlackboardComponent(
					*BlackboardComponent,
					PerActorOptionsAssignmentMethod,
					EntriesForEveryActor,
					PerActorOptions);
			}
		}
	}
	else
	{
		LogError(TEXT("Cannot SetBlackboardValues without a Blackboard"));
	}

	constexpr bool bIsFinished = true;
	TriggerFirstOutput(bIsFinished);
}

void UFlowNode_SetBlackboardValues::DeinitializeInstance()
{
	CleanupInjectComponentsManager();

	Super::DeinitializeInstance();
}

void UFlowNode_SetBlackboardValues::EnsureInjectComponentsManager()
{
	if (IsValid(InjectComponentsManager))
	{
		return;
	}

	InjectComponentsManager = NewObject<UFlowInjectComponentsManager>(this);

	InjectComponentsManager->InitializeRuntime();
	InjectComponentsManager->BeforeActorRemovedDelegate.AddDynamic(this, &ThisClass::OnBeforeActorRemoved);
}

void UFlowNode_SetBlackboardValues::CleanupInjectComponentsManager()
{
	if (IsValid(InjectComponentsManager))
	{
		InjectComponentsManager->ShutdownRuntime();
		InjectComponentsManager->BeforeActorRemovedDelegate.RemoveDynamic(this, &ThisClass::OnBeforeActorRemoved);
	}

	InjectComponentsManager = nullptr;
}

void UFlowNode_SetBlackboardValues::OnBeforeActorRemoved(AActor* RemovedActor)
{
	if (IsValid(RemovedActor))
	{
		OnStopMonitoringActor(*RemovedActor);
	}
}

TArray<AActor*> UFlowNode_SetBlackboardValues::TryResolveActorsForBlackboard() const
{
	// Default to the Flow graph's actor.  
	// (Not very satisfactory, because the UFlowNode_SetBlackboardValues node does this simpler,
	// but we expect subclasses to replace this function).

	TArray<AActor*> ResolvedActors;
	AActor* FoundActor = GetFlowAsset()->TryFindActorOwner();
	if (IsValid(FoundActor))
	{
		ResolvedActors.Reserve(1);
		ResolvedActors.Add(FoundActor);
	}

	return ResolvedActors;
}

TArray<UBlackboardComponent*> UFlowNode_SetBlackboardValues::GetBlackboardComponentsToApplyTo() const
{
	// Resolve which BlackboardComponentClass to use (if we need inject the blackboard)
	TSubclassOf<UBlackboardComponent> BlackboardComponentClass = SpecificBlackboardComponentClass;
	if (!IsValid(BlackboardComponentClass))
	{
		// If no specific one provided, the BlackboardComponentClass to use from the FlowAsset
		UAIFlowAsset* AIFlowAsset = Cast<UAIFlowAsset>(GetFlowAsset());
		if (IsValid(AIFlowAsset))
		{
			BlackboardComponentClass = AIFlowAsset->GetBlackboardComponentClass();
		}
		else
		{
			BlackboardComponentClass = UBlackboardComponent::StaticClass();
		}
	}

	// Find (or Inject) the desired blackboard components
	const TArray<AActor*> ResolvedActors = TryResolveActorsForBlackboard();
	TArray<UBlackboardComponent*> BlackboardComponents =
		FAIFlowActorBlackboardHelper::FindOrAddBlackboardComponentOnActors(
			ResolvedActors,
			InjectComponentsManager,
			BlackboardComponentClass,
			SpecificBlackboardAsset,
			SpecificBlackboardSearchRule,
			InjectRule);

	return BlackboardComponents;
}

bool UFlowNode_SetBlackboardValues::TryFindPropertyByPinName(
	const UObject& PropertyOwnerObject,
	const FName& PinName,
	const FProperty*& OutFoundProperty,
	TInstancedStruct<FFlowDataPinValue>& OutFoundInstancedStruct) const
{
	// The SetBlackboardValues node stores its properties in instanced structs in an array, so look there first

	for (const UFlowBlackboardEntryValue* BlackboardEntry : EntriesForEveryActor.Entries)
	{
		if (!IsValid(BlackboardEntry))
		{
			LogError(TEXT("Found null BlackboardEntry.  This is unexpected."), EFlowOnScreenMessageType::Temporary);

			continue;
		}

		if (BlackboardEntry->Key.KeyName == PinName)
		{
			const IFlowDataPinPropertyProviderInterface* FlowDataPinProviderInterface = CastChecked<IFlowDataPinPropertyProviderInterface>(BlackboardEntry);

			if (FlowDataPinProviderInterface->TryProvideFlowDataPinProperty(OutFoundInstancedStruct))
			{
				return true;
			}
			else
			{
				// Found the property name, but it could not provide the data for the data pin property
				break;
			}
		}
	}

	return Super::TryFindPropertyByPinName(PropertyOwnerObject, PinName, OutFoundProperty, OutFoundInstancedStruct);
}

#if WITH_EDITOR
void UFlowNode_SetBlackboardValues::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChainEvent)
{
	Super::PostEditChangeChainProperty(PropertyChainEvent);

	if (PropertyChainEvent.PropertyChain.Num() == 0)
	{
		return;
	}

	auto& Property = PropertyChainEvent.PropertyChain.GetActiveMemberNode()->GetValue();

	constexpr EPropertyChangeType::Type RelevantChangeTypesForReconstructionMask =
		EPropertyChangeType::Unspecified |
		EPropertyChangeType::ArrayAdd |
		EPropertyChangeType::ArrayRemove |
		EPropertyChangeType::ArrayClear |
		EPropertyChangeType::ValueSet |
		EPropertyChangeType::Redirected |
		EPropertyChangeType::ArrayMove;

	const uint32 PropertyChangedTypeFlags = (PropertyChainEvent.ChangeType & RelevantChangeTypesForReconstructionMask);
	const bool bIsRelevantChangeTypeForReconstruction = PropertyChangedTypeFlags != 0;
	const bool bChangedInputProperties = Property->GetFName() == GET_MEMBER_NAME_CHECKED(UFlowNode_SetBlackboardValues, EntriesForEveryActor);
	if (bIsRelevantChangeTypeForReconstruction && bChangedInputProperties)
	{
		OnReconstructionRequested.ExecuteIfBound();
	}
}

UBlackboardData* UFlowNode_SetBlackboardValues::GetBlackboardAssetForEditor() const
{
	if (IsValid(SpecificBlackboardAsset))
	{
		return SpecificBlackboardAsset;
	}

	return GetBlackboardAsset();
}

UBlackboardData* UFlowNode_SetBlackboardValues::GetBlackboardAssetForPropertyHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle) const
{
	if (UBlackboardData* BlackboardAssetForEditor = GetBlackboardAssetForEditor())
	{
		return BlackboardAssetForEditor;
	}
	
	return Super::GetBlackboardAssetForPropertyHandle(PropertyHandle);
}

void UFlowNode_SetBlackboardValues::AutoGenerateDataPins(FFlowAutoDataPinsWorkingData& InOutWorkingData) const
{
	Super::AutoGenerateDataPins(InOutWorkingData);

	for (const UFlowBlackboardEntryValue* BlackboardEntry : EntriesForEveryActor.Entries)
	{
		if (!IsValid(BlackboardEntry))
		{
			LogError(TEXT("Found null BlackboardEntry.  This is unexpected."), EFlowOnScreenMessageType::Temporary);

			continue;
		}

		const FName& PinName = BlackboardEntry->Key.KeyName;
		if (PinName.IsNone())
		{
			continue;
		}

		const IFlowDataPinPropertyProviderInterface* FlowDataPinProviderInterface = CastChecked<IFlowDataPinPropertyProviderInterface>(BlackboardEntry);
		TInstancedStruct<FFlowDataPinValue> InstancedFlowDataPinProperty;

		if (FlowDataPinProviderInterface->TryProvideFlowDataPinProperty(InstancedFlowDataPinProperty))
		{
			const FFlowDataPinValue& FlowDataPinValuePtr = InstancedFlowDataPinProperty.Get<FFlowDataPinValue>();
			if (const FFlowPinType* FlowPinType = FFlowPinType::LookupPinType(FlowDataPinValuePtr.GetPinTypeName()))
			{
				FFlowPin NewFlowPin = FlowPinType->CreateFlowPinFromValueWrapper(PinName, FlowDataPinValuePtr);

				// The FFlowDataPinProperty's for this node are all Input pins
				InOutWorkingData.AutoInputDataPinsNext.AddUnique(NewFlowPin);
			}
			else
			{
				LogError(FString::Printf(TEXT("Could not auto-generate pin %s: Could not find pin type %s."), *PinName.ToString(), *FlowDataPinValuePtr.GetPinTypeName().ToString()), EFlowOnScreenMessageType::Temporary);
			}
		}
	}
}

void UFlowNode_SetBlackboardValues::AppendEntriesForEveryActor(FTextBuilder& InOutTextBuilder) const
{
	for (const UFlowBlackboardEntryValue* Entry : EntriesForEveryActor.Entries)
	{
		if (IsValid(Entry) && !IsInputConnected(Entry->Key.KeyName))
		{
			InOutTextBuilder.AppendLine(Entry->BuildNodeConfigText());
		}
	}
}

#endif // WITH_EDITOR

void UFlowNode_SetBlackboardValues::UpdateNodeConfigText_Implementation()
{
#if WITH_EDITOR
	FTextBuilder TextBuilder;

	AppendEntriesForEveryActor(TextBuilder);

	SetNodeConfigText(TextBuilder.ToText());
#endif // WITH_EDITOR
}