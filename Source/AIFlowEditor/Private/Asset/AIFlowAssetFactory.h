// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "CoreMinimal.h"
#include "Asset/FlowAssetFactory.h"
#include "AIFlowAssetFactory.generated.h"

UCLASS(HideCategories = Object)
class AIFLOWEDITOR_API UAIFlowAssetFactory : public UFlowAssetFactory
{
	GENERATED_UCLASS_BODY()	
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
