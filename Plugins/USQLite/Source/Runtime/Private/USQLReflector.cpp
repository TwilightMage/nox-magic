//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "USQLReflector.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Reflector Routines:

bool USQLReflector::Logs;

bool USQLReflector::IsSupportedProperty(const UProperty* Property) {
	return (
		Property->IsA(UStrProperty::StaticClass()) ||
		Property->IsA(UIntProperty::StaticClass()) ||
		Property->IsA(USetProperty::StaticClass()) ||
		Property->IsA(UMapProperty::StaticClass()) ||
		Property->IsA(UBoolProperty::StaticClass()) ||
		Property->IsA(UByteProperty::StaticClass()) ||
		Property->IsA(UEnumProperty::StaticClass()) ||
		Property->IsA(UNameProperty::StaticClass()) ||
		Property->IsA(UTextProperty::StaticClass()) ||
		Property->IsA(UFloatProperty::StaticClass()) ||
		Property->IsA(UArrayProperty::StaticClass()) ||
		Property->IsA(UStructProperty::StaticClass()) ||
		Property->IsA(UObjectProperty::StaticClass())
	);//
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Property Reflector:

FString USQLReflector::UPropertyToSQL(UBoolProperty* Property, const UObject* Container) {
	check(Property); check(Container);
	//
	const auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	const bool Value = Property->GetPropertyValue(ValuePtr);
	//
	return (Value==true) ? TEXT("'UBool>>true'") : TEXT("'UBool>>false'");
}

FString USQLReflector::UPropertyToSQL(UByteProperty* Property, const UObject* Container) {
	check(Property); check(Container);
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	return FString::Printf(TEXT("%i"),Property->GetUnsignedIntPropertyValue(ValuePtr));
}

FString USQLReflector::UPropertyToSQL(UIntProperty* Property, const UObject* Container) {
	check(Property); check(Container);
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	return FString::Printf(TEXT("%i"),Property->GetSignedIntPropertyValue(ValuePtr));
}

FString USQLReflector::UPropertyToSQL(UFloatProperty* Property, const UObject* Container) {
	check(Property); check(Container);
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	return FString::Printf(TEXT("%f"),Property->GetFloatingPointPropertyValue(ValuePtr));
}

FString USQLReflector::UPropertyToSQL(UEnumProperty* Property, const UObject* Container) {
	check(Property); check(Container);
	//
	FString Output = TEXT("None");
	UEnum* Enum = Property->GetEnum();
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	Output = Enum->GetNameStringByValue(Property->GetUnderlyingProperty()->GetUnsignedIntPropertyValue(ValuePtr));
	//
	return FString::Printf(TEXT("'UEnum>>%s'"),*Output);
}

FString USQLReflector::UPropertyToSQL(UNameProperty* Property, const UObject* Container) {
	check(Property); check(Container);
	//
	FString Output = TEXT("None");
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	Output = Property->GetPropertyValue(ValuePtr).ToString();
	Output = Output.Replace(TEXT("'"),TEXT("`"));
	//
	return FString::Printf(TEXT("'UName>>%s'"),*Output);
}

FString USQLReflector::UPropertyToSQL(UTextProperty* Property, const UObject* Container) {
	check(Property); check(Container);
	//
	FString Output = TEXT("None");
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	Output = Property->GetPropertyValue(ValuePtr).ToString();
	Output = Output.Replace(TEXT("'"),TEXT("`"));
	//
	return FString::Printf(TEXT("'UText>>%s'"),*Output);
}

FString USQLReflector::UPropertyToSQL(UStrProperty* Property, const UObject* Container, const ESQLWriteMode Mode) {
	check(Property); check(Container);
	FString Output = TEXT("None");
	//
	switch (Mode) {
		case ESQLWriteMode::String:
		{
			auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
			Output = Property->GetPropertyValue(ValuePtr);
			Output = Output.Replace(TEXT("'"),TEXT("`"));
			return FString::Printf(TEXT("'UString>>%s'"),*Output);
		}	break;
		//
		case ESQLWriteMode::TimeStamp:
			Output = FString(TEXT("DATETIME('now','localtime')")); return Output;
		break;
	default: break;}
	//
	return TEXT("'UString>>NULL'");
}

FString USQLReflector::UPropertyToSQL(UArrayProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	auto ArrayPtr = Property->ContainerPtrToValuePtr<void>(Container);
	FScriptArrayHelper Helper(Property,ArrayPtr);
	//
	FString Output; TArray<TSharedPtr<FJsonValue>>Out;
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonWriter<>>Buffer = TJsonWriterFactory<>::Create(&Output);
	//
	if (Property->Inner->IsA(UObjectProperty::StaticClass())) {
		auto Objects = *Property->ContainerPtrToValuePtr<TArray<UObject*>>(Container);
		for (auto OBJ : Objects) {
			if ((OBJ!=nullptr)&&OBJ->IsValidLowLevelFast()) {
				FString FullName;
				if (OBJ->IsA(AActor::StaticClass())) {
					FullName = MakeActorDBID(CastChecked<AActor>(OBJ));
				} else if (OBJ->IsA(UActorComponent::StaticClass())) {
					FullName = MakeComponentDBID(CastChecked<UActorComponent>(OBJ));
				} else {FullName=MakeObjectDBID(OBJ);}
				//
				auto ClassPath = OBJ->GetClass()->GetDefaultObject()->GetPathName();
				TSharedPtr<FJsonValue>JValue = MakeShareable(new FJsonValueString(ClassPath+TEXT("::")+FullName));
		Out.Add(JValue);}}
	} else {
		for (int32 I=0, N=Helper.Num()-1; I<=N; I++) {
			auto Value = FJsonObjectConverter::UPropertyToJsonValue(Property->Inner,Helper.GetRawPtr(I),0,CPF_Transient);
			if (Value.IsValid()&&(!Value->IsNull())) {Out.Push(Value);}
	}} if (Out.Num()>0) {JSON->SetArrayField(Property->GetName(),Out);}
	//
	FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);
	return FString::Printf(TEXT("'UArray>>%s'"),*Output);
}

FString USQLReflector::UPropertyToSQL(USetProperty* Property, const UObject* Container) {
	check(Property); check(Container);
	//
	auto SetPtr = Property->ContainerPtrToValuePtr<void>(Container);
	FScriptSetHelper Helper(Property,SetPtr);
	//
	FString Output; TArray<TSharedPtr<FJsonValue>>Out;
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonWriter<>>Buffer = TJsonWriterFactory<>::Create(&Output);
	//
	if (Property->ElementProp->IsA(UObjectProperty::StaticClass())) {
		auto Objects = *Property->ContainerPtrToValuePtr<TSet<UObject*>>(Container);
		for (auto OBJ : Objects) {
			if ((OBJ!=nullptr)&&OBJ->IsValidLowLevelFast()) {
				FString FullName;
				if (OBJ->IsA(AActor::StaticClass())) {
					FullName = MakeActorDBID(CastChecked<AActor>(OBJ));
				} else if (OBJ->IsA(UActorComponent::StaticClass())) {
					FullName = MakeComponentDBID(CastChecked<UActorComponent>(OBJ));
				} else {FullName=MakeObjectDBID(OBJ);}
				//
				auto ClassPath = OBJ->GetClass()->GetDefaultObject()->GetPathName();
				TSharedPtr<FJsonValue>JValue = MakeShareable(new FJsonValueString(ClassPath+TEXT("::")+FullName));
		Out.Add(JValue);}}
	} else {
		for (int32 I=0, N=Helper.Num()-1; I<=N; I++) {
			auto Value = FJsonObjectConverter::UPropertyToJsonValue(Property->ElementProp,Helper.GetElementPtr(I),0,CPF_Transient);
			if (Value.IsValid()&&(!Value->IsNull())) {Out.Push(Value);}
	}} if (Out.Num()>0) {JSON->SetArrayField(Property->GetName(),Out);}
	//
	FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);
	return FString::Printf(TEXT("'USet>>%s'"),*Output);
}

FString USQLReflector::UPropertyToSQL(UMapProperty* Property, const UObject* Container) {
	check(Property); check(Container);
	//
	auto MapPtr = Property->ContainerPtrToValuePtr<void>(Container);
	FScriptMapHelper Helper(Property,MapPtr);
	//
	FString Output;
	TArray<TSharedPtr<FJsonValue>>Keys;
	TArray<TSharedPtr<FJsonValue>>Values;
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonWriter<>>Buffer = TJsonWriterFactory<>::Create(&Output);
	//
	if (Property->KeyProp->IsA(UObjectProperty::StaticClass())) {
		auto Objects = *Property->ContainerPtrToValuePtr<TMap<UObject*,void*>>(Container);
		for (auto OBJ : Objects) {
			if ((OBJ.Key!=nullptr)&&OBJ.Key->IsValidLowLevelFast()) {
				FString FullName;
				if (OBJ.Key->IsA(AActor::StaticClass())) {
					FullName = MakeActorDBID(CastChecked<AActor>(OBJ.Key));
				} else if (OBJ.Key->IsA(UActorComponent::StaticClass())) {
					FullName = MakeComponentDBID(CastChecked<UActorComponent>(OBJ.Key));
				} else {FullName=MakeObjectDBID(OBJ.Key);}
				//
				auto ClassPath = OBJ.Key->GetClass()->GetDefaultObject()->GetPathName();
				TSharedPtr<FJsonValue>JKey = MakeShareable(new FJsonValueString(ClassPath+TEXT("::")+FullName));
		Keys.Add(JKey);}}
	} else {
		for (int32 I=0, N=Helper.Num()-1; I<=N; I++) {
			auto Key = FJsonObjectConverter::UPropertyToJsonValue(Property->KeyProp,Helper.GetKeyPtr(I),0,CPF_Transient);
			if (Key.IsValid()&&(!Key->IsNull())) {Keys.Push(Key);}
		}///
	}///
	//
	if ((Property->KeyProp->IsA(UNameProperty::StaticClass()))&&(Property->ValueProp->IsA(UObjectProperty::StaticClass()))) {
		auto Objects = *Property->ContainerPtrToValuePtr<TMap<FName,UObject*>>(Container);
		for (auto OBJ : Objects) {
			if ((OBJ.Value!=nullptr)&&OBJ.Value->IsValidLowLevelFast()) {
				FString FullName;
				if (OBJ.Value->IsA(AActor::StaticClass())) {
					FullName = MakeActorDBID(CastChecked<AActor>(OBJ.Value));
				} else if (OBJ.Value->IsA(UActorComponent::StaticClass())) {
					FullName = MakeComponentDBID(CastChecked<UActorComponent>(OBJ.Value));
				} else {FullName=MakeObjectDBID(OBJ.Value);}
				//
				auto ClassPath = OBJ.Value->GetClass()->GetDefaultObject()->GetPathName();
				TSharedPtr<FJsonValue>JValue = MakeShareable(new FJsonValueString(ClassPath+TEXT("::")+FullName));
		Values.Add(JValue);}}
	} else if ((Property->KeyProp->IsA(UObjectProperty::StaticClass()))&&(Property->ValueProp->IsA(UObjectProperty::StaticClass()))) {
		auto Objects = *Property->ContainerPtrToValuePtr<TMap<UObject*,UObject*>>(Container);
		for (auto OBJ : Objects) {
			if ((OBJ.Value!=nullptr)&&OBJ.Value->IsValidLowLevelFast()) {
				FString FullName;
				if (OBJ.Value->IsA(AActor::StaticClass())) {
					FullName = MakeActorDBID(CastChecked<AActor>(OBJ.Value));
				} else if (OBJ.Value->IsA(UActorComponent::StaticClass())) {
					FullName = MakeComponentDBID(CastChecked<UActorComponent>(OBJ.Value));
				} else {FullName=MakeObjectDBID(OBJ.Value);}
				//
				auto ClassPath = OBJ.Value->GetClass()->GetDefaultObject()->GetPathName();
				TSharedPtr<FJsonValue>JValue = MakeShareable(new FJsonValueString(ClassPath+TEXT("::")+FullName));
		Values.Add(JValue);}}
	} else if (Property->ValueProp->IsA(UObjectProperty::StaticClass())) {
		LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("Property Map Value Type <-> [Data Map Value Type] mismatch:  %s"),*Property->GetName()));
		LOG_DB(Logs,ESQLSeverity::Error,TEXT("Only  TMap<UObject*,UObject*>  OR  TMap<FName,UObject*>  are supported for Maps of Object References!"));
	} else {
		for (int32 I=0, N=Helper.Num()-1; I<=N; I++) {
			auto Value = FJsonObjectConverter::UPropertyToJsonValue(Property->ValueProp,Helper.GetValuePtr(I),0,CPF_Transient);
			if (Value.IsValid()&&(!Value->IsNull())) {Values.Push(Value);}
		}///
	}///
	//
	if ((Keys.Num()>0)&&(Values.Num()>0)) {
		JSON->SetArrayField(Property->GetName()+TEXT("_KEY"),Keys);
		JSON->SetArrayField(Property->GetName()+TEXT("_VALUE"),Values);
	}///
	//
	FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);
	return FString::Printf(TEXT("'UMap>>%s'"),*Output);
}

FString USQLReflector::UPropertyToSQL(UStructProperty* Property, const UObject* Container, const ESQLWriteMode Mode) {
	check(Property); check(Container); FString Output;
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonWriter<>>Buffer = TJsonWriterFactory<>::Create(&Output);
	//
	switch (Mode) {
		case ESQLWriteMode::DateTime:
		{
			auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
			TSharedPtr<FJsonValue>_JSON = FJsonObjectConverter::UPropertyToJsonValue(CastChecked<UProperty>(Property),ValuePtr,CPF_SaveGame,CPF_Transient);
			_JSON.Get()->TryGetString(Output); FDateTime Value; FDateTime::Parse(Output,Value);
			Output = FString::Printf(TEXT("%i"),Value.ToUnixTimestamp()); return Output;
		}	break;
		//
		case ESQLWriteMode::Color:
		{
			auto StructPtr = Property->ContainerPtrToValuePtr<void>(Container);
			auto Value = FJsonObjectConverter::UPropertyToJsonValue(CastChecked<UProperty>(Property),StructPtr,CPF_SaveGame,CPF_Transient);
			if (Value.IsValid()&&(!Value->IsNull())) {JSON->SetField(Property->GetName(),Value); FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);}
			return FString::Printf(TEXT("'UColor>>%s'"),*Output);
		}	break;
		//
		case ESQLWriteMode::Vector2D:
		{
			if (Property->Struct!=TBaseStructure<FVector2D>::Get()) {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Property Type <-> Column Type] mismatch:  %s"),*Property->GetName())); break;}
			auto StructPtr = Property->ContainerPtrToValuePtr<void>(Container);
			auto Value = FJsonObjectConverter::UPropertyToJsonValue(CastChecked<UProperty>(Property),StructPtr,CPF_SaveGame,CPF_Transient);
			if (Value.IsValid()&&(!Value->IsNull())) {JSON->SetField(Property->GetName(),Value); FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);}
			return FString::Printf(TEXT("'UVector2D>>%s'"),*Output);
		}	break;
		//
		case ESQLWriteMode::Vector3D:
		{
			if (Property->Struct!=TBaseStructure<FVector>::Get()) {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Property Type <-> Column Type] mismatch:  %s"),*Property->GetName())); break;}
			auto StructPtr = Property->ContainerPtrToValuePtr<void>(Container);
			auto Value = FJsonObjectConverter::UPropertyToJsonValue(CastChecked<UProperty>(Property),StructPtr,CPF_SaveGame,CPF_Transient);
			if (Value.IsValid()&&(!Value->IsNull())) {JSON->SetField(Property->GetName(),Value); FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);}
			return FString::Printf(TEXT("'UVector3D>>%s'"),*Output);
		}	break;
		//
		case ESQLWriteMode::Rotator:
		{
			if (Property->Struct!=TBaseStructure<FRotator>::Get()) {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Property Type <-> Column Type] mismatch:  %s"),*Property->GetName())); break;}
			auto StructPtr = Property->ContainerPtrToValuePtr<void>(Container);
			auto Value = FJsonObjectConverter::UPropertyToJsonValue(CastChecked<UProperty>(Property),StructPtr,CPF_SaveGame,CPF_Transient);
			if (Value.IsValid()&&(!Value->IsNull())) {JSON->SetField(Property->GetName(),Value); FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);}
			return FString::Printf(TEXT("'URotator>>%s'"),*Output);
		}	break;
		//
		case ESQLWriteMode::Struct:
		{
			if (Property->Struct==FRuntimeFloatCurve::StaticStruct()) {
				auto StructPtr = Property->ContainerPtrToValuePtr<FRuntimeFloatCurve>(Container);
				for (TFieldIterator<UProperty>IT(Property->Struct); IT; ++IT) {
					UProperty* Field = *IT;
					for (int32 I=0; I<Field->ArrayDim; ++I) {
						auto ValuePtr = Field->ContainerPtrToValuePtr<FRichCurve>(StructPtr,I);
						if (UStructProperty* CurveData = Cast<UStructProperty>(Field)) {ParseStructToJSON(JSON,CurveData,ValuePtr);}
				}} FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);
				return FString::Printf(TEXT("'UFloatCurve>>%s'"),*Output);
			break;}
			//
			auto StructPtr = Property->ContainerPtrToValuePtr<void>(Container);
			ParseStructToJSON(Output,Property,StructPtr);
			return FString::Printf(TEXT("'UStruct>>%s'"),*Output);
		}	break;
	default: break;}
	//
	return TEXT("'UStruct>>NULL'");
}

FString USQLReflector::UPropertyToSQL(UObjectProperty* Property, const UObject* Container, const ESQLWriteMode Mode) {
	check(Property); check(Container);
	//
	FString Output;
	switch (Mode) {
		case ESQLWriteMode::ObjectPtr:
		{
			auto OBJ = *Property->ContainerPtrToValuePtr<UObject*>(Container);
			if ((OBJ!=nullptr)&&OBJ->IsValidLowLevelFast()) {
				FString FullName;
				//
				if (OBJ->IsA(AActor::StaticClass())) {
					FullName = MakeActorDBID(CastChecked<AActor>(OBJ));
				} else if (OBJ->IsA(UActorComponent::StaticClass())) {
					FullName = MakeComponentDBID(CastChecked<UActorComponent>(OBJ));
				} else {FullName=MakeObjectDBID(OBJ);}
				//
				auto ClassPath = OBJ->GetClass()->GetDefaultObject()->GetPathName();
				//
				Output = (ClassPath+TEXT("::")+FullName);
				return FString::Printf(TEXT("'UObject>>%s'"),*Output);
			}///
		}	break;
	default: break;}
	//
	return TEXT("'UObject>>NULL'");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Property Parser:

void USQLReflector::SQLToUProperty(const FSQLField &Field, UBoolProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	if (Field.Text.ToLower()==TEXT("true")) {
		Property->SetPropertyValue(ValuePtr,true);
	return;}
	//
	Property->SetPropertyValue(ValuePtr,false);
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, UByteProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	Property->SetIntPropertyValue(ValuePtr,Field.Integer);
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, UIntProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	Property->SetIntPropertyValue(ValuePtr,Field.Integer);
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, UFloatProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	Property->SetFloatingPointPropertyValue(ValuePtr,Field.Float);
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, UNameProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	Property->SetPropertyValue(ValuePtr,FName(*Field.Text));
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, UTextProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	Property->SetPropertyValue(ValuePtr,FText::FromString(Field.Text));
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, UStrProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	Property->SetPropertyValue(ValuePtr,Field.Text);
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, UEnumProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	UEnum* Enum = Property->GetEnum();
	auto Value = Enum->GetValueByName(*Field.Text);
	if (Value==INDEX_NONE) {Value=0;}
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	Property->GetUnderlyingProperty()->SetIntPropertyValue(ValuePtr,Value);
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, UArrayProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(Field.Text);
	if (!FJsonSerializer::Deserialize(JReader,JSON)||!JSON.IsValid()) {
		LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object String for : [%s]"),*Property->GetName()));
	return;}
	//
	auto ArrayPtr = Property->ContainerPtrToValuePtr<void>(Container);
	FScriptArrayHelper Helper(Property,ArrayPtr);
	//
	if (JSON->HasField(Field.Name.ToString())) {
		auto JArray = JSON->GetArrayField(Field.Name.ToString());
		Helper.Resize(JArray.Num());
		//
		if (Property->Inner->IsA(UObjectProperty::StaticClass())) {
			TArray<UObject*>&_REF = *Property->ContainerPtrToValuePtr<TArray<UObject*>>(Container);
			for (int32 I=0, N=JArray.Num()-1; I<=N; I++) {
				const auto &JValue = JArray[I];
				if (JValue.IsValid()&&(!JValue->IsNull())) {
					FString Class, Instance;
					JValue->AsString().Split(TEXT("::"),&Class,&Instance);
					//
					const FSoftObjectPath ClassPath(Class);
					TSoftObjectPtr<UObject>ClassPtr(ClassPath);
					//
					auto CDO = ClassPtr.LoadSynchronous();
					if (IsInGameThread()&&(CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
						for (TActorIterator<AActor>Actor(Container->GetWorld()); Actor; ++Actor) {
							if (MakeActorDBID(*Actor)==Instance) {_REF[I]=(*Actor); break;}
						}///
					} else if ((CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
						DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUArrayProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUArrayProperty,STATGROUP_TaskGraphTasks);
						FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
							FSimpleDelegateGraphTask::FDelegate::CreateStatic(&RestoreActorReferenceIntoArrayInGameThread,Property,Container,Instance,I),
							GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUArrayProperty),
							nullptr, ENamedThreads::GameThread
						);//
					} else if (IsInGameThread()&&(CDO!=nullptr)) {
						for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
							if (MakeObjectDBID(*OBJ)==Instance) {_REF[I]=(*OBJ); break;}
						} if (_REF[I]==nullptr) {_REF[I]=NewObject<UObject>(Container,CDO->GetClass(),NAME_None,RF_NoFlags,CDO);}
					} else if (CDO!=nullptr) {
						DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUArrayProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUArrayProperty,STATGROUP_TaskGraphTasks);
						FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
							FSimpleDelegateGraphTask::FDelegate::CreateStatic(&NewObjectIntoArrayInGameThread,Property,Container,CDO,Instance,I),
							GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUArrayProperty),
							nullptr, ENamedThreads::GameThread
						);//
					}///
			}}//////
		} else {
			for (int32 I=0, N=JArray.Num()-1; I<=N; I++) {
				const auto &JValue = JArray[I];
				if (JValue.IsValid()&&(!JValue->IsNull())) {
					if (!FJsonObjectConverter::JsonValueToUProperty(JValue,Property->Inner,Helper.GetRawPtr(I),0,CPF_Transient)) {
						LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Array Element Type <-> Property Array Element Type] mismatch:  %s"),*Property->GetName()));
	}}}}} else {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Array <-> Property Array] mismatch:  %s"),*Property->GetName()));}
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, USetProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(Field.Text);
	if (!FJsonSerializer::Deserialize(JReader,JSON)||!JSON.IsValid()) {
			LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object String for : [%s]"),*Property->GetName()));
	return;}
	//
	auto SetPtr = Property->ContainerPtrToValuePtr<void>(Container);
	FScriptSetHelper Helper(Property,SetPtr);
	//
	if (JSON->HasField(Field.Name.ToString())) {
		auto JSet = JSON->GetArrayField(Field.Name.ToString());
		Helper.EmptyElements();
		//
		if (Property->ElementProp->IsA(UObjectProperty::StaticClass())) {
			TSet<UObject*>&_REF = *Property->ContainerPtrToValuePtr<TSet<UObject*>>(Container);
			for (int32 I=0, N=JSet.Num()-1; I<=N; I++) {
				const auto &JValue = JSet[I];
				if (JValue.IsValid()&&(!JValue->IsNull())) {
					FString Class, Instance;
					JValue->AsString().Split(TEXT("::"),&Class,&Instance);
					//
					const FSoftObjectPath ClassPath(Class);
					TSoftObjectPtr<UObject>ClassPtr(ClassPath);
					//
					auto CDO = ClassPtr.LoadSynchronous();
					if (IsInGameThread()&&(CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
						for (TActorIterator<AActor>Actor(Container->GetWorld()); Actor; ++Actor) {
							if (MakeActorDBID(*Actor)==Instance) {_REF.Add(*Actor); break;}
						}///
					} else if ((CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
						DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUSetProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUSetProperty,STATGROUP_TaskGraphTasks);
						FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
							FSimpleDelegateGraphTask::FDelegate::CreateStatic(&RestoreActorReferenceIntoSetInGameThread,Property,Container,Instance),
							GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUSetProperty),
							nullptr, ENamedThreads::GameThread
						);//
					} else if (IsInGameThread()&&(CDO!=nullptr)) {
						bool IsSet = false;
						for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
							if (MakeObjectDBID(*OBJ)==Instance) {_REF.Add(*OBJ); IsSet=true; break;}
						} if (!IsSet) {_REF.Add(NewObject<UObject>(Container,CDO->GetClass(),NAME_None,RF_NoFlags,CDO));}
					} else if (CDO!=nullptr) {
						DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUSetProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUSetProperty,STATGROUP_TaskGraphTasks);
						FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
							FSimpleDelegateGraphTask::FDelegate::CreateStatic(&NewObjectIntoSetInGameThread,Property,Container,CDO,Instance),
							GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUSetProperty),
							nullptr, ENamedThreads::GameThread
						);//
			}}}/////////
		} else {
			for (int32 I=0, N=JSet.Num()-1; I<=N; I++) {
				const auto &JValue = JSet[I];
				auto Index = Helper.AddDefaultValue_Invalid_NeedsRehash();
				if (JValue.IsValid()&&(!JValue->IsNull())) {
					if (!FJsonObjectConverter::JsonValueToUProperty(JValue,Property->ElementProp,Helper.GetElementPtr(Index),0,CPF_Transient)) {
						LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Set Element Type <-> Property Set Element Type] mismatch:  %s"),*Property->GetName()));
		}}}} Helper.Rehash();
	} else {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Set Member <-> Property Set Member] mismatch:  %s"),*Property->GetName()));}
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, UMapProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(Field.Text);
	if (!FJsonSerializer::Deserialize(JReader,JSON)||!JSON.IsValid()) {
			LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object String for : [%s]"),*Property->GetName()));
	return;}
	//
	auto MapPtr = Property->ContainerPtrToValuePtr<void>(Container);
	FScriptMapHelper Helper(Property,MapPtr);
	//
	TArray<TSharedPtr<FJsonValue>>JKeys;
	if (JSON->HasField(Field.Name.ToString()+TEXT("_KEY"))) {
		JKeys = JSON->GetArrayField(Field.Name.ToString()+TEXT("_KEY"));
		Helper.EmptyValues();
		//
		if (Property->KeyProp->IsA(UObjectProperty::StaticClass())) {
			TMap<UObject*,void*>&_REF = *Property->ContainerPtrToValuePtr<TMap<UObject*,void*>>(Container);
			for (int32 I=0, N=JKeys.Num()-1; I<=N; I++) {
				const auto &JKey = JKeys[I];
				if (JKey.IsValid()&&(!JKey->IsNull())) {
					FString Class, Instance;
					JKey->AsString().Split(TEXT("::"),&Class,&Instance);
					//
					const FSoftObjectPath ClassPath(Class);
					TSoftObjectPtr<UObject>ClassPtr(ClassPath);
					//
					auto CDO = ClassPtr.LoadSynchronous();
					if (IsInGameThread()&&(CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
						for (TActorIterator<AActor>Actor(Container->GetWorld()); Actor; ++Actor) {
							if (MakeActorDBID(*Actor)==Instance) {_REF.Add(*Actor); break;}
						}///
					} else if ((CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
						DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUMapProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty,STATGROUP_TaskGraphTasks);
						FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
							FSimpleDelegateGraphTask::FDelegate::CreateStatic(&RestoreActorReferenceIntoMapKeyInGameThread,Property,Container,Instance),
							GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty),
							nullptr, ENamedThreads::GameThread
						);//
					} else if (IsInGameThread()&&(CDO!=nullptr)) {
						bool IsSet = false;
						for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
							if (MakeObjectDBID(*OBJ)==Instance) {_REF.Add(*OBJ); IsSet=true; break;}
						} if (!IsSet) {_REF.Add(NewObject<UObject>(Container,CDO->GetClass(),NAME_None,RF_NoFlags,CDO));}
					} else if (CDO!=nullptr) {
						DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUMapProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty,STATGROUP_TaskGraphTasks);
						FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
							FSimpleDelegateGraphTask::FDelegate::CreateStatic(&NewObjectIntoMapKeyInGameThread,Property,Container,CDO,Instance),
							GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty),
							nullptr, ENamedThreads::GameThread
						);//
			}}}/////////
		} else {
			for (int32 I=0, N=JKeys.Num()-1; I<=N; I++) {
				const auto &JKey = JKeys[I];
				auto IKey = Helper.AddDefaultValue_Invalid_NeedsRehash();
				if (JKey.IsValid()&&(!JKey->IsNull())) {
					if (!FJsonObjectConverter::JsonValueToUProperty(JKey,Property->KeyProp,Helper.GetKeyPtr(IKey),0,CPF_Transient)) {
						LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Map Key Type <-> Property Map Key Type] mismatch:  %s"),*Property->GetName()));
		}}}} Helper.Rehash();
	} else {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Map Member <-> Property Map Member] mismatch:  %s"),*Property->GetName()));}
	//
	if (JSON->HasField(Field.Name.ToString()+TEXT("_VALUE"))) {
		auto JValues = JSON->GetArrayField(Field.Name.ToString()+TEXT("_VALUE"));
		if ((Property->KeyProp->IsA(UObjectProperty::StaticClass()))&&(Property->ValueProp->IsA(UObjectProperty::StaticClass()))) {
			TMap<UObject*,UObject*>&_REF = *Property->ContainerPtrToValuePtr<TMap<UObject*,UObject*>>(Container);
			for (int32 I=0, N=JValues.Num()-1; I<=N; I++) {
				const auto &JValue = JValues[I];
				const auto &JKey = JKeys[I];
				if (JValue.IsValid()&&(!JValue->IsNull())) {
					FString Class, Instance, Key;
					JKey->AsString().Split(TEXT("::"),nullptr,&Key);
					JValue->AsString().Split(TEXT("::"),&Class,&Instance);
					//
					const FSoftObjectPath ClassPath(Class);
					TSoftObjectPtr<UObject>ClassPtr(ClassPath);
					//
					TArray<UObject*>Keys;
					UObject* KeyOBJ = nullptr;
					_REF.GenerateKeyArray(Keys);
					//
					for (auto OBJ : Keys) {
						if (MakeObjectDBID(OBJ)==Key) {KeyOBJ=OBJ; break;}
					} if (KeyOBJ==nullptr) {continue;}
					//
					auto CDO = ClassPtr.LoadSynchronous();
					if (IsInGameThread()&&(CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
						for (TActorIterator<AActor>Actor(Container->GetWorld()); Actor; ++Actor) {
							if (MakeActorDBID(*Actor)==Instance) {_REF.Emplace(KeyOBJ,*Actor); break;}
						}///
					} else if ((CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
						DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUMapProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty,STATGROUP_TaskGraphTasks);
						FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
							FSimpleDelegateGraphTask::FDelegate::CreateStatic(&RestoreActorReferenceIntoMapValueInGameThread,Property,Container,KeyOBJ,Instance),
							GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty),
							nullptr, ENamedThreads::GameThread
						);//
					} else if (IsInGameThread()&&(CDO!=nullptr)) {
						bool IsSet = false;
						for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
							if (MakeObjectDBID(*OBJ)==Instance) {_REF.Emplace(KeyOBJ,*OBJ); IsSet=true; break;}
						} if (!IsSet) {_REF.Emplace(KeyOBJ,NewObject<UObject>(Container,CDO->GetClass(),NAME_None,RF_NoFlags,CDO));}
					} else if (CDO!=nullptr) {
						DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUMapProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty,STATGROUP_TaskGraphTasks);
						FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
							FSimpleDelegateGraphTask::FDelegate::CreateStatic(&NewObjectIntoMapValueInGameThread,Property,Container,KeyOBJ,CDO,Instance),
							GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty),
							nullptr, ENamedThreads::GameThread
						);//
			}}}/////////
		} else if ((Property->KeyProp->IsA(UNameProperty::StaticClass()))&&(Property->ValueProp->IsA(UObjectProperty::StaticClass()))) {
			TMap<FName,UObject*>&_REF = *Property->ContainerPtrToValuePtr<TMap<FName,UObject*>>(Container);
			for (int32 I=0, N=JValues.Num()-1; I<=N; I++) {
				const auto &JValue = JValues[I];
				const auto &JKey = JKeys[I];
				if (JValue.IsValid()&&(!JValue->IsNull())) {
					FString Class, Instance;
					FName Key = *JKey->AsString();
					JValue->AsString().Split(TEXT("::"),&Class,&Instance);
					//
					const FSoftObjectPath ClassPath(Class);
					TSoftObjectPtr<UObject>ClassPtr(ClassPath);
					//
					auto CDO = ClassPtr.LoadSynchronous();
					if (IsInGameThread()&&(CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
						for (TActorIterator<AActor>Actor(Container->GetWorld()); Actor; ++Actor) {
							if (MakeActorDBID(*Actor)==Instance) {_REF.Emplace(Key,*Actor); break;}
						}///
					} else if ((CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
						DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUMapProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty,STATGROUP_TaskGraphTasks);
						FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
							FSimpleDelegateGraphTask::FDelegate::CreateStatic(&RestoreActorReferenceIntoNamedMapValueInGameThread,Property,Container,Key,Instance),
							GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty),
							nullptr, ENamedThreads::GameThread
						);//
					} else if (IsInGameThread()&&(CDO!=nullptr)) {
						bool IsSet = false;
						for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
							if (MakeObjectDBID(*OBJ)==Instance) {_REF.Emplace(Key,*OBJ); IsSet=true; break;}
						} if (!IsSet) {_REF.Emplace(Key,NewObject<UObject>(Container,CDO->GetClass(),NAME_None,RF_NoFlags,CDO));}
					} else if (CDO!=nullptr) {
						DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUMapProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty,STATGROUP_TaskGraphTasks);
						FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
							FSimpleDelegateGraphTask::FDelegate::CreateStatic(&NewObjectIntoNamedMapValueInGameThread,Property,Container,Key,CDO,Instance),
							GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty),
							nullptr, ENamedThreads::GameThread
						);//
			}}}/////////
		} else if (Property->ValueProp->IsA(UObjectProperty::StaticClass())) {
			LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Map Value Type <-> Property Map Value Type] mismatch:  %s"),*Property->GetName()));
			LOG_DB(Logs,ESQLSeverity::Error,TEXT("Only  TMap<UObject*,UObject*>  OR  TMap<FName,UObject*>  are supported for Maps of Object References!"));
		} else {
			for (int32 I=0, N=JValues.Num()-1; I<=N; I++) {
				const auto &JValue = JValues[I];
				if (JValue.IsValid()&&(!JValue->IsNull())) {
					if (!FJsonObjectConverter::JsonValueToUProperty(JValue,Property->ValueProp,Helper.GetValuePtr(I),0,CPF_Transient)) {
						LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Map Value Type <-> Property Map Value Type] mismatch:  %s"),*Property->GetName()));
	}}}}} else {LOG_DB(Logs,ESQLSeverity::Warning,FString::Printf(TEXT("[Data Map Member <-> Property Map Member] mismatch:  %s"),*Property->GetName()));}
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, UStructProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(Field.Text);
	if (Field.Cast!=ESQLReadMode::Integer) {
		if (!FJsonSerializer::Deserialize(JReader,JSON)||!JSON.IsValid()) {
			LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object String for : [%s]"),*Property->GetName()));
	return;}}
	//
	switch (Field.Cast) {
		case ESQLReadMode::Integer:
		{
			auto StructPtr = Property->ContainerPtrToValuePtr<FDateTime>(Container);
			if (StructPtr) {(*StructPtr)=FDateTime::FromUnixTimestamp(Field.Integer);}
		}	break;
		//
		case ESQLReadMode::Color:
		{
			if (Property->Struct==TBaseStructure<FColor>::Get()) {
				auto JColor = JSON->GetObjectField(Field.Name.ToString());
				FColor Color; if (JColor.IsValid()) {
					Color.R = JColor->GetIntegerField(TEXT("r")); Color.G = JColor->GetIntegerField(TEXT("g"));
					Color.B = JColor->GetIntegerField(TEXT("b")); Color.A = JColor->GetIntegerField(TEXT("a"));
				}///
				auto StructPtr = Property->ContainerPtrToValuePtr<FColor>(Container);
				if (StructPtr) {(*StructPtr)=Color;}
			} else if (Property->Struct==TBaseStructure<FLinearColor>::Get()) {
				auto JColor = JSON->GetObjectField(Field.Name.ToString());
				FLinearColor Color; if (JColor.IsValid()) {
					Color.R = JColor->GetNumberField(TEXT("r")); Color.G = JColor->GetNumberField(TEXT("g"));
					Color.B = JColor->GetNumberField(TEXT("b")); Color.A = JColor->GetNumberField(TEXT("a"));
				}///
				auto StructPtr = Property->ContainerPtrToValuePtr<FLinearColor>(Container);
				if (StructPtr) {(*StructPtr)=Color;}
			} else {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Column Type <-> Property Type] mismatch:  %s"),*Property->GetName()));}
		}	break;
		//
		case ESQLReadMode::Vector2D:
		{
			if (Property->Struct!=TBaseStructure<FVector2D>::Get()) {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Column Type <-> Property Type] mismatch:  %s"),*Property->GetName())); break;}
			auto JVector = JSON->GetObjectField(Field.Name.ToString());
			FVector2D Vector; if (JVector.IsValid()) {
				Vector.X = JVector->GetNumberField(TEXT("x"));
				Vector.Y = JVector->GetNumberField(TEXT("y"));
			}///
			auto StructPtr = Property->ContainerPtrToValuePtr<FVector2D>(Container);
			if (StructPtr) {(*StructPtr)=Vector;}
		}	break;
		//
		case ESQLReadMode::Vector3D:
		{
			if (Property->Struct!=TBaseStructure<FVector>::Get()) {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Column Type <-> Property Type] mismatch:  %s"),*Property->GetName())); break;}
			auto JVector = JSON->GetObjectField(Field.Name.ToString());
			FVector Vector; if (JVector.IsValid()) {
				Vector.X = JVector->GetNumberField(TEXT("x"));
				Vector.Y = JVector->GetNumberField(TEXT("y"));
				Vector.Z = JVector->GetNumberField(TEXT("z"));
			}///
			auto StructPtr = Property->ContainerPtrToValuePtr<FVector>(Container);
			if (StructPtr) {(*StructPtr)=Vector;}
		}	break;
		//
		case ESQLReadMode::Rotator:
		{
			if (Property->Struct!=TBaseStructure<FRotator>::Get()) {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Column Type <-> Property Type] mismatch:  %s"),*Property->GetName())); break;}
			auto JRotator = JSON->GetObjectField(Field.Name.ToString());
			FRotator Rotator; if (JRotator.IsValid()) {
				Rotator.Pitch = JRotator->GetNumberField(TEXT("pitch"));
				Rotator.Roll = JRotator->GetNumberField(TEXT("roll"));
				Rotator.Yaw = JRotator->GetNumberField(TEXT("yaw"));
			}///
			auto StructPtr = Property->ContainerPtrToValuePtr<FRotator>(Container);
			if (StructPtr) {(*StructPtr)=Rotator;}
		}	break;
		//
		case ESQLReadMode::FloatCurve:
		{
			if (Property->Struct!=FRuntimeFloatCurve::StaticStruct()) {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Column Type <-> Property Type] mismatch:  %s"),*Property->GetName())); break;}
			FRuntimeFloatCurve FloatCurve; FRichCurve CurveData;
			FJsonObjectConverter::JsonObjectToUStruct<FRichCurve>(JSON.ToSharedRef(),&CurveData);
			auto StructPtr = Property->ContainerPtrToValuePtr<FRuntimeFloatCurve>(Container);
			if (StructPtr) {FloatCurve.ExternalCurve = nullptr; FloatCurve.EditorCurveData = CurveData; (*StructPtr)=FloatCurve;}
		}	break;
		//
		case ESQLReadMode::Struct:
		{
			void* StructPtr = Property->ContainerPtrToValuePtr<void>(Container);
			if (StructPtr) {ParseJSONtoStruct(JSON,Property,StructPtr,Container);}
			else {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Column Type <-> Property Type] mismatch:  %s"),*Property->GetName()));}
		}	break;
	default: break;}
}

void USQLReflector::SQLToUProperty(FSQLField &Field, UObjectProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	switch (Field.Cast) {
		case ESQLReadMode::ObjectPtr:
		{
			auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
			//
			FString Class, Instance;
			Field.Text.Split(TEXT("::"),&Class,&Instance);
			//
			const FSoftObjectPath ClassPath(Class);
			TSoftObjectPtr<UObject>ClassPtr(ClassPath);
			//
			auto CDO = ClassPtr.LoadSynchronous();
			if (IsInGameThread()&&(CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
				for (TActorIterator<AActor>Actor(Container->GetWorld()); Actor; ++Actor) {
					if (MakeActorDBID(*Actor)==Instance) {Property->SetPropertyValue(ValuePtr,(*Actor)); break;}
				}///
			} else if ((CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
				DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUProperty,STATGROUP_TaskGraphTasks);
				FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
					FSimpleDelegateGraphTask::FDelegate::CreateStatic(&RestoreActorReferenceInGameThread,Property,Container,Instance),
					GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUProperty),
					nullptr, ENamedThreads::GameThread
				);//
			} else if (IsInGameThread()&&(CDO!=nullptr)) {
				bool IsSet = false;
				for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
					if (MakeObjectDBID(*OBJ)==Instance) {Property->SetPropertyValue(ValuePtr,(*OBJ)); IsSet=true; break;}
				} if (!IsSet) {Property->SetPropertyValue(ValuePtr,NewObject<UObject>(Container,CDO->GetClass(),NAME_None,RF_NoFlags,CDO));}
			} else if (CDO!=nullptr) {
				DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUProperty,STATGROUP_TaskGraphTasks);
				FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
					FSimpleDelegateGraphTask::FDelegate::CreateStatic(&NewObjectReferenceInGameThread,Property,Container,CDO,Instance),
					GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUProperty),
					nullptr, ENamedThreads::GameThread
				);//
			}///
		}	break;
	default: break;}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Struct Parser:

TSharedPtr<FJsonObject> USQLReflector::ParseStructToJSON(FString &Output, UStructProperty* Property, const void* StructPtr) {
	TSharedRef<TJsonWriter<>>Buffer = TJsonWriterFactory<>::Create(&Output);
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	check(Property); check(StructPtr);
	//
	UScriptStruct* Struct = Property->Struct;
	for (TFieldIterator<UProperty>IT(Struct); IT; ++IT) {
		UProperty* Field = *IT; 
		for (int32 I=0; I<Field->ArrayDim; ++I) {
			auto ValuePtr = Field->ContainerPtrToValuePtr<void>(StructPtr,I);
			if (!ValuePtr) {continue;} ParsePropertyToJSON(JSON,Field,ValuePtr,0,CPF_Transient);
		}///
	}///
	//
	FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer); return JSON;
}

void USQLReflector::ParseStructToJSON(TSharedPtr<FJsonObject> &JSON, UStructProperty* Property, const void* StructPtr) {
	UScriptStruct* Struct = Property->Struct;
	check(Property); check(StructPtr);
	//
	for (TFieldIterator<UProperty>IT(Struct); IT; ++IT) {
		UProperty* Field = *IT; 
		for (int32 I=0; I<Field->ArrayDim; ++I) {
			auto ValuePtr = Field->ContainerPtrToValuePtr<void>(StructPtr,I);
			if (!ValuePtr) {continue;} ParsePropertyToJSON(JSON,Field,ValuePtr,0,CPF_Transient);
		}///
	}///
}

TSharedPtr<FJsonObject> USQLReflector::ParseStructToJSON(UStructProperty* Property, const void* StructPtr) {
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	UScriptStruct* Struct = Property->Struct;
	check(Property); check(StructPtr);
	//
	for (TFieldIterator<UProperty>IT(Struct); IT; ++IT) {
		UProperty* Field = *IT; 
		for (int32 I=0; I<Field->ArrayDim; ++I) {
			auto ValuePtr = Field->ContainerPtrToValuePtr<void>(StructPtr,I);
			if (!ValuePtr) {continue;} ParsePropertyToJSON(JSON,Field,ValuePtr,0,CPF_Transient);
		}///
	}///
	//
	return JSON;
}

void USQLReflector::ParsePropertyToJSON(TSharedPtr<FJsonObject> &JSON, UProperty* Property, const void* ValuePtr, int64 CheckFlags, int64 SkipFlags) {
	check(Property); check(ValuePtr);
	TSharedPtr<FJsonValue>JValue;
	//
	if (UBoolProperty* _Bool = Cast<UBoolProperty>(Property)) {
		JValue = FJsonObjectConverter::UPropertyToJsonValue(_Bool,ValuePtr,CheckFlags,SkipFlags);
		if (JValue.IsValid()&&(!JValue->IsNull())) {JSON->SetField(_Bool->GetName(),JValue);}
	}///
	//
	if (UEnumProperty* _Enum = Cast<UEnumProperty>(Property)) {
		UEnum* EnumPtr = _Enum->GetEnum();
		FString Value = EnumPtr->GetNameStringByValue(_Enum->GetUnderlyingProperty()->GetUnsignedIntPropertyValue(ValuePtr));
		JSON->SetStringField(_Enum->GetName(),Value);
	}///
	//
	if (UNumericProperty* _Numeric = Cast<UNumericProperty>(Property)) {
		JValue = FJsonObjectConverter::UPropertyToJsonValue(_Numeric,ValuePtr,CheckFlags,SkipFlags);
		if (JValue.IsValid()&&(!JValue->IsNull())) {JSON->SetField(_Numeric->GetName(),JValue);}
	}///
	//
	if (UNameProperty* _Name = Cast<UNameProperty>(Property)) {
		JValue = FJsonObjectConverter::UPropertyToJsonValue(_Name,ValuePtr,CheckFlags,SkipFlags);
		if (JValue.IsValid()&&(!JValue->IsNull())) {JSON->SetField(_Name->GetName(),JValue);}
	}///
	//
	if (UStrProperty* _String = Cast<UStrProperty>(Property)) {
		JValue = FJsonObjectConverter::UPropertyToJsonValue(_String,ValuePtr,CheckFlags,SkipFlags);
		if (JValue.IsValid()&&(!JValue->IsNull())) {JSON->SetField(_String->GetName(),JValue);}
	}///
	//
	if (UTextProperty* _Text = Cast<UTextProperty>(Property)) {
		JValue = FJsonObjectConverter::UPropertyToJsonValue(_Text,ValuePtr,CheckFlags,SkipFlags);
		if (JValue.IsValid()&&(!JValue->IsNull())) {JSON->SetField(_Text->GetName(),JValue);}
	}///
	//
	if (UArrayProperty* _Array = Cast<UArrayProperty>(Property)) {
		TArray<TSharedPtr<FJsonValue>>Out;
		FScriptArrayHelper Helper(_Array,ValuePtr);
		if (_Array->Inner->IsA(UObjectProperty::StaticClass())) {
			LOG_DB(Logs,ESQLSeverity::Error,TEXT("Array of Object References, when member of a Struct Property, is not supported!"));
		} else {
			for (int32 I=0, N=Helper.Num()-1; I<=N; I++) {
				auto Value = FJsonObjectConverter::UPropertyToJsonValue(_Array->Inner,Helper.GetRawPtr(I),CheckFlags,SkipFlags);
				if (Value.IsValid()&&(!Value->IsNull())) {Out.Push(Value);}
	}} if (Out.Num()>0) {JSON->SetArrayField(_Array->GetName(),Out);}}
	//
	if (USetProperty* _Set = Cast<USetProperty>(Property)) {
		TArray<TSharedPtr<FJsonValue>>Out;
		FScriptSetHelper Helper(_Set,ValuePtr);
		if (_Set->ElementProp->IsA(UObjectProperty::StaticClass())) {
			LOG_DB(Logs,ESQLSeverity::Error,TEXT("Set of Object References, when member of a Struct Property, is not supported!"));
		} else {
			for (int32 I=0, N=Helper.Num()-1; I<=N; I++) {
				auto Value = FJsonObjectConverter::UPropertyToJsonValue(_Set->ElementProp,Helper.GetElementPtr(I),CheckFlags,SkipFlags);
				if (Value.IsValid()&&(!Value->IsNull())) {Out.Push(Value);}
	}} if (Out.Num()>0) {JSON->SetArrayField(_Set->GetName(),Out);}}
	//
	if (UMapProperty* _Map = Cast<UMapProperty>(Property)) {
		TArray<TSharedPtr<FJsonValue>>Keys;
		TArray<TSharedPtr<FJsonValue>>Values;
		FScriptMapHelper Helper(_Map,ValuePtr);
		//
		if (_Map->KeyProp->IsA(UObjectProperty::StaticClass())) {
			LOG_DB(Logs,ESQLSeverity::Error,TEXT("Map Keys of Object References, when member of a Struct Property, are not supported!"));
		} else {
			for (int32 I=0, N=Helper.Num()-1; I<=N; I++) {
				auto Key = FJsonObjectConverter::UPropertyToJsonValue(_Map->KeyProp,Helper.GetKeyPtr(I),CheckFlags,SkipFlags);
				if (Key.IsValid()&&(!Key->IsNull())) {Keys.Push(Key);}
			}///
		}///
		if (_Map->ValueProp->IsA(UObjectProperty::StaticClass())) {
			LOG_DB(Logs,ESQLSeverity::Error,TEXT("Map Values of Object References, when member of a Struct Property, are not supported!"));
		} else {
			for (int32 I=0, N=Helper.Num()-1; I<=N; I++) {
				auto Value = FJsonObjectConverter::UPropertyToJsonValue(_Map->ValueProp,Helper.GetValuePtr(I),CheckFlags,SkipFlags);
				if (Value.IsValid()&&(!Value->IsNull())) {Values.Push(Value);}
			}///
		}///
		if ((Keys.Num()>0)&&(Values.Num()>0)) {
			JSON->SetArrayField(_Map->GetName()+TEXT("_KEY"),Keys);
			JSON->SetArrayField(_Map->GetName()+TEXT("_VALUE"),Values);
		}///
	}///
	//
	if (UStructProperty* _Struct = Cast<UStructProperty>(Property)) {
		JSON->SetObjectField(_Struct->GetName(),ParseStructToJSON(_Struct,ValuePtr));
	}///
	//
	if (UObjectProperty* _Object = Cast<UObjectProperty>(Property)) {
		if (_Object->GetPropertyValue(ValuePtr)->IsValidLowLevelFast()) {
			auto FullName = MakeObjectDBID(_Object->GetPropertyValue(ValuePtr));
			auto ClassPath = _Object->GetPropertyValue(ValuePtr)->GetClass()->GetDefaultObject()->GetPathName();
			JSON->SetStringField(_Object->GetName(),ClassPath+TEXT("::")+FullName);
		}///
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Struct Member Parser:

void USQLReflector::ParseJSONtoStruct(TSharedPtr<FJsonObject> &JSON, UStructProperty* Property, void* StructPtr, UObject* Container) {
	UScriptStruct* Struct = Property->Struct;
	check(Property); check(StructPtr);
	//
	for (TFieldIterator<UProperty>IT(Struct); IT; ++IT) {
		UProperty* Field = *IT; 
		for (int32 I=0; I<Field->ArrayDim; ++I) {
			void* ValuePtr = Field->ContainerPtrToValuePtr<void>(StructPtr,I);
			if (ValuePtr) {ParseJSONtoProperty(JSON,Field,ValuePtr,Container,0,CPF_Transient);}
		}///
	}///
}

void USQLReflector::ParseJSONtoProperty(TSharedPtr<FJsonObject> &JSON, UProperty* Property, void* ValuePtr, UObject* Container, int64 CheckFlags, int64 SkipFlags) {
	check(Property); check(ValuePtr);
	//
	if (UBoolProperty* _Bool = Cast<UBoolProperty>(Property)) {
		const auto Field = _Bool->GetName();
		if (JSON->HasField(Field)) {
			_Bool->SetPropertyValue(ValuePtr,JSON->GetBoolField(Field));
		}///
	}///
	//
	if (UEnumProperty* _Enum = Cast<UEnumProperty>(Property)) {
		UEnum* EnumPtr = _Enum->GetEnum();
		const auto Field = _Enum->GetName();
		if ((EnumPtr)&&JSON->HasField(Field)) {
			auto Value = EnumPtr->GetValueByName(*JSON->GetStringField(Field));
			_Enum->GetUnderlyingProperty()->SetIntPropertyValue(ValuePtr,Value);
		}///
	}///
	//
	if (UByteProperty* _Byte = Cast<UByteProperty>(Property)) {
		const auto Field = _Byte->GetName();
		if (JSON->HasField(Field)) {
			_Byte->SetIntPropertyValue(ValuePtr,(uint64)JSON->GetIntegerField(Field));
		}///
	}///
	//
	if (UIntProperty* _Int = Cast<UIntProperty>(Property)) {
		const auto Field = _Int->GetName();
		if (JSON->HasField(Field)) {
			_Int->SetIntPropertyValue(ValuePtr,(int64)JSON->GetIntegerField(Field));
		}///
	}///
	//
	if (UFloatProperty* _Float = Cast<UFloatProperty>(Property)) {
		const auto Field = _Float->GetName();
		if (JSON->HasField(Field)) {
			_Float->SetFloatingPointPropertyValue(ValuePtr,JSON->GetNumberField(Field));
		}///
	}///
	//
	if (UNameProperty* _Name = Cast<UNameProperty>(Property)) {
		const auto Field = _Name->GetName();
		if (JSON->HasField(Field)) {
			_Name->SetPropertyValue(ValuePtr,*JSON->GetStringField(Field));
		}///
	}///
	//
	if (UStrProperty* _String = Cast<UStrProperty>(Property)) {
		const auto Field = _String->GetName();
		if (JSON->HasField(Field)) {
			_String->SetPropertyValue(ValuePtr,JSON->GetStringField(Field));
		}///
	}///
	//
	if (UTextProperty* _Text = Cast<UTextProperty>(Property)) {
		const auto Field = _Text->GetName();
		if (JSON->HasField(Field)) {
			_Text->SetPropertyValue(ValuePtr,FText::FromString(JSON->GetStringField(Field)));
		}///
	}///
	//
	if (UArrayProperty* _Array = Cast<UArrayProperty>(Property)) {
		const auto Field = _Array->GetName();
		if (JSON->HasField(Field)) {
			if (_Array->Inner->IsA(UObjectProperty::StaticClass())) {
				LOG_DB(Logs,ESQLSeverity::Error,TEXT("Array of Object References, when member of a Struct Property, is not supported!"));
			} else {
				auto JArray = JSON->GetArrayField(Field);
				FScriptArrayHelper Helper(_Array,ValuePtr);
				Helper.Resize(JArray.Num());
				for (int32 I=0, N=JArray.Num()-1; I<=N; I++) {
					const auto &JValue = JArray[I];
					if (JValue.IsValid()&&(!JValue->IsNull())) {
						if (!FJsonObjectConverter::JsonValueToUProperty(JValue,_Array->Inner,Helper.GetRawPtr(I),0,SkipFlags)) {
							LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Array Element Type <-> Property Array Element Type] mismatch:  %s"),*Property->GetName()));
		}}}}}///////////////
	}///
	//
	if (USetProperty* _Set = Cast<USetProperty>(Property)) {
		const auto Field = _Set->GetName();
		if (JSON->HasField(Field)) {
			if (_Set->ElementProp->IsA(UObjectProperty::StaticClass())) {
				LOG_DB(Logs,ESQLSeverity::Error,TEXT("Set of Object References, when member of a Struct Property, is not supported!"));
			} else {
				auto JSet = JSON->GetArrayField(Field);
				FScriptSetHelper Helper(_Set,ValuePtr);
				Helper.EmptyElements();
				for (int32 I=0, N=JSet.Num()-1; I<=N; I++) {
					const auto &JValue = JSet[I];
					auto Index = Helper.AddDefaultValue_Invalid_NeedsRehash();
					if (JValue.IsValid()&&(!JValue->IsNull())) {
						if (!FJsonObjectConverter::JsonValueToUProperty(JValue,_Set->ElementProp,Helper.GetElementPtr(Index),0,SkipFlags)) {
							LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Set Element Type <-> Property Set Element Type] mismatch:  %s"),*Property->GetName()));
					}}//////
	} Helper.Rehash();}}}
	//
	if (UMapProperty* _Map = Cast<UMapProperty>(Property)) {
		int32 Num=0;
		const auto Field = _Map->GetName();
		if (JSON->HasField(Field+TEXT("_KEY"))) {
			if (_Map->KeyProp->IsA(UObjectProperty::StaticClass())) {
				LOG_DB(Logs,ESQLSeverity::Error,TEXT("Map Keys of Object References, when member of a Struct Property, are not supported!"));
			} else {
				auto JKeys = JSON->GetArrayField(Field+TEXT("_KEY"));
				FScriptMapHelper Helper(_Map,ValuePtr);
				Helper.EmptyValues(); Num = JKeys.Num();
				for (int32 I=0, N=JKeys.Num()-1; I<=N; I++) {
					const auto &JKey = JKeys[I];
					auto IKey = Helper.AddDefaultValue_Invalid_NeedsRehash();
					if (JKey.IsValid()&&(!JKey->IsNull())) {
						if (!FJsonObjectConverter::JsonValueToUProperty(JKey,_Map->KeyProp,Helper.GetKeyPtr(IKey),0,SkipFlags)) {
							LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Map Key Type <-> Property Map Key Type] mismatch:  %s"),*Property->GetName()));
		}}} Helper.Rehash();}}
		//
		if (JSON->HasField(Field+TEXT("_VALUE"))) {
			if (_Map->ValueProp->IsA(UObjectProperty::StaticClass())) {
				LOG_DB(Logs,ESQLSeverity::Error,TEXT("Map Values of Object References, when member of a Struct Property, are not supported!"));
			} else {
				auto JValues = JSON->GetArrayField(Field+TEXT("_VALUE"));
				FScriptMapHelper Helper(_Map,ValuePtr);
				check(JValues.Num()==Num);
				for (int32 I=0, N=JValues.Num()-1; I<=N; I++) {
					const auto &JValue = JValues[I];
					if (JValue.IsValid()&&(!JValue->IsNull())) {
						if (!FJsonObjectConverter::JsonValueToUProperty(JValue,_Map->ValueProp,Helper.GetValuePtr(I),0,SkipFlags)) {
							LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Map Value Type <-> Property Map Value Type] mismatch:  %s"),*Property->GetName()));
		}}}}}///////////////
	}///
	//
	if (UStructProperty* _Struct = Cast<UStructProperty>(Property)) {
		const auto Field = _Struct->GetName();
		if (JSON->HasField(Field)) {
			auto JStruct = JSON->GetObjectField(Field);
			if (JStruct.IsValid()) {ParseJSONtoStruct(JStruct,_Struct,ValuePtr,Container);}
		}///
	}///
	//
	if (UObjectProperty* _Object = Cast<UObjectProperty>(Property)) {
		const auto Field = _Object->GetName();
		if (JSON->HasField(Field)) {
			FString Class, Instance;
			JSON->GetStringField(Field).Split(TEXT("::"),&Class,&Instance);
			//
			const FSoftObjectPath ClassPath(Class);
			TSoftObjectPtr<UObject>ClassPtr(ClassPath);
			//
			auto CDO = ClassPtr.LoadSynchronous();
			if (IsInGameThread()&&(CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
				for (TActorIterator<AActor>Actor(Container->GetWorld()); Actor; ++Actor) {
					if (MakeActorDBID(*Actor)==Instance) {_Object->SetPropertyValue(ValuePtr,(*Actor)); break;}
				}///
			} else if ((CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
				DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUProperty,STATGROUP_TaskGraphTasks);
				FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
					FSimpleDelegateGraphTask::FDelegate::CreateStatic(&RestoreActorReferenceInGameThread,_Object,Container,Instance),
					GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUProperty),
					nullptr, ENamedThreads::GameThread
				);//
			} else if (IsInGameThread()&&(CDO!=nullptr)) {
				bool IsSet = false;
				for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
					if (MakeObjectDBID(*OBJ)==Instance) {_Object->SetPropertyValue(ValuePtr,(*OBJ)); IsSet=true; break;}
				} if (!IsSet) {_Object->SetPropertyValue(ValuePtr,NewObject<UObject>(Container,CDO->GetClass(),NAME_None,RF_NoFlags,CDO));}
			} else if (CDO!=nullptr) {
				DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUProperty,STATGROUP_TaskGraphTasks);
				FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
					FSimpleDelegateGraphTask::FDelegate::CreateStatic(&NewObjectReferenceInGameThread,_Object,Container,CDO,Instance),
					GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUProperty),
					nullptr, ENamedThreads::GameThread
				);//
			}///
		} else {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Object Struct Member <-> Property Object Struct Member] mismatch:  %s"),*Property->GetName()));}
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const FGuid USQLReflector::FindGUID(UObject* Container) {
	FGuid GUID; GUID.Invalidate();
	//
	if (!Container->IsValidLowLevelFast()) {return GUID;}
	//
	for (TFieldIterator<UProperty>PIT(Container->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
		UProperty* Property = *PIT;
		//
		const bool IsStruct = Property->IsA(UStructProperty::StaticClass());
		if ((IsStruct)&&(CastChecked<UStructProperty>(Property)->Struct==TBaseStructure<FGuid>::Get())
			&&(Property->GetName().Equals(TEXT("SGUID"),ESearchCase::CaseSensitive))) {
			GUID = ParseGUID(CastChecked<UStructProperty>(Property),Container);
		break;}
	}///
	//
	return GUID;
}

const FGuid USQLReflector::FindGUID(const UObject* Container) {
	FGuid GUID; GUID.Invalidate();
	//
	if (!Container->IsValidLowLevelFast()) {return GUID;}
	//
	for (TFieldIterator<UProperty>PIT(Container->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
		UProperty* Property = *PIT;
		//
		const bool IsStruct = Property->IsA(UStructProperty::StaticClass());
		if ((IsStruct)&&(CastChecked<UStructProperty>(Property)->Struct==TBaseStructure<FGuid>::Get())
			&&(Property->GetName().Equals(TEXT("SGUID"),ESearchCase::CaseSensitive))) {
			GUID = ParseGUID(CastChecked<UStructProperty>(Property),Container);
		break;}
	}///
	//
	return GUID;
}

const FString USQLReflector::FindSGUID(UObject* Container) {
	if (!Container->IsValidLowLevelFast()) {
		return TEXT("");
	} FString GUID;
	//
	for (TFieldIterator<UProperty>PIT(Container->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
		UProperty* Property = *PIT;
		//
		const bool IsStruct = Property->IsA(UStructProperty::StaticClass());
		if ((IsStruct)&&(CastChecked<UStructProperty>(Property)->Struct==TBaseStructure<FGuid>::Get())
			&&(Property->GetName().Equals(TEXT("SGUID"),ESearchCase::CaseSensitive))) {
			GUID = ParseSGUID(CastChecked<UStructProperty>(Property),Container);
		break;}
	}///
	//
	return GUID;
}

const FString USQLReflector::FindSGUID(const UObject* Container) {
	if (!Container->IsValidLowLevelFast()) {
		return TEXT("");
	} FString GUID;
	//
	for (TFieldIterator<UProperty>PIT(Container->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
		UProperty* Property = *PIT;
		//
		const bool IsStruct = Property->IsA(UStructProperty::StaticClass());
		if ((IsStruct)&&(CastChecked<UStructProperty>(Property)->Struct==TBaseStructure<FGuid>::Get())
			&&(Property->GetName().Equals(TEXT("SGUID"),ESearchCase::CaseSensitive))) {
			GUID = ParseSGUID(CastChecked<UStructProperty>(Property),Container);
		break;}
	}///
	//
	return GUID;
}

const bool USQLReflector::IsMatchingSGUID(const FGuid &SGUID, UObject* Target) {
	if (!Target->IsValidLowLevelFast()) {return false;}
	FGuid OID; OID.Invalidate();
	//
	for (TFieldIterator<UProperty>PIT(Target->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
		UProperty* Property = *PIT;
		//
		const bool IsStruct = Property->IsA(UStructProperty::StaticClass());
		if ((IsStruct)&&(CastChecked<UStructProperty>(Property)->Struct==TBaseStructure<FGuid>::Get())
			&&(Property->GetName().Equals(TEXT("SGUID"),ESearchCase::CaseSensitive))) {
			OID = ParseGUID(CastChecked<UStructProperty>(Property),Target);
		break;}
	}///
	//
	return (SGUID==OID);
}

const bool USQLReflector::IsMatchingSGUID(UObject* Target, UObject* ComparedTo) {
	if (!Target->IsValidLowLevelFast()||!ComparedTo->IsValidLowLevelFast()) {return false;}
	FGuid GUID; GUID.Invalidate();
	FGuid OID; OID.Invalidate();
	//
	for (TFieldIterator<UProperty>PIT(Target->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
		UProperty* Property = *PIT;
		//
		const bool IsStruct = Property->IsA(UStructProperty::StaticClass());
		if ((IsStruct)&&(CastChecked<UStructProperty>(Property)->Struct==TBaseStructure<FGuid>::Get())
			&&(Property->GetName().Equals(TEXT("SGUID"),ESearchCase::CaseSensitive))) {
			GUID = ParseGUID(CastChecked<UStructProperty>(Property),Target);
		break;}
	}///
	//
	for (TFieldIterator<UProperty>PIT(ComparedTo->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
		UProperty* Property = *PIT;
		//
		const bool IsStruct = Property->IsA(UStructProperty::StaticClass());
		if ((IsStruct)&&(CastChecked<UStructProperty>(Property)->Struct==TBaseStructure<FGuid>::Get())
			&&(Property->GetName().Equals(TEXT("SGUID"),ESearchCase::CaseSensitive))) {
			OID = ParseGUID(CastChecked<UStructProperty>(Property),ComparedTo);
		break;}
	}///
	//
	return (GUID==OID);
}

const FGuid USQLReflector::ParseGUID(UStructProperty* Property, const UObject* Container) {
	check(Property); check(Container);
	//
	FGuid GUID; GUID.Invalidate();
	UScriptStruct* Struct = Property->Struct;
	if (Struct!=TBaseStructure<FGuid>::Get()) {return GUID;}
	//
	const FGuid* StructPtr = Property->ContainerPtrToValuePtr<FGuid>(Container);
	const auto Value = FJsonObjectConverter::UPropertyToJsonValue(CastChecked<UProperty>(Property),StructPtr,0,0);
	//
	FGuid::Parse(Value->AsString(),GUID);
	//
	return GUID;
}

const FString USQLReflector::ParseSGUID(UStructProperty* Property, const UObject* Container) {
	check(Property); check(Container);
	//
	FGuid GUID; GUID.Invalidate();
	UScriptStruct* Struct = Property->Struct;
	if (Struct!=TBaseStructure<FGuid>::Get()) {return FString();}
	//
	const FGuid* StructPtr = Property->ContainerPtrToValuePtr<FGuid>(Container);
	const auto Value = FJsonObjectConverter::UPropertyToJsonValue(CastChecked<UProperty>(Property),StructPtr,0,0);
	//
	if (FGuid::Parse(Value->AsString(),GUID)) {
		if (GUID.IsValid()) {return GUID.ToString();}
	}///
	//
	return FString();
}

void USQLReflector::SetGUID(const FGuid &GUID, UObject* Container) {
	check(Container);
	//
	for (TFieldIterator<UProperty>PIT(Container->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
		UProperty* Property = *PIT;
		//
		const bool IsStruct = Property->IsA(UStructProperty::StaticClass());
		if ((IsStruct)&&(CastChecked<UStructProperty>(Property)->Struct==TBaseStructure<FGuid>::Get())
			&&(Property->GetName().Equals(TEXT("SGUID"),ESearchCase::CaseSensitive))) {
			auto OID = Property->ContainerPtrToValuePtr<FGuid>(Container);
			if (OID!=nullptr) {(*OID)=GUID;}
		break;}
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const FString USQLReflector::FindDBID(UObject* Container) {
	if (!Container->IsValidLowLevelFast()) {
		return TEXT("");
	}///
	//
	FString RAW, ID;
	for (TFieldIterator<UProperty>PIT(Container->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
		UProperty* Property = *PIT;
		//
		const bool IsName = Property->IsA(UNameProperty::StaticClass());
		if ((IsName)&&(Property->GetName().Equals(TEXT("DBID"),ESearchCase::CaseSensitive))) {
			const FName* ValuePtr = Property->ContainerPtrToValuePtr<FName>(Container);
			RAW = (CastChecked<UNameProperty>(Property)->GetPropertyValue(ValuePtr)).ToString();
		break;}
	}///
	//
	for (const TCHAR &CH : RAW) {
		if (TChar<WIDECHAR>::IsAlpha(CH)||TChar<WIDECHAR>::IsDigit(CH)||CH==TEXT('_')||CH==TEXT('.')) {ID.AppendChar(CH);}
	} return ID;
}

const FString USQLReflector::FindDBID(const UObject* Container) {
	if (!Container->IsValidLowLevelFast()) {
		return TEXT("");
	}///
	//
	FString RAW, ID;
	for (TFieldIterator<UProperty>PIT(Container->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
		UProperty* Property = *PIT;
		//
		const bool IsName = Property->IsA(UNameProperty::StaticClass());
		if ((IsName)&&(Property->GetName().Equals(TEXT("DBID"),ESearchCase::CaseSensitive))) {
			const FName* ValuePtr = Property->ContainerPtrToValuePtr<FName>(Container);
			RAW = (CastChecked<UNameProperty>(Property)->GetPropertyValue(ValuePtr)).ToString();
		break;}
	}///
	//
	for (const TCHAR &CH : RAW) {
		if (TChar<WIDECHAR>::IsAlpha(CH)||TChar<WIDECHAR>::IsDigit(CH)||CH==TEXT('_')||CH==TEXT('.')) {ID.AppendChar(CH);}
	} return ID;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DB ID Interface:

///	(C) 2018 - Bruno Xavier B. Leite
bool USQLReflector::GetPlayerNetworkID(const APlayerController* PlayerController, FString &ID, const bool AppendPort) {
	if (!PlayerController) {return false;}
	if (!PlayerController->PlayerState->UniqueId.IsValid()) {return false;}
	if (!PlayerController->PlayerState->IsValidLowLevelFast()) {return false;}
#if WITH_SERVER_CODE
	ID = PlayerController->PlayerState->UniqueId->ToString();
	if ((!ID.IsEmpty())&&(!AppendPort)) {ID.Split(FString(":"),&ID,nullptr,ESearchCase::IgnoreCase,ESearchDir::FromEnd);}
#endif
	return false;
}

///	(C) 2018 - Bruno Xavier B. Leite
FString USQLReflector::MakeObjectDBID(UObject* OBJ) {
#if WITH_SERVER_CODE
	if (!OBJ||!OBJ->IsValidLowLevelFast()||OBJ->IsPendingKill()) {return TEXT("ERROR:ID");}
	//
	FString DBID = FindDBID(OBJ);
	if (!DBID.IsEmpty()) {return DBID;}
	//
	FString OwnerID;
	const auto Owner = OBJ->GetTypedOuter<AActor>();
	//
	if (Owner) {
		FString OwnerGUID = FindSGUID(Owner);
		if (!Owner->GetName().Split(TEXT("_C_"),&OwnerID,nullptr,ESearchCase::CaseSensitive,ESearchDir::FromEnd)) {
			OwnerID = Owner->GetFName().ToString();
		}///
		//
		if (!OwnerGUID.IsEmpty()) {
			OwnerID.Append(TEXT("_"));
			OwnerID.Append(OwnerGUID);
		}///
	}///
	//
	FString ID;
	FString NetworkID;
	FString GUID = FindSGUID(OBJ);
	//
	if (!OBJ->GetName().Split(TEXT("_C_"),&ID,nullptr,ESearchCase::CaseSensitive,ESearchDir::FromEnd)) {
		ID = OBJ->GetFName().ToString();
	} if (!GUID.IsEmpty()) {
		ID.Append(TEXT("_"));
		ID.Append(GUID);
	} if (!OwnerID.IsEmpty()) {ID=(OwnerID+FString(TEXT("_"))+ID);}
	//
	if (Owner && Owner->GetNetOwningPlayer()) {
		if (!GetPlayerNetworkID(Owner->GetNetOwningPlayer()->GetPlayerController(Owner->GetWorld()),NetworkID,false)) {
			if (Owner->GetNetOwningPlayer()->GetPlayerController(Owner->GetWorld())) {
				ID = FString(TEXT("P"))+FString::FromInt(UGameplayStatics::GetPlayerControllerID(Owner->GetNetOwningPlayer()->GetPlayerController(Owner->GetWorld())))+(FString(TEXT("_"))+ID);
		}} else {ID=(NetworkID+FString(TEXT("_"))+ID);}
	} return *ID;
#endif
	return FString();
}

///	(C) 2018 - Bruno Xavier B. Leite
FString USQLReflector::MakeActorDBID(AActor* Actor) {
#if WITH_SERVER_CODE
	if (!Actor||!Actor->IsValidLowLevelFast()||Actor->IsPendingKill()) {return TEXT("ERROR:ID");}
	//
	FString DBID = FindDBID(Actor);
	if (!DBID.IsEmpty()) {return DBID;}
	//
	FString ID;
	FString NetworkID;
	FString GUID = FindSGUID(Actor);
	//
	if (!Actor->GetName().Split(TEXT("_C_"),&ID,nullptr,ESearchCase::CaseSensitive,ESearchDir::FromEnd)) {
		ID = Actor->GetFName().ToString();
	} if (!GUID.IsEmpty()) {
		ID.Append(TEXT("_"));
		ID.Append(GUID);
	}///
	//
	if (Actor->GetNetOwningPlayer()) {
		if (!GetPlayerNetworkID(Actor->GetNetOwningPlayer()->GetPlayerController(Actor->GetWorld()),NetworkID,false)) {
			if (Actor->GetNetOwningPlayer()->GetPlayerController(Actor->GetWorld())) {
				ID = FString(TEXT("P"))+FString::FromInt(UGameplayStatics::GetPlayerControllerID(Actor->GetNetOwningPlayer()->GetPlayerController(Actor->GetWorld())))+(FString(TEXT("_"))+ID);
		}} else {ID=(NetworkID+FString(TEXT("_"))+ID);}
	} return *ID;
#endif
	return FString();
}

///	(C) 2018 - Bruno Xavier B. Leite
FString USQLReflector::MakeComponentDBID(UActorComponent* CMP) {
#if WITH_SERVER_CODE
	if (!CMP||!CMP->IsValidLowLevelFast()||CMP->IsPendingKill()) {return TEXT("ERROR:ID");}
	//
	FString DBID = FindDBID(CMP);
	if (!DBID.IsEmpty()) {return DBID;}
	//
	FString OwnerID;
	const auto &Owner = CMP->GetOwner();
	//
	if (Owner) {
		FString OwnerGUID = FindSGUID(Owner);
		if (!Owner->GetName().Split(TEXT("_C_"),&OwnerID,nullptr,ESearchCase::CaseSensitive,ESearchDir::FromEnd)) {
			OwnerID = Owner->GetFName().ToString();
		}///
		//
		if (!OwnerGUID.IsEmpty()) {
			OwnerID.Append(TEXT("_"));
			OwnerID.Append(OwnerGUID);
		}///
	}///
	//
	FString ID;
	FString NetworkID;
	FString GUID = FindSGUID(CMP);
	//
	if (!CMP->GetName().Split(TEXT("_C_"),&ID,nullptr,ESearchCase::CaseSensitive,ESearchDir::FromEnd)) {
		ID = CMP->GetFName().ToString();
	} if (!GUID.IsEmpty()) {
		ID.Append(TEXT("_"));
		ID.Append(GUID);
	} if (!OwnerID.IsEmpty()) {ID=(OwnerID+FString(TEXT("_"))+ID);}
	//
	if (Owner && Owner->GetNetOwningPlayer()) {
		if (!GetPlayerNetworkID(Owner->GetNetOwningPlayer()->GetPlayerController(Owner->GetWorld()),NetworkID,false)) {
			if (Owner->GetNetOwningPlayer()->GetPlayerController(Owner->GetWorld())) {
				ID = FString(TEXT("P"))+FString::FromInt(UGameplayStatics::GetPlayerControllerID(Owner->GetNetOwningPlayer()->GetPlayerController(Owner->GetWorld())))+(FString(TEXT("_"))+ID);
		}} else {ID=(NetworkID+FString(TEXT("_"))+ID);}
	} return *ID;
#endif
	return FString();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UObject* USQLReflector::NewObjectInstance(UObject* Context, UClass* Class) {
	if (!Context||!Context->IsValidLowLevelFast()||Context->IsPendingKill()) {return nullptr;}
	if (Class==nullptr) {return nullptr;}
	//
	auto World = GEngine->GetWorldFromContextObject(Context,EGetWorldErrorMode::LogAndReturnNull);
	if (World!=nullptr) {return NewObject<UObject>(Context,Class);}
	//
	return nullptr;
}

UObject* USQLReflector::NewNamedObjectInstance(UObject* Context, UClass* Class, FName Name) {
	if (!Context||!Context->IsValidLowLevelFast()||Context->IsPendingKill()) {return nullptr;}
	if (Class==nullptr) {return nullptr;}
	//
	auto World = GEngine->GetWorldFromContextObject(Context,EGetWorldErrorMode::LogAndReturnNull);
	if (World!=nullptr) {return NewObject<UObject>(Context,Class,Name);}
	//
	return nullptr;
}

UObject* USQLReflector::GetClassDefaultObject(UClass* Class) {
	return Class->GetDefaultObject(true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////