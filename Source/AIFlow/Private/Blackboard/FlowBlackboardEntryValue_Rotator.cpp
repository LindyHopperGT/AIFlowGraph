// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Blackboard/FlowBlackboardEntryValue_Rotator.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Rotator.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIFlowLogChannels.h"
#include "Nodes/FlowNode.h"
#include "Types/FlowDataPinProperties.h"
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

bool UFlowBlackboardEntryValue_Rotator::TryProvideFlowDataPinProperty(const bool bIsInputPin, TInstancedStruct<FFlowDataPinProperty>& OutFlowDataPinProperty) const
{
	if (bIsInputPin)
	{
		OutFlowDataPinProperty.InitializeAs<FFlowDataPinInputProperty_Rotator>(RotatorValue);
	}
	else
	{
		OutFlowDataPinProperty.InitializeAs<FFlowDataPinInputProperty_Rotator>(RotatorValue);
	}

	return false;
}

bool UFlowBlackboardEntryValue_Rotator::TrySetValueFromInputDataPin(const FName& PinName, UFlowNode& PinOwnerFlowNode)
{
	const FFlowDataPinResult_Rotator FlowDataPinResult = PinOwnerFlowNode.TryResolveDataPinAsRotator(PinName);

	if (FlowDataPinResult.Result == EFlowDataPinResolveResult::Success)
	{
		RotatorValue = FlowDataPinResult.Value;

		return true;
	}

	return false;
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
