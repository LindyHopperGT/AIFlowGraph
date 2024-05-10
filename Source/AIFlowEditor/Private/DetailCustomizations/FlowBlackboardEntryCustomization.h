// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "UnrealExtensions/IFlowCuratedNamePropertyCustomization.h"

#include "Types/FlowBlackboardEntry.h"

// Forward Declaration
class IBlackboardAssetProvider;
class UBlackboardData;

// Details customization for FFlowBlackboardEntry
class FFlowBlackboardEntryCustomization : public IFlowCuratedNamePropertyCustomization
{
private:
	typedef IFlowCuratedNamePropertyCustomization Super;

public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance() { return MakeShareable(new FFlowBlackboardEntryCustomization()); }

protected:

	//~Begin IPropertyTypeCustomization
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	//~End IPropertyTypeCustomization

	//~Begin ICuratedNamePropertyCustomization
	virtual TSharedPtr<IPropertyHandle> GetCuratedNamePropertyHandle() const override;
	virtual void SetCuratedName(const FName& NewName) override;
	virtual bool TryGetCuratedName(FName& OutName) const override;
	virtual TArray<FName> GetCuratedNameOptions() const override;
	//~End ICuratedNamePropertyCustomization

	// Accessor to return the actual struct being edited
	FORCEINLINE FFlowBlackboardEntry* GetFlowBlackboardEntry() const
		{ return IFlowExtendedPropertyTypeCustomization::TryGetTypedStructValue<FFlowBlackboardEntry>(StructPropertyHandle); }

	const IBlackboardAssetProvider* TryGetBlackboardAssetProviderFromOuters() const;
	const UBlackboardData* GetBlackboardData() const;

	static TArray<FName> GetFlowBlackboardEntries(const UBlackboardData& BlackboardAsset, const FFlowBlackboardEntry& FlowBlackboardEntry);
};
