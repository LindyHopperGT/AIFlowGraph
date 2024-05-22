// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Interfaces/FlowBlackboardInterface.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowBlackboardInterface)

bool IFlowBlackboardInterface::IsValidBlackboardKey(const FName& KeyName) const
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (!IsValid(BlackboardComp))
	{
		return false;
	}

	return IsValidBlackboardKey(*BlackboardComp, KeyName);
}

bool IFlowBlackboardInterface::IsValidBlackboardKey(const UBlackboardComponent& BlackboardComp, const FName& KeyName)
{
	const FBlackboard::FKey KeyId = BlackboardComp.GetKeyID(KeyName);
	const bool bIsValidKey = BlackboardComp.IsValidKey(KeyId);
	return bIsValidKey;
}

TArray<FName> IFlowBlackboardInterface::GatherAllBlackboardKeysOfType(UBlackboardKeyType* AllowedType) const
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (!IsValid(BlackboardComp))
	{
		return TArray<FName>();
	}

	return GatherAllBlackboardKeysOfType(*BlackboardComp, AllowedType);
}

TArray<FName> IFlowBlackboardInterface::GatherAllBlackboardKeysOfType(const UBlackboardComponent& BlackboardComp, UBlackboardKeyType* AllowedType)
{
	TArray<FName> MatchingKeys;

	UBlackboardData* BlackboardAsset = BlackboardComp.GetBlackboardAsset();
	if (!IsValid(BlackboardAsset))
	{
		return MatchingKeys;
	}

	MatchingKeys.Reserve(BlackboardComp.GetNumKeys());

	// Get matching keys from all blackboards
	for (const UBlackboardData* It = BlackboardAsset; It; It = It->Parent)
	{
		for (int32 KeyIndex = 0; KeyIndex < It->Keys.Num(); KeyIndex++)
		{
			const FBlackboardEntry& EntryInfo = It->Keys[KeyIndex];
			if (!EntryInfo.KeyType)
			{
				continue;
			}

			const bool bFilterPassed = !IsValid(AllowedType) || EntryInfo.KeyType->IsAllowedByFilter(AllowedType);

			if (bFilterPassed)
			{
				MatchingKeys.Add(EntryInfo.EntryName);
			}
		}
	}

	return MatchingKeys;
}

UBlackboardKeyType* IFlowBlackboardInterface::GetBlackboardKeyType(const FName& KeyName) const
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (!IsValid(BlackboardComp))
	{
		return nullptr;
	}

	return GetBlackboardKeyType(*BlackboardComp, KeyName);
}

UBlackboardKeyType* IFlowBlackboardInterface::GetBlackboardKeyType(const UBlackboardComponent& BlackboardComp, const FName& KeyName)
{
	return GetBlackboardKeyType(BlackboardComp.GetBlackboardAsset(), KeyName);
}

UBlackboardKeyType* IFlowBlackboardInterface::GetBlackboardKeyType(const UBlackboardData* BlackboardAsset, const FName& KeyName)
{
	if (!BlackboardAsset)
	{
		return nullptr;
	}

	const FBlackboard::FKey KeyId = BlackboardAsset->GetKeyID(KeyName);

	// Look for the key on all matching blackboards
	for (const UBlackboardData* It = BlackboardAsset; It; It = It->Parent)
	{
		if (const FBlackboardEntry* BlackboardEntry = It->GetKey(KeyId))
		{
			return BlackboardEntry->KeyType;
		}
	}

	return nullptr;
}

UBlackboardComponent* IFlowBlackboardInterface::GetBlackboardComponent() const
{
	// Must be overridden by implementing classes to return the appropriate blackboard component
	return nullptr;
}

