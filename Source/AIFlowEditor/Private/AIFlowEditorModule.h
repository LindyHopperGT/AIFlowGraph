// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "AssetTypeCategories.h"
#include "IAssetTypeActions.h"
#include "Modules/ModuleInterface.h"
#include "PropertyEditorDelegates.h"

class AIFLOWEDITOR_API FAIFlowEditorModule : public IModuleInterface
{
public:
	static EAssetTypeCategories::Type FlowAssetCategory;

private:
	TArray<TSharedRef<IAssetTypeActions>> RegisteredAssetActions;
	TSet<FName> CustomClassLayouts;
	TSet<FName> CustomStructLayouts;

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterAssets();
	void UnregisterAssets();

	void RegisterDetailCustomizations();
	void UnregisterDetailCustomizations();

	void RegisterCustomClassLayout(const TSubclassOf<UObject> Class, const FOnGetDetailCustomizationInstance DetailLayout);
	void RegisterCustomStructLayout(const UScriptStruct& Struct, const FOnGetPropertyTypeCustomizationInstance DetailLayout);
};
