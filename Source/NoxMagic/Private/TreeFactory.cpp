#include "TreeFactory.h"
#include "TreeAsset.h"
#include "AssetTypeCategories.h"

UTreeFactory::UTreeFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UTreeAsset::StaticClass();
}

uint32 UTreeFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Misc;
}

UObject* UTreeFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UTreeAsset* result = NewObject<UTreeAsset>(InParent, InClass, InName, Flags);
	result->nodes.Add(FTreeNode("undefined", FVector2D::ZeroVector));
	return result;
}