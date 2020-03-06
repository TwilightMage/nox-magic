//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "USQLEditor.h"
#include "USQLEditor_Shared.h"
#include "Kismet2/KismetEditorUtilities.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOCTEXT_NAMESPACE "Synaptech"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

USQLite_Factory::USQLite_Factory(const class FObjectInitializer &OBJ) : Super(OBJ) {
	SupportedClass = USQLite::StaticClass();
	bEditAfterNew = true;
	bCreateNew = true;
}

UObject* USQLite_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) {
	check(Class->IsChildOf(USQLite::StaticClass()));
	return NewObject<USQLite>(InParent,Class,Name,Flags|RF_Transactional,Context);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FText FATA_USQLite::GetName() const {
	return NSLOCTEXT("FATA_USQLite","FATA_USQLite","SQL Database");
}

FText FATA_USQLite::GetAssetDescription(const FAssetData &AssetData) const {
	return FText::FromString(FString(TEXT("SQL Database Asset.")));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////