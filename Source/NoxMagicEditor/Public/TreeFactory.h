#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "TreeFactory.generated.h"

UCLASS()
class NOXMAGICEDITOR_API UTreeFactory : public UFactory
{
	GENERATED_BODY()

public:

	UTreeFactory();
	virtual uint32 GetMenuCategories() const override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	
};
