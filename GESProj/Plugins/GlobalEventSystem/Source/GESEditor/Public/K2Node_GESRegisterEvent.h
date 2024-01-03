// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "K2Node_GESEventNodeBase.h"
#include "K2Node_GESRegisterEvent.generated.h"

UCLASS()
class GESEDITOR_API UK2Node_GESRegisterEvent : public UK2Node_GESEventNodeBase
{
	GENERATED_BODY()

public:
	UK2Node_GESRegisterEvent() : UK2Node_GESEventNodeBase(EGPD_Output)
	{
		NodeTitleCache = FText::FromString(TEXT("GESRegisterEvent"));
		ToolTipHeadingCache = FText::FromString(TEXT("GESRegisterEvent"));
		TooltipTextCache = FText::FromString(TEXT("Register event for current blueprint"));
		KeywordsCache = FText::FromString(TEXT("GES,RE,RGE"));
	}
	
	virtual void BeforeAllocateEventDataPins() override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

	// check current event is registered or not
	bool IsEventRegistered() const;
};
