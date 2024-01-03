// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "K2Node_GESEventNodeBase.h"
#include "K2Node_GESDispatchEvent.generated.h"

UCLASS()
class GESEDITOR_API UK2Node_GESDispatchEvent : public UK2Node_GESEventNodeBase
{
	GENERATED_BODY()
	
public:
	UK2Node_GESDispatchEvent() : UK2Node_GESEventNodeBase(EGPD_Input)
	{
		NodeTitleCache = FText::FromString(TEXT("GESDispatchEvent"));
		ToolTipHeadingCache = FText::FromString(TEXT("GESDispatchEvent"));
		TooltipTextCache = FText::FromString(TEXT("Dispatch event by current blueprint"));
		KeywordsCache = FText::FromString(TEXT("GES,DE,DPE"));		
	}
	
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
};
