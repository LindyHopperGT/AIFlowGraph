// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Blackboard/FlowBlackboardEntryValue_Int.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIFlowLogChannels.h"
#include "Nodes/FlowNode.h"
#include "Types/FlowDataPinProperties.h"
#include "Types/FlowDataPinResults.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowBlackboardEntryValue_Int)

#if WITH_EDITOR
void UFlowBlackboardEntryValue_Int::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ArchetypeObject | RF_ClassDefaultObject))
	{
		if (Key.AllowedTypes.IsEmpty())
		{
			Key.AllowedTypes.Add(NewObject<UBlackboardKeyType_Int>(this));
		}
	}
}

FString UFlowBlackboardEntryValue_Int::GetEditorValueString() const
{
	return FString::FromInt(IntValue);
}

FText UFlowBlackboardEntryValue_Int::BuildNodeConfigText() const
{
	return FText::FromString(FString::Printf(TEXT("Set %s to \"%d\""), *Key.GetKeyName().ToString(), IntValue));
}
#endif // WITH_EDITOR

bool UFlowBlackboardEntryValue_Int::TryProvideFlowDataPinProperty(const bool bIsInputPin, TInstancedStruct<FFlowDataPinProperty>& OutFlowDataPinProperty) const
{
	if (bIsInputPin)
	{
		OutFlowDataPinProperty.InitializeAs<FFlowDataPinInputProperty_Int32>(IntValue);
	}
	else
	{
		OutFlowDataPinProperty.InitializeAs<FFlowDataPinOutputProperty_Int32>(IntValue);
	}

	return true;
}

bool UFlowBlackboardEntryValue_Int::TrySetValueFromInputDataPin(const FName& PinName, UFlowNode& PinOwnerFlowNode)
{
	const FFlowDataPinResult_Int FlowDataPinResult = PinOwnerFlowNode.TryResolveDataPinAsInt(PinName);

	if (FlowDataPinResult.Result == EFlowDataPinResolveResult::Success)
	{
		IntValue = static_cast<int32>(FlowDataPinResult.Value);

		return true;
	}

	return false;
}

void UFlowBlackboardEntryValue_Int::SetOnBlackboardComponent(UBlackboardComponent* BlackboardComponent) const
{
	if (IsValid(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsInt(Key.GetKeyName(), IntValue);
	}
}

EBlackboardCompare::Type UFlowBlackboardEntryValue_Int::CompareKeyValues(const UBlackboardComponent* BlackboardComponent, const FName& OtherKeyName) const
{
	if (!IsValid(BlackboardComponent))
	{
		UE_LOG(LogAIFlow, Error, TEXT("Cannot CompareKeyValues without a Blackboard!"));
		return EBlackboardCompare::NotEqual;
	}

	const int32 OtherValueAsInt = BlackboardComponent->GetValueAsInt(OtherKeyName);

	if (IntValue == OtherValueAsInt)
	{
		return EBlackboardCompare::Equal;
	}
	else
	{
		return EBlackboardCompare::NotEqual;
	}
}

TSubclassOf<UBlackboardKeyType> UFlowBlackboardEntryValue_Int::GetSupportedBlackboardKeyType() const
{
	return UBlackboardKeyType_Int::StaticClass();
}

bool UFlowBlackboardEntryValue_Int::TryGetNumericalValuesForArithmeticOperation(int32* OutIntValue, float* OutFloatValue) const
{
	if (OutIntValue)
	{
		*OutIntValue = IntValue;
	}

	if (OutFloatValue)
	{
		*OutFloatValue = static_cast<float>(IntValue);
	}

	return true;
}
