// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "FlowBlackboardEntryCustomization.h"
#include "Types/FlowBlackboardEntry.h"

#include "BehaviorTree/BlackboardAssetProvider.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"
#include "IDetailChildrenBuilder.h"
#include "UObject/UnrealType.h"

// FFlowBlackboardEntryCustomization Implementation

void FFlowBlackboardEntryCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> PropertyHandle = InStructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFlowBlackboardEntry, AllowedTypes));
	if (PropertyHandle)
	{
		StructBuilder.AddProperty(PropertyHandle.ToSharedRef());
	}
}

TSharedPtr<IPropertyHandle> FFlowBlackboardEntryCustomization::GetCuratedNamePropertyHandle() const
{
	check(StructPropertyHandle->IsValidHandle());

	TSharedPtr<IPropertyHandle> FoundHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFlowBlackboardEntry, KeyName));
	check(FoundHandle);

	return FoundHandle;
}

TArray<FName> FFlowBlackboardEntryCustomization::GetCuratedNameOptions() const
{
	TArray<FName> Results;

	const UBlackboardData* BlackboardAsset = GetBlackboardData();
	if (!IsValid(BlackboardAsset))
	{
		return Results;
	}

	const FFlowBlackboardEntry* FlowBlackboardEntry = GetFlowBlackboardEntry();
	if (!FlowBlackboardEntry)
	{
		return Results;
	}

	Results = GetFlowBlackboardEntries(*BlackboardAsset, *FlowBlackboardEntry);

	return Results;
}

TArray<FName> FFlowBlackboardEntryCustomization::GetFlowBlackboardEntries(
	const UBlackboardData& BlackboardAsset,
	const FFlowBlackboardEntry& FlowBlackboardEntry)
{
	TArray<FName> ValidBlackboardEntries;

	const TArray<TObjectPtr<UBlackboardKeyType>>& AllowedTypes = FlowBlackboardEntry.AllowedTypes;

	for (const UBlackboardData* It = &BlackboardAsset; It; It = It->Parent)
	{
		for (int32 KeyIndex = 0; KeyIndex < It->Keys.Num(); KeyIndex++)
		{
			const FBlackboardEntry& EntryInfo = It->Keys[KeyIndex];
			if (!EntryInfo.KeyType)
			{
				continue;
			}

			// Add all BlackboardKeys that pass the AllowedTypes filters
			bool bFilterPassed = true;
			if (AllowedTypes.Num())
			{
				bFilterPassed = false;

				for (int32 FilterIndex = 0; FilterIndex < AllowedTypes.Num(); FilterIndex++)
				{
					UBlackboardKeyType* AllowedType = AllowedTypes[FilterIndex];

					if (EntryInfo.KeyType->IsAllowedByFilter(AllowedType))
					{
						bFilterPassed = true;

						break;
					}

					// Special-case for enum filter without an EnumType set
					//  (that is, show all enum type blackboard entries when no specific enum type filter is set)
					if (UBlackboardKeyType_Enum* AllowedTypeEnum = Cast<UBlackboardKeyType_Enum>(AllowedType))
					{
						if (EntryInfo.KeyType->IsA<UBlackboardKeyType_Enum>() && !AllowedTypeEnum->EnumType)
						{
							bFilterPassed = true;

							break;
						}
					}
				}
			}

			if (bFilterPassed)
			{
				ValidBlackboardEntries.Add(EntryInfo.EntryName);
			}
		}
	}

	return ValidBlackboardEntries;
}

void FFlowBlackboardEntryCustomization::SetCuratedName(const FName& NewKeyName)
{
	TSharedPtr<IPropertyHandle> KeyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFlowBlackboardEntry, KeyName));

	check(KeyHandle);

	KeyHandle->SetPerObjectValue(0, NewKeyName.ToString());
}

bool FFlowBlackboardEntryCustomization::TryGetCuratedName(FName& OutName) const
{
	const FFlowBlackboardEntry* FlowBlackboardEntry = GetFlowBlackboardEntry();
	if (FlowBlackboardEntry)
	{
		OutName = FlowBlackboardEntry->KeyName;

		return true;
	}
	else
	{
		return false;
	}
}

const IBlackboardAssetProvider* FFlowBlackboardEntryCustomization::TryGetBlackboardAssetProviderFromOuters() const
{
	check(StructPropertyHandle->IsValidHandle());

	TArray<UObject*> OuterObjects;
	StructPropertyHandle->GetOuterObjects(OuterObjects);

	if (OuterObjects.Num() != 1)
	{
		// We don't support multiselect. Peace out.
		return nullptr;
	}

	UObject* CurrentOuter = OuterObjects[0];

	while (IsValid(CurrentOuter))
	{
		const IBlackboardAssetProvider* FlowNodeOuter = Cast<IBlackboardAssetProvider>(CurrentOuter);
		if (FlowNodeOuter)
		{
			return FlowNodeOuter;
		}

		CurrentOuter = CurrentOuter->GetOuter();
	}

	return nullptr;
}

const UBlackboardData* FFlowBlackboardEntryCustomization::GetBlackboardData() const
{
	const IBlackboardAssetProvider* BlackboardProvider = TryGetBlackboardAssetProviderFromOuters();
	if (!BlackboardProvider)
	{
		return nullptr;
	}

	const UBlackboardData* BlackboardData = BlackboardProvider->GetBlackboardAsset();
	if (!IsValid(BlackboardData))
	{
		return nullptr;
	}

	return BlackboardData;
}
