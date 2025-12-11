// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Blackboard/FlowBlackboardEntryValue_Rotator.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Rotator.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIFlowLogChannels.h"
#include "Nodes/FlowNode.h"
#include "Types/FlowDataPinValuesStandard.h"
#include "Types/FlowDataPinResults.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowBlackboardEntryValue_Rotator)

#if WITH_EDITOR
void UFlowBlackboardEntryValue_Rotator::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ArchetypeObject | RF_ClassDefaultObject))
	{
		if (Key.AllowedTypes.IsEmpty())
		{
			Key.AllowedTypes.Add(NewObject<UBlackboardKeyType_Rotator>(this));
		}
	}
}

FText UFlowBlackboardEntryValue_Rotator::BuildNodeConfigText() const
{
	return FText::FromString(FString::Printf(TEXT("Set %s to \"%s\""), *Key.GetKeyName().ToString(), *GetEditorValueString()));
}
#endif // WITH_EDITOR

bool UFlowBlackboardEntryValue_Rotator::TryProvideFlowDataPinProperty(TInstancedStruct<FFlowDataPinValue>& OutFlowDataPinProperty) const
{
	OutFlowDataPinProperty.InitializeAs<FFlowDataPinValue_Rotator>(RotatorValue);
	return false;
}

bool UFlowBlackboardEntryValue_Rotator::TryProvideFlowDataPinPropertyFromBlackboardEntry(
	const FName& BlackboardKeyName,
	const UBlackboardKeyType& BlackboardKeyType,
	UBlackboardComponent* OptionalBlackboardComponent,
	TInstancedStruct<FFlowDataPinValue>& OutFlowDataPinProperty) const
{
	return
		TryProvideFlowDataPinPropertyFromBlackboardEntryTemplate<UBlackboardKeyType_Rotator, FFlowDataPinValue_Rotator>(
			BlackboardKeyName,
			BlackboardKeyType,
			OptionalBlackboardComponent,
			OutFlowDataPinProperty);
}

bool UFlowBlackboardEntryValue_Rotator::TrySetValueFromInputDataPin(const FName& PinName, UFlowNode& PinOwnerFlowNode)
{
	const EFlowDataPinResolveResult ResolveResult = PinOwnerFlowNode.TryResolveDataPinValue<FFlowPinType_Rotator>(PinName, RotatorValue);
	return FlowPinType::IsSuccess(ResolveResult);
}

void UFlowBlackboardEntryValue_Rotator::SetOnBlackboardComponent(UBlackboardComponent* BlackboardComponent) const
{
	if (IsValid(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsRotator(Key.GetKeyName(), RotatorValue);
	}
}

EBlackboardCompare::Type UFlowBlackboardEntryValue_Rotator::CompareKeyValues(const UBlackboardComponent* BlackboardComponent, const FName& OtherKeyName) const
{
	if (!IsValid(BlackboardComponent))
	{
		UE_LOG(LogAIFlow, Error, TEXT("Cannot CompareKeyValues without a Blackboard!"));
		return EBlackboardCompare::NotEqual;
	}

	const FRotator OtherValueAsRotator = BlackboardComponent->GetValueAsRotator(OtherKeyName);

	if (RotatorValue.Equals(OtherValueAsRotator))
	{
		return EBlackboardCompare::Equal;
	}
	else
	{
		return EBlackboardCompare::NotEqual;
	}
}

TSubclassOf<UBlackboardKeyType> UFlowBlackboardEntryValue_Rotator::GetSupportedBlackboardKeyType() const
{
	return UBlackboardKeyType_Rotator::StaticClass();
}
