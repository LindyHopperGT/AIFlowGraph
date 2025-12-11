// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Blackboard/FlowBlackboardEntryValue_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIFlowLogChannels.h"
#include "Nodes/FlowNode.h"
#include "Types/FlowDataPinValuesStandard.h"
#include "Types/FlowDataPinResults.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowBlackboardEntryValue_Vector)

#if WITH_EDITOR
void UFlowBlackboardEntryValue_Vector::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ArchetypeObject | RF_ClassDefaultObject))
	{
		if (Key.AllowedTypes.IsEmpty())
		{
			Key.AllowedTypes.Add(NewObject<UBlackboardKeyType_Vector>(this));
		}
	}
}

FText UFlowBlackboardEntryValue_Vector::BuildNodeConfigText() const
{
	return FText::FromString(FString::Printf(TEXT("Set %s to \"%s\""), *Key.GetKeyName().ToString(), *GetEditorValueString()));
}
#endif // WITH_EDITOR

bool UFlowBlackboardEntryValue_Vector::TryProvideFlowDataPinProperty(TInstancedStruct<FFlowDataPinValue>& OutFlowDataPinProperty) const
{
	OutFlowDataPinProperty.InitializeAs<FFlowDataPinValue_Vector>(VectorValue);
	return true;
}

bool UFlowBlackboardEntryValue_Vector::TryProvideFlowDataPinPropertyFromBlackboardEntry(
	const FName& BlackboardKeyName,
	const UBlackboardKeyType& BlackboardKeyType,
	UBlackboardComponent* OptionalBlackboardComponent,
	TInstancedStruct<FFlowDataPinValue>& OutFlowDataPinProperty) const
{
	return
		TryProvideFlowDataPinPropertyFromBlackboardEntryTemplate<UBlackboardKeyType_Vector, FFlowDataPinValue_Vector>(
			BlackboardKeyName,
			BlackboardKeyType,
			OptionalBlackboardComponent,
			OutFlowDataPinProperty);
}

bool UFlowBlackboardEntryValue_Vector::TrySetValueFromInputDataPin(const FName& PinName, UFlowNode& PinOwnerFlowNode)
{
	const EFlowDataPinResolveResult ResolveResult = PinOwnerFlowNode.TryResolveDataPinValue<FFlowPinType_Vector>(PinName, VectorValue);
	return FlowPinType::IsSuccess(ResolveResult);
}

void UFlowBlackboardEntryValue_Vector::SetOnBlackboardComponent(UBlackboardComponent* BlackboardComponent) const
{
	if (IsValid(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsVector(Key.GetKeyName(), VectorValue);
	}
}

EBlackboardCompare::Type UFlowBlackboardEntryValue_Vector::CompareKeyValues(const UBlackboardComponent* BlackboardComponent, const FName& OtherKeyName) const
{
	if (!IsValid(BlackboardComponent))
	{
		UE_LOG(LogAIFlow, Error, TEXT("Cannot CompareKeyValues without a Blackboard!"));
		return EBlackboardCompare::NotEqual;
	}

	const FVector OtherValueAsVector = BlackboardComponent->GetValueAsVector(OtherKeyName);

	if (VectorValue.Equals(OtherValueAsVector))
	{
		return EBlackboardCompare::Equal;
	}
	else
	{
		return EBlackboardCompare::NotEqual;
	}
}

TSubclassOf<UBlackboardKeyType> UFlowBlackboardEntryValue_Vector::GetSupportedBlackboardKeyType() const
{
	return UBlackboardKeyType_Vector::StaticClass();
}
