//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "USQLAsync.h"
#include "USQLite_Shared.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

USQL_CheckThreadSafety* USQL_CheckThreadSafety::SQL_CheckThreadSafety(UObject* Context, USQLite* Database, const float WaitForSeconds, bool Loop, const FSQLite_ThreadChanged &ThreadChangedCallback) {
	USQL_CheckThreadSafety* OBJ = NewObject<USQL_CheckThreadSafety>();
	//
	OBJ->Repeat = Loop;
	OBJ->Target = Database;
	OBJ->Timer = WaitForSeconds;
	OBJ->World = Context->GetWorld();
	OBJ->Callback = ThreadChangedCallback;
	//
	return OBJ;
}

void USQL_CheckThreadSafety::Activate() {
	if (Target==nullptr) {FFrame::KismetExecutionMessage(TEXT("{USQL}:: Invalid Target Database."),ELogVerbosity::Error); return;}
	if (World==nullptr) {FFrame::KismetExecutionMessage(TEXT("{USQL}:: Invalid World Context."),ELogVerbosity::Error); return;}
	if (Timer <= 0.0f) {Repeat = false;}
	//
	if (World) {
		const FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this,&USQL_CheckThreadSafety::Execute);
		World->GetTimerManager().SetTimer(TimerHandle,TimerDelegate,Timer,Repeat);
	}///
}

void USQL_CheckThreadSafety::Execute() {
	ESQLThreadSafety ThreadSafety;
	Target->DB_GetThreadSafety(ThreadSafety,Callback);
	//
	switch (ThreadSafety) {
		case ESQLThreadSafety::IsCurrentlyThreadSafe:
			ThreadSafe.Broadcast(); break;
		case ESQLThreadSafety::AsynchronousSaving:
			AsyncLoading.Broadcast(); break;
		case ESQLThreadSafety::AsynchronousLoading:
			AsyncSaving.Broadcast(); break;
	default: break;}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////