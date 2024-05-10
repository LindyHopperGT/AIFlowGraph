// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "UObject/Interface.h"
#include "Templates/SubclassOf.h"

#include "FlowBlackboardInterface.generated.h"

// Forward Declarations
class UBlackboardKeyType;
class UBlackboardComponent;

// Implemented by objects that can access a Flow Blackboard
//  (eg, UAIFlowAsset, UAIFlowNode and UAIFlowNodeAddOn implement this)
UINTERFACE(MinimalAPI, BlueprintType, DisplayName = "Flow Blackboard Interface", meta = (CannotImplementInterfaceInBlueprint))
class UFlowBlackboardInterface : public UInterface
{
	GENERATED_BODY()
};

class AIFLOW_API IFlowBlackboardInterface
{
	GENERATED_BODY()

public:

	// Get the blackboard component
	UFUNCTION(BlueprintCallable, Category = "FlowNode")
	virtual UBlackboardComponent* GetBlackboardComponent() const;

	// Is the given blackboard key valid?
	UFUNCTION(BlueprintCallable, Category = "FlowNode")
	virtual bool IsValidBlackboardKey(const FName& KeyName) const;
	static bool IsValidBlackboardKey(const UBlackboardComponent& BlackboardComponent, const FName& KeyName);

	// Gather all blackboard keys of a given type
	UFUNCTION(BlueprintCallable, Category = "FlowNode")
	virtual TArray<FName> GatherAllBlackboardKeysOfType(UBlackboardKeyType* KeyType = nullptr) const;
	static TArray<FName> GatherAllBlackboardKeysOfType(const UBlackboardComponent& BlackboardComponent, UBlackboardKeyType* KeyType = nullptr);

	// Get the KeyType of a blackboard key
	UFUNCTION(BlueprintCallable, Category = "FlowNode")
	virtual TSubclassOf<UBlackboardKeyType> GetBlackboardKeyType(const FName& KeyName) const;
	static TSubclassOf<UBlackboardKeyType> GetBlackboardKeyType(const UBlackboardComponent& BlackboardComponent, const FName& KeyName);
};
