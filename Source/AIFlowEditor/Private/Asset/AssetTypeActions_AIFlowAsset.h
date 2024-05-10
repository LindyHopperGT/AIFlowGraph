// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "Asset/AssetTypeActions_FlowAsset.h"
#include "Toolkits/IToolkitHost.h"

class AIFLOWEDITOR_API FAssetTypeActions_AIFlowAsset : public FAssetTypeActions_FlowAsset
{
public:
	virtual FText GetName() const override;
	virtual uint32 GetCategories() override;

	virtual UClass* GetSupportedClass() const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
};
