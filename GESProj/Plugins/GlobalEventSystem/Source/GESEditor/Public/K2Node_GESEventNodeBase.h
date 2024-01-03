// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "K2Node.h"
#include "GESEditorCommon.h"
#include "K2Node_GESEventNodeBase.generated.h"

UCLASS(Abstract)
class GESEDITOR_API UK2Node_GESEventNodeBase : public UK2Node
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	FName EventTypeValue;
	
	UPROPERTY()
	TArray<FGESEventDataPinInfo> EventDataPinInfos;
	
	EEdGraphPinDirection EventDataPinDirection;

	FText NodeTitleCache;
	FText ToolTipHeadingCache;
	FText TooltipTextCache;
	FText KeywordsCache;
	
public:
	UK2Node_GESEventNodeBase() : EventDataPinDirection(EGPD_Input) {}
	UK2Node_GESEventNodeBase(const EEdGraphPinDirection InEventDataPinDirection) : EventDataPinDirection(InEventDataPinDirection) {}
	
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	
	virtual FText GetMenuCategory() const override { return FText::FromString(TEXT("GlobalEventSystem")); }

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override { return NodeTitleCache; }
	virtual FText GetToolTipHeading() const override { return ToolTipHeadingCache; }
	virtual FText GetTooltipText() const override { return TooltipTextCache; }
	virtual FText GetKeywords() const override { return KeywordsCache; }
	
	virtual FLinearColor GetNodeTitleColor() const override { return FLinearColor::Yellow; }
	virtual FLinearColor GetNodeBodyTintColor() const override { return FLinearColor::Red; }

	virtual bool ShouldDrawNodeAsControlPointOnly(int32& OutInputPinIndex, int32& OutOutputPinIndex) const override
	{
		OutInputPinIndex = 0;
		OutOutputPinIndex = 1;
		return true;
	}
	
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override
	{
		static const FSlateIcon Icon = FSlateIcon("EditorStyle", "GraphEditor.Default_16x");
		return Icon;
	}

	UEdGraphPin* GetThenPin() const
	{
		UEdGraphPin* Pin = FindPin(UEdGraphSchema_K2::PN_Then);
		check(Pin == nullptr || Pin->Direction == EGPD_Output); // If pin exists, it must be output
		return Pin;
	}
	
	virtual void AllocateDefaultPins() override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;

protected:
	virtual bool OnEventTypePinValueChanged(UEdGraphPin* Pin);

	virtual void BeforeAllocateEventDataPins() {}
	virtual void AllocateEventDataPins();
	virtual void AfterAllocateEventDataPins() {}
};
