//
//  LayerManager.cpp
//  UE4PluginsDev
//
//  Created by Benjamin Regnier on 06/11/2016.
//  Copyright © 2016 Epic Games, Inc. All rights reserved.
//

#include "ConfigurationPrivatePCH.h"

#include "Tools.h"

#include "LayerManager.h"
#include "LevelEditor.h"
#include "SLevelViewport.h"
#include "EditorUndoClient.h"
#include "InputCoreTypes.h"

#include "Classes/Editor/Transactor.h"
#include "ScopedTransaction.h"

ULayerManager::ULayerManager ()
{
	SetFlags (RF_Transactional);
}

ULayerManager::~ULayerManager ()
{

}

void ULayerManager::Init () {
	TArray<uint8> BinaryArray;
	BinaryArray.Init (0, 0);

	FString FilePath = FPaths::Combine (*FPaths::GamePluginsDir (), TEXT ("Configuration"), TEXT ("Content"), TEXT ("ConfData.dat"));

	LogText (FilePath);

	if (FPaths::FileExists (FilePath)) {
		Layers.Empty ();
		FFileHelper::LoadFileToArray (BinaryArray, *FilePath);

		if (BinaryArray.Num () > 0) {
			FMemoryReader Reader = FMemoryReader (BinaryArray, true);

			while (!Reader.AtEnd ()) {
				UMaterialLayer* Layer = NewObject<UMaterialLayer> ();
				Reader << *Layer;
				//Layer->Serialize (Reader);
				//Layer->OnEnabledChanged.BindUObject (this, &ULayerManager::OnLayerEnabledChanged);
				Layers.Add (Layer);
			}

			CurrentLayer = Layers.Last ();
			Layers.RemoveAt (Layers.Num () - 1);

			for (UMaterialLayer* Layer : Layers)
			{
				if (Layer->LayerName == CurrentLayer->LayerName)
				{
					CurrentLayer = Layer;
					break;
				}
			}
		}
	}
	else {
		AddLayer ();
		CurrentLayer = Layers[0];
		ParseAllActors (CurrentLayer);
		Save ();
	}

	UTransBuffer* Transbuffer = Cast<UTransBuffer> (GEditor->Trans);

	if (Transbuffer)
	{
		Transbuffer->OnUndo ().AddUObject (this, &ULayerManager::OnUndoRedo);
		Transbuffer->OnRedo ().AddUObject (this, &ULayerManager::OnUndoRedo);
	}
}

void ULayerManager::OnUndoRedo (FUndoSessionContext Context, bool bCanRedo)
{
	UTransBuffer* Transbuffer = Cast<UTransBuffer> (GEditor->Trans);
	int32 UndoCount = Transbuffer->GetUndoCount ();
	const FTransaction* Transaction = Transbuffer->GetTransaction (Transbuffer->UndoBuffer.Num () - UndoCount);

	if (!Transaction)
	{
		return;
	}

	TArray<UObject*> Objects;
	Transaction->GetTransactionObjects (Objects);

	bool bNeedSave = false;

	for (UObject* Object : Objects)
	{
		UObject* Outer = Object->GetOuter ();

		if (Outer)
		{
			AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor> (Outer);
			ASkeletalMeshActor* SkeletalMeshActor = Cast<ASkeletalMeshActor> (Outer);

			if (StaticMeshActor || SkeletalMeshActor)
			{
				CurrentLayer->Update (Cast<AActor> (Outer));
				bNeedSave = true;
			}
		}
	}

	if (bNeedSave) {
		Save ();
	}
}

bool ULayerManager::ActorExistInCurrentLayer (AActor* Actor)
{
	/*if (Cast<AStaticMeshActor> (Actor) || Cast<ASkeletalMeshActor> (Actor))
	{
		if (CurrentLayer->GetPrimitiveToMaterialsMap ().FindKey (Actor->GetName ()))
		{
			return true;
		}
	}

	return false;*/

	return true;
}

void ULayerManager::ParseAllActors (UMaterialLayer* Layer)
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule> ("LevelEditor");
	TSharedPtr<SLevelViewport> Viewport = StaticCastSharedPtr<SLevelViewport> (LevelEditorModule.GetFirstActiveViewport ());

	TActorIterator< AStaticMeshActor > StaticMeshActorItr = TActorIterator< AStaticMeshActor > (Viewport->GetWorld ());

	while (StaticMeshActorItr)
	{
		Layer->Update (*StaticMeshActorItr);
		++StaticMeshActorItr;
	}

	TActorIterator< ASkeletalMeshActor > SkeletalMeshActorItr = TActorIterator< ASkeletalMeshActor > (Viewport->GetWorld ());

	while (SkeletalMeshActorItr)
	{
		UE_LOG (LogTemp, Warning, TEXT ("Actor Name is %s: "), *SkeletalMeshActorItr->GetName ());

		Layer->Update (*SkeletalMeshActorItr);
		++SkeletalMeshActorItr;
	}
}

void ULayerManager::Save () {
	if (Layers.Num () == 0 && !CurrentLayer)
	{
		return;
	}

	FBufferArchive BufferArchive;

	for (auto Layer : Layers) {
		BufferArchive << *Layer;
		//Layer->Serialize (BufferArchive);
	}

	BufferArchive << *CurrentLayer;

	if (BufferArchive.Num () > 0) {
		FString FilePath = FPaths::Combine (*FPaths::GamePluginsDir (), TEXT ("Configuration"), TEXT ("Content"), TEXT ("ConfData.dat"));
		LogText (FilePath);
		FFileHelper::SaveArrayToFile (BufferArchive, *FilePath);
	}
}

void ULayerManager::AddLayer () {
	CommitBeforeChange ("Add Layer");
	UMaterialLayer* Layer = NewObject<UMaterialLayer> ();
	Layer->Init (ELayerEnum::MATERIAL, "Layer_" + FString::FromInt (Layers.Num ()));
	//Layer->OnEnabledChanged.BindUObject(this, &ULayerManager::OnLayerEnabledChanged);
	Layers.Add (Layer);
	Save ();
}

void ULayerManager::RemoveLayer (UMaterialLayer* MaterialLayer) {
	if (MaterialLayer == nullptr || Layers.Num () == 1) { return; }

	CommitBeforeChange ("Remove Layer");
	Layers.Remove (MaterialLayer);
	Save ();
}

void ULayerManager::Duplicate (UMaterialLayer* MaterialLayer) {
	if (!MaterialLayer) { return; }

	CommitBeforeChange ("Duplicate Layer");
	Layers.Add (MaterialLayer->Clone ());
	Save ();
}

void ULayerManager::OnObjectModified (UObject* Object) {
	if (!CurrentLayer) { return; }

	UpdateFromObject (Object);
}

void ULayerManager::UpdateFromObject (UObject* Object) {
	UObject* Outer = Object->GetOuter ();

	AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor> (Outer);

	if (StaticMeshActor)
	{
		CurrentLayer->Update (StaticMeshActor);
		LogText ("On Object Modify " + StaticMeshActor->GetName ());
		Save ();
		return;
	}

	ASkeletalMeshActor* SkeletalMeshActor = Cast<ASkeletalMeshActor> (Outer);

	if (SkeletalMeshActor)
	{
		CurrentLayer->Update (SkeletalMeshActor);
		LogText ("On Object Modify " + SkeletalMeshActor->GetName ());
		Save ();
		return;
	}
}

void ULayerManager::OnLayerEnabledChanged () {
	ApplyDisplayedLayers ();
}

void ULayerManager::ApplyDisplayedLayers ()
{
	for (UMaterialLayer* Layer : Layers)
	{
		if (Layer->IsEnabled ())
		{
			Layer->Apply ();
		}
	}
}

bool ULayerManager::SwapMaterials (UMaterialLayer* Layer1, UMaterialLayer* Layer2)
{
	int32 idx1 = Layers.Find (Layer1);
	int32 idx2 = Layers.Find (Layer2);

	if (idx1 >= 0 && idx2 >= 0)
	{
		CommitBeforeChange ("Swap Layers");
		Layers[idx1] = Layer2;
		Layers[idx2] = Layer1;

		Save ();

		return true;
	}

	return false;
}

void ULayerManager::OnObjectPropertyChanged (UObject* Object, FPropertyChangedEvent& PropertyChangedEvent)
{
	static int32 ObjectCount = 0;

	TSet<UObject*> Objects =  FCoreUObjectDelegates::ObjectsModifiedThisFrame;

	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName () : NAME_None;

	if (PropertyName == "OverrideMaterials")
	{
		if (GEditor->GetSelectedActorCount () == 1 || (GEditor->GetSelectedActorCount () > 1 && ObjectCount == 0)) {
			ShouldCreateLayer ();
			++ObjectCount;
		}
		else {
			++ObjectCount;
		}

		if (GEditor->GetSelectedActorCount () == ObjectCount) {
			ObjectCount = 0;
		}

		LogText ("On Object Property Changed on : " + Object->GetName());
		UpdateFromObject (Object);
	}
}

void ULayerManager::OnApplyObjectOnActor (UObject* Object, AActor* Actor)
{
	if (!CurrentLayer) { return; }

	ShouldCreateLayer ();

	UMaterialInterface* Material = Cast<UMaterialInterface> (Object);

	if (Material)
	{
		CurrentLayer->Update (Actor);
	}

	Save ();
}

void ULayerManager::ShouldCreateLayer ()
{
	FModifierKeysState KeysState = FSlateApplication::Get ().GetModifierKeys ();

	if (KeysState.IsControlDown ())
	{
		AddLayer ();
		CurrentLayer = Layers.Last ();
		OnLayersManagerNotify.ExecuteIfBound ();
	}
}

void ULayerManager::SetCurentLayer (UMaterialLayer* Layer)
{
	CommitBeforeChange ("Change Current Layer");
	CurrentLayer = Layer;
}

void ULayerManager::PreEditUndo ()
{
	LogText ("PreEditUndo");
	LogText (CurrentLayer->LayerName);
}

void ULayerManager::PostEditUndo ()
{
	LogText ("PostEditUndo");
	LogText (CurrentLayer->LayerName);

	OnPostEditUndo.ExecuteIfBound ();
	OnLayersManagerNotify.ExecuteIfBound ();
}

void ULayerManager::CommitBeforeChange (const FString& String)
{
	const FScopedTransaction Transaction (TEXT ("LayerManager"), FText::FromString (String), nullptr);
	this->Modify ();
}