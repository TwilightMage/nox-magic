#include "SaveSlotMeta.h"
#include "FileHelper.h"
#include <filesystem>

USaveSlotMeta* USaveSlotMeta::Load(FString path)
{
	TArray<uint8> bytes;
	FFileHelper::LoadFileToArray(bytes, *path);

	return UNoxMagicFunctions::DeserializeObject<USaveSlotMeta>(bytes);
}

USaveSlotMeta* USaveSlotMeta::Save(FString path, FDateTime inTime, UTexture2D* inThumb)
{
	USaveSlotMeta* object = NewObject<USaveSlotMeta>();
	object->name = FString(std::experimental::filesystem::path(*path).stem().c_str());
	object->time = inTime;
	UNoxMagicFunctions::RawDataFromTexture(inThumb, object->thumb);

	TArray<uint8> bytes = UNoxMagicFunctions::SerializeObject(object);
	FFileHelper::SaveArrayToFile(bytes, *path);

	return object;
}

FSaveMeta USaveSlotMeta::GetStruct()
{
	UTexture2D* tex;
	UNoxMagicFunctions::TextureFromRawData(thumb, tex);
	return FSaveMeta(name, time, tex);
}
