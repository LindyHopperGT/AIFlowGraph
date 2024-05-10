// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Blackboard/FlowBlackboardEntryValue_Rotator.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Rotator.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIFlowLogChannels.h"

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
	return FText::FromString(FString::Printf(TEXT("Set %s to %s"), *Key.GetKeyName().ToString(), *GetEditorValueString()));
}
#endif // WITH_EDITOR

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
