//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "USQLite.h"
#include "USQLComboBoxWidget.h"
#include "USQLite_Shared.h"

#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SQLTableWidget : public SCompoundWidget {
private:
	TArray<TSharedPtr<SQLComboBoxWidget>> ColumnTypes;
	TArray<TSharedPtr<SEditableTextBox>> ColumnNames;
	TArray<TSharedPtr<SCheckBox>> ChecksNN;
	TArray<TSharedPtr<SCheckBox>> ChecksPK;
	TArray<TSharedPtr<SCheckBox>> ChecksAI;
	TArray<TSharedPtr<SCheckBox>> ChecksU;
	//
	TSharedPtr<IPropertyHandle> Columns;
	TSharedPtr<IPropertyHandle> Lock;

	TSharedPtr<SGridPanel> PanelGrid;
public:
	SQLTableWidget();
	virtual ~SQLTableWidget() override;
	//
	SLATE_BEGIN_ARGS(SQLTableWidget)
	: _Lock(TSharedPtr<IPropertyHandle>())
	, _Columns(TSharedPtr<IPropertyHandle>())
	{}//
		SLATE_ATTRIBUTE(TSharedPtr<IPropertyHandle>,Lock)
		SLATE_ATTRIBUTE(TSharedPtr<IPropertyHandle>,Columns)
	SLATE_END_ARGS()
	//
	void Construct(const FArguments &InArgs);
	void SetColumns(TSharedPtr<IPropertyHandle> InColumns);
	void OnSetColumnName(const FText &Input, ETextCommit::Type CommitType) const;
	void OnSetColumnNN(const ECheckBoxState state) const;
	void OnSetColumnPK(const ECheckBoxState state) const;
	void OnSetColumnAI(const ECheckBoxState state) const;
	void OnSetColumnU(const ECheckBoxState state) const;
	void SetColumnTypeByID(TSharedPtr<uint32> ColumnID, TSharedPtr<FString> NewType);
	FReply OnDeleteColumn(uint32 index);
	//
	FText OnGetResultString() const;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////