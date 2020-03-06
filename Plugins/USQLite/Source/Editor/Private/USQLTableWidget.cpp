//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "USQLTableWidget.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOCTEXT_NAMESPACE "Synaptech"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SQLTableWidget::SQLTableWidget(){}

SQLTableWidget::~SQLTableWidget() {
	ColumnNames.Empty();
	ColumnTypes.Empty();
	ChecksNN.Empty();
	ChecksPK.Empty();
	ChecksAI.Empty();
	ChecksU.Empty();
	//
	Columns.Reset();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQLTableWidget::Construct(const FArguments &InArgs) {
	Lock = InArgs._Lock.Get();
	Columns = InArgs._Columns.Get();
	//
	check(Lock->IsValidHandle());
	check(Columns->IsValidHandle());
	//
	uint32 R;
	ColumnTypes.Empty();
	ColumnNames.Empty();
	ChecksNN.Empty();
	ChecksPK.Empty();
	ChecksAI.Empty();
	ChecksU.Empty();
	Columns->GetNumChildren(R);
	//
	//
	for (uint32 I = 0; I < R; I++)
	{
		ColumnTypes.Add(TSharedPtr<SQLComboBoxWidget>(nullptr));
		ColumnNames.Add(TSharedPtr<SEditableTextBox>(nullptr));
		ChecksNN.Add(TSharedPtr<SCheckBox>(nullptr));
		ChecksPK.Add(TSharedPtr<SCheckBox>(nullptr));
		ChecksAI.Add(TSharedPtr<SCheckBox>(nullptr));
		ChecksU.Add(TSharedPtr<SCheckBox>(nullptr));
	}
	//
	TSharedRef<SQLTableWidget>ThisShared = StaticCastSharedRef<SQLTableWidget>(AsShared());
	SAssignNew(PanelGrid, SGridPanel);
	//
	PanelGrid->AddSlot(0, 0).Padding(FMargin(2.f))
	[
		SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
		[
			SNew(STextBlock).Text(FText::FromString("Type"))
		]
	];
	PanelGrid->AddSlot(1, 0).Padding(FMargin(2.f))
	[
		SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
		[
			SNew(STextBlock).Text(FText::FromString("Name"))
		]
	];

	PanelGrid->AddSlot(2, 0).Padding(FMargin(2.f))
	[
		SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
		[
			SNew(STextBlock).Text(FText::FromString("NN")).ToolTipText(FText::FromString("NOT NULL"))
		]
	];
	PanelGrid->AddSlot(3, 0).Padding(FMargin(2.f))
	[
		SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
		[
			SNew(STextBlock).Text(FText::FromString("PK")).ToolTipText(FText::FromString("PRIMARY KEY"))
		]
	];
	PanelGrid->AddSlot(4, 0).Padding(FMargin(2.f))
	[
		SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
		[
			SNew(STextBlock).Text(FText::FromString("AI")).ToolTipText(FText::FromString("AUTOINCREMENT"))
		]
	];
	PanelGrid->AddSlot(5, 0).Padding(FMargin(2.f))
	[
		SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
		[
			SNew(STextBlock).Text(FText::FromString("U")).ToolTipText(FText::FromString("UNIQUE"))
		]
	];
	//
	for (uint32 I=0; I<R; I++) {
		FString ColumnType;
		FText ColumnName;
		bool ColumnNN;
		bool ColumnPK;
		bool ColumnAI;
		bool ColumnU;

		bool Locked; Lock->GetValue(Locked);
		TSharedPtr<IPropertyHandle> ColumnHandle = Columns->GetChildHandle(I).ToSharedRef();
		//
		ColumnHandle->GetChildHandle(0)->GetValue(ColumnType);
		ColumnHandle->GetChildHandle(1)->GetValueAsDisplayText(ColumnName);
		ColumnHandle->GetChildHandle(2)->GetValue(ColumnNN);
		ColumnHandle->GetChildHandle(3)->GetValue(ColumnPK);
		ColumnHandle->GetChildHandle(4)->GetValue(ColumnAI);
		ColumnHandle->GetChildHandle(5)->GetValue(ColumnU);

		bool canBeAI = (ColumnType == "INTEGER" || ColumnType == "BYTE") && ColumnPK;
		//
		TSharedPtr<FString> ColumnTypeHandle = MakeShared<FString>(ColumnType);
		PanelGrid->AddSlot(0, I + 1).Padding(FMargin(2.f))
		[
			SNew(SBox).MinDesiredWidth(150.f)
			[
				SAssignNew(ColumnTypes[I], SQLComboBoxWidget).IsEnabled(!Locked)
				.Owner(ThisShared).ColumnID(MakeShared<uint32>(I)).ColumnType(ColumnTypeHandle)
			]
		];
		//
		PanelGrid->AddSlot(1, I + 1).Padding(FMargin(2.f))
		[
			SAssignNew(ColumnNames[I],SEditableTextBox).IsReadOnly(Locked).MinDesiredWidth(200.f)
			.OnTextCommitted(this,&SQLTableWidget::OnSetColumnName).Text(ColumnName)
		];

		PanelGrid->AddSlot(2, I + 1).Padding(FMargin(2.f))
		[
			SNew(SBox).HAlign(HAlign_Center)
			[
				SAssignNew(ChecksNN[I], SCheckBox).IsEnabled(!Locked)
				.OnCheckStateChanged(this, &SQLTableWidget::OnSetColumnNN).IsChecked(ColumnNN ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
			]
		];
		PanelGrid->AddSlot(3, I + 1).Padding(FMargin(2.f))
		[
			SNew(SBox).HAlign(HAlign_Center)
			[
				SAssignNew(ChecksPK[I], SCheckBox).IsEnabled(!Locked)
				.OnCheckStateChanged(this, &SQLTableWidget::OnSetColumnPK).IsChecked(ColumnPK ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
			]
		];
		PanelGrid->AddSlot(4, I + 1).Padding(FMargin(2.f))
		[
			SNew(SBox).HAlign(HAlign_Center)
			[
				SAssignNew(ChecksAI[I], SCheckBox).IsEnabled(!Locked && canBeAI)
				.OnCheckStateChanged(this, &SQLTableWidget::OnSetColumnAI).IsChecked((ColumnType == "INTEGER" || ColumnType == "BYTE") ? ColumnAI ? ECheckBoxState::Checked : ECheckBoxState::Unchecked : ECheckBoxState::Unchecked)
				.ToolTipText(canBeAI ? FText::FromString("Only INTEGER PRIMARY KEY can be AUTOINCREMENT") : FText::GetEmpty())
			]
		];
		PanelGrid->AddSlot(5, I + 1).Padding(FMargin(2.f))
		[
			SNew(SBox).HAlign(HAlign_Center)
			[
				SAssignNew(ChecksU[I], SCheckBox).IsEnabled(!Locked)
				.OnCheckStateChanged(this, &SQLTableWidget::OnSetColumnU).IsChecked(ColumnU ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
			]
		];
		PanelGrid->AddSlot(6, I + 1).Padding(FMargin(2.f))
		[
			SNew(SBox).WidthOverride(100.f)
		];
		PanelGrid->AddSlot(7, I + 1).Padding(FMargin(2.f))
		[
			SNew(SButton).Text(FText::FromString("Remove")).OnClicked_Lambda([&]() { return OnDeleteColumn(I); }).IsEnabled(!Locked)
		];
	}///
	//
	//
	ChildSlot.HAlign(HAlign_Fill).VAlign(VAlign_Fill)[PanelGrid->AsShared()];
}

void SQLTableWidget::OnSetColumnName(const FText &Input, ETextCommit::Type CommitType) const {
	for (TSharedPtr<SEditableTextBox> Field : ColumnNames) {
		Field->SetBorderBackgroundColor(FSlateColor(FLinearColor::White));
		Field->SetForegroundColor(FSlateColor(FLinearColor::Black));
		Field->SetToolTipText(Input);
	}///
	
	FString oldname;
	//
	uint32 C=0; FText Warning = FText();
	for (TSharedPtr<SEditableTextBox> Field : ColumnNames) {
		TSharedPtr<IPropertyHandle> Handle = Columns->GetChildHandle(C)->GetChildHandle(1).ToSharedRef();
		FString oldFieldValue;
		Handle->GetValue(oldFieldValue);
		if (oldFieldValue != Field->GetText().ToString())
		{
			oldname = oldFieldValue;
			Handle->SetValue(Field->GetText().ToString());
		}
		//
		for (TSharedPtr<SEditableTextBox> Other : ColumnNames) {
			if (Other==Field) {continue;}
			if (Other->GetText().ToString().Equals(Field->GetText().ToString(),ESearchCase::IgnoreCase)) {
				Field->SetForegroundColor(FSlateColor(FLinearColor::White));
				Other->SetForegroundColor(FSlateColor(FLinearColor::White));
				Field->SetBorderBackgroundColor(FSlateColor(FLinearColor(0.5f,0.1f,0.1f,0.5f)));
				Other->SetBorderBackgroundColor(FSlateColor(FLinearColor(0.5f,0.1f,0.1f,0.5f)));
				//
				Warning = FText::FromString(TEXT("SQL Table's Categories must be unique.\nYour Table has duplicate columns."));
				LOG_DB(USQL::Logs,ESQLSeverity::Warning,Warning.ToString());
				Field->SetToolTipText(Warning);
			}///
		}///
	C++;}
	//
	if (!Warning.IsEmpty())
	{
		auto Title = FText::FromString(":: USQLite ::");
		FMessageDialog::Open(EAppMsgType::Ok,Warning,&Title);
	}
	else if (!oldname.IsEmpty())
	{
		TArray<UObject*> Packages;
		Columns->GetOuterObjects(Packages);

		for (auto Pack : Packages) {
			Pack->MarkPackageDirty();
			auto Owner = Cast<USQLite>(Pack);
			if (Owner) { Owner->OnRenameColumn(oldname, Input.ToString()); }
		}///
	}
}

void SQLTableWidget::OnSetColumnNN(const ECheckBoxState state) const
{
	uint32 C = 0;
	for (TSharedPtr<SCheckBox> check : ChecksNN) {
		bool checked;
		Columns->GetChildHandle(C)->GetChildHandle(2).ToSharedRef()->GetValue(checked);
		if (checked != check->IsChecked())
		{
			Columns->GetChildHandle(C)->GetChildHandle(2).ToSharedRef()->SetValue(check->IsChecked());

			TArray<UObject*> Packages;
			Columns->GetOuterObjects(Packages);

			for (auto Pack : Packages)
			{
				Pack->MarkPackageDirty();
				auto Owner = Cast<USQLite>(Pack);
				if (Owner) Owner->OnTableDirty();
			}///
		}
		C++;
	}
}

void SQLTableWidget::OnSetColumnPK(const ECheckBoxState state) const
{
	uint32 C = 0;
	for (TSharedPtr<SCheckBox> check : ChecksNN) {
		bool checked;
		Columns->GetChildHandle(C)->GetChildHandle(2).ToSharedRef()->GetValue(checked);
		if (checked != check->IsChecked())
		{
			Columns->GetChildHandle(C)->GetChildHandle(2).ToSharedRef()->SetValue(check->IsChecked());

			TArray<UObject*> Packages;
			Columns->GetOuterObjects(Packages);

			bool canBeAI = (ColumnTypes[C]->GetComboBoxContent().ToString() == "INTEGER" || ColumnTypes[C]->GetComboBoxContent().ToString() == "BYTE") && ChecksPK[C]->IsChecked();
			ChecksAI[C]->SetEnabled(canBeAI);
			ChecksAI[C]->SetToolTipText(canBeAI ? FText::FromString("Only INTEGER PRIMARY KEY can be AUTOINCREMENT") : FText::GetEmpty());
			if (!canBeAI)
			{
				ChecksAI[C]->SetIsChecked(ECheckBoxState::Unchecked);
			}

			for (auto Pack : Packages)
			{
				Pack->MarkPackageDirty();
				auto Owner = Cast<USQLite>(Pack);
				if (Owner) Owner->OnTableDirty();
			}///
		}
		C++;
	}
}

void SQLTableWidget::OnSetColumnAI(const ECheckBoxState state) const
{
	uint32 C = 0;
	for (TSharedPtr<SCheckBox> check : ChecksNN) {
		bool checked;
		Columns->GetChildHandle(C)->GetChildHandle(4).ToSharedRef()->GetValue(checked);
		if (checked != check->IsChecked())
		{
			Columns->GetChildHandle(C)->GetChildHandle(4).ToSharedRef()->SetValue(check->IsChecked());

			TArray<UObject*> Packages;
			Columns->GetOuterObjects(Packages);

			for (auto Pack : Packages)
			{
				Pack->MarkPackageDirty();
				auto Owner = Cast<USQLite>(Pack);
				if (Owner) Owner->OnTableDirty();
			}///
		}
		C++;
	}
}

void SQLTableWidget::OnSetColumnU(const ECheckBoxState state) const
{
	uint32 C = 0;
	for (TSharedPtr<SCheckBox> check : ChecksNN) {
		bool checked;
		Columns->GetChildHandle(C)->GetChildHandle(5).ToSharedRef()->GetValue(checked);
		if (checked != check->IsChecked())
		{
			Columns->GetChildHandle(C)->GetChildHandle(5).ToSharedRef()->SetValue(check->IsChecked());

			TArray<UObject*> Packages;
			Columns->GetOuterObjects(Packages);

			for (auto Pack : Packages)
			{
				Pack->MarkPackageDirty();
				auto Owner = Cast<USQLite>(Pack);
				if (Owner) Owner->OnTableDirty();
			}///
		}
		C++;
	}
}

FText SQLTableWidget::OnGetResultString() const
{
	FString SQL;
	uint32 R; Columns->GetNumChildren(R);
	
	TArray<FString> columnTypes;
	for (uint32 I=0; I<R; I++)
	{
		FString ColumnName, ColumnType;
		bool NN, PK, AI, U;
		Columns->GetChildHandle(I)->GetChildHandle(0).ToSharedRef()->GetValue(ColumnType);
		Columns->GetChildHandle(I)->GetChildHandle(1).ToSharedRef()->GetValue(ColumnName);
		Columns->GetChildHandle(I)->GetChildHandle(2).ToSharedRef()->GetValue(NN);
		Columns->GetChildHandle(I)->GetChildHandle(3).ToSharedRef()->GetValue(PK);
		Columns->GetChildHandle(I)->GetChildHandle(4).ToSharedRef()->GetValue(AI);
		Columns->GetChildHandle(I)->GetChildHandle(5).ToSharedRef()->GetValue(U);

		columnTypes.Add(FString::Printf(TEXT("`%s` %s"), *ColumnName, *USQL::BuildFieldTypeName(USQL::USQLTypes.Downcast(ColumnType), NN, PK, AI, U)));
	}
	SQL = FString::Printf(TEXT("CREATE TABLE IF NOT EXISTS `%s` (%s);"), *FString("test"), *FString::Join(columnTypes, *FString(", ")));
	//
	return FText::FromString(*SQL);
}

void SQLTableWidget::SetColumns(TSharedPtr<IPropertyHandle>InColumns) {
	Columns = InColumns;
}

void SQLTableWidget::SetColumnTypeByID(TSharedPtr<uint32> ColumnID, TSharedPtr<FString> NewType) {
	if (!ColumnID.IsValid()||!NewType.IsValid()) {return;}
	//
	TSharedPtr<IPropertyHandle>ColumnHandle = Columns->GetChildHandle(ColumnID.ToSharedRef().Get())->GetChildHandle(0).ToSharedRef();
	ColumnHandle->SetValue(*NewType.Get());
	//
	if (!Columns->IsValidHandle()) {return;}
	//
	TArray<UObject*> Packages;
	Columns->GetOuterObjects(Packages);
	//
	for (auto Pack : Packages) {
		Pack->MarkPackageDirty();
		auto Owner = Cast<USQLite>(Pack);
		if (Owner) {Owner->OnTableDirty();}
	}///

	bool canBeAI = (*NewType.Get() == "INTEGER" || *NewType.Get() == "BYTE") && ChecksPK[*ColumnID.Get()];
	ChecksAI[*ColumnID.Get()]->SetEnabled(canBeAI);
	ChecksAI[*ColumnID.Get()]->SetToolTipText(canBeAI ? FText::FromString("Only INTEGER PRIMARY KEY can be AUTOINCREMENT") : FText::GetEmpty());
	if (!canBeAI)
	{
		ChecksAI[*ColumnID.Get()]->SetIsChecked(ECheckBoxState::Unchecked);
	}
}

FReply SQLTableWidget::OnDeleteColumn(uint32 index)
{
	for (int i = 0; i < 8; i++)
	{
		PanelGrid->RemoveSlot(PanelGrid->Slot(i, index + 1).GetWidget());
	}

	return FReply::Handled();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////