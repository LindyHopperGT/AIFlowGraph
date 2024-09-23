// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Blackboard/FlowBlackboardEntryValue_Name.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Name.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIFlowLogChannels.h"
#include "Nodes/FlowNode.h"
#include "Types/FlowDataPinProperties.h"
#include "Types/FlowDataPinResults.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowBlackboardEntryValue_Name)

#if WITH_EDITOR
void UFlowBlackboardEntryValue_Name::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ArchetypeObject | RF_ClassDefaultObject))
	{
		if (Key.AllowedTypes.IsEmpty())
		{
			Key.AllowedTypes.Add(NewObject<UBlackboardKeyType_Name>(this));
		}
	}
}

FString UFlowBlackboardEntryValue_Name::GetEditorValueString() const
{
	return *NameValue.ToString();
}

FText UFlowBlackboardEntryValue_Name::BuildNodeConfigText() const
{
	return FText::FromString(FString::Printf(TEXT("Set %s to \"%s\""), *Key.GetKeyName().ToString(), *GetEditorValueString()));
}
#endif // WITH_EDITOR

bool UFlowBlackboardEntryValue_Name::TryProvideFlowDataPinProperty(const bool bIsInputPin, TInstancedStruct<FFlowDataPinProperty>& OutFlowDataPinProperty) const
{
	if (bIsInputPin)
	{
		OutFlowDataPinProperty.InitializeAs<FFlowDataPinInputProperty_Name>(NameValue);
	}
	else
	{
		OutFlowDataPinProperty.InitializeAs<FFlowDataPinOutputProperty_Name>(NameValue);
	}

	return true;
}

bool UFlowBlackboardEntryValue_Name::TrySetValueFromInputDataPin(const FName& PinName, UFlowNode& PinOwnerFlowNode)
{
	const FFlowDataPinResult_Name FlowDataPinResult = PinOwnerFlowNode.TryResolveDataPinAsName(PinName);

	if (FlowDataPinResult.Result == EFlowDataPinResolveResult::Success)
	{
		NameValue = FlowDataPinResult.Value;

		return true;
	}

	return false;
}

void UFlowBlackboardEntryValue_Name::SetOnBlackboardComponent(UBlackboardComponent* BlackboardComponent) const
{
	if (IsValid(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsName(Key.GetKeyName(), NameValue);
	}
}

EBlackboardCompare::Type UFlowBlackboardEntryValue_Name::CompareKeyValues(const UBlackboardComponent* BlackboardComponent, const FName& OtherKeyName) const
{
	if (!IsValid(BlackboardComponent))
	{
		UE_LOG(LogAIFlow, Error, TEXT("Cannot CompareKeyValues without a Blackboard!"));
		return EBlackboardCompare::NotEqual;
	}

	const FName OtherValueAsName = BlackboardComponent->GetValueAsName(OtherKeyName);

	if (NameValue == OtherValueAsName)
	{
		return EBlackboardCompare::Equal;
	}
	else
	{
		return EBlackboardCompare::NotEqual;
	}
}

TSubclassOf<UBlackboardKeyType> UFlowBlackboardEntryValue_Name::GetSupportedBlackboardKeyType() const
{
	return UBlackboardKeyType_Name::StaticClass();
}
