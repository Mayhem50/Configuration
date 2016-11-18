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

ULayerManager::ULayerManager()
{

}

ULayerManager::~ULayerManager()
{
    
}

void ULayerManager::Init(){
    TArray<uint8> BinaryArray;
	BinaryArray.Init (0, 0);
    
    FString FilePath = FPaths::Combine(*FPaths::GamePluginsDir(), TEXT("Configuration"), TEXT("Content"), TEXT("ConfData.dat"));
    
	LogText (FilePath);
    
    if(FPaths::FileExists(FilePath)){
		Layers.Empty ();
        FFileHelper::LoadFileToArray(BinaryArray, *FilePath);
        
        if(BinaryArray.Num() > 0){
            FMemoryReader Reader = FMemoryReader(BinaryArray, true);
            
            while(!Reader.AtEnd()){
                UMaterialLayer* Layer = NewObject<UMaterialLayer>();
                Reader << *Layer;
				//Layer->Serialize (Reader);
                //Layer->OnEnabledChanged.BindUObject (this, &ULayerManager::OnLayerEnabledChanged);
                Layers.Add(Layer);
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
    else{
        AddLayer();
        CurrentLayer = Layers[0];
		ParseAllActors (CurrentLayer);
        Save();
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
	if(Context.PrimaryObject)
		LogText (Context.PrimaryObject->GetName ());

	LogText (Context.Title.ToString() + " " + Context.Context);

	UTransBuffer* Transbuffer = Cast<UTransBuffer> (GEditor->Trans);
	int32 UndoCount = Transbuffer->GetUndoCount ();
	const FTransaction* Transaction = Transbuffer->GetTransaction (Transbuffer->UndoBuffer.Num () - UndoCount);

	if (!Transaction)
	{
		return;
	}

	TArray<UObject*> Objects;
	Transaction->GetTransactionObjects (Objects);

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
			}

			LogText (StaticMeshActor->GetName ());
		}
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

void ULayerManager::Save(){
	if (Layers.Num () == 0 && !CurrentLayer)
	{
		return;
	}

    FBufferArchive BufferArchive;
    
    for(auto Layer : Layers){
        BufferArchive << *Layer;
		//Layer->Serialize (BufferArchive);
    }

	BufferArchive << *CurrentLayer;
    
    if(BufferArchive.Num() > 0){
        FString FilePath = FPaths::Combine(*FPaths::GamePluginsDir(), TEXT("Configuration"), TEXT("Content"), TEXT("ConfData.dat"));
        LogText(FilePath);
        FFileHelper::SaveArrayToFile(BufferArchive, *FilePath);
    }
}

void ULayerManager::AddLayer(){
    UMaterialLayer* Layer = NewObject<UMaterialLayer>();
	Layer->Init (ELayerEnum::MATERIAL, "Layer_" + FString::FromInt (Layers.Num ()));
    //Layer->OnEnabledChanged.BindUObject(this, &ULayerManager::OnLayerEnabledChanged);
    Layers.Add(Layer);
}

void ULayerManager::RemoveLayer(UMaterialLayer* MaterialLayer){
    if(MaterialLayer == nullptr || Layers.Num() == 1){ return; }
    
    Layers.Remove(MaterialLayer);
}

void ULayerManager::Duplicate(UMaterialLayer* MaterialLayer){
    if(!MaterialLayer){ return; }
    
    Layers.Add(MaterialLayer->Clone());
}

void ULayerManager::OnObjectModified(UObject* Object){
    if(!CurrentLayer){ return; }

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

void ULayerManager::OnLayerEnabledChanged(){
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

	if (idx1 > 0 && idx2 > 0)
	{
		Layers[idx1] = Layer2;
		Layers[idx2] = Layer1;

		Save ();

		return true;
	}

	return false;
}

void ULayerManager::OnObjectPropertyChanged (UObject* Object, FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName () : NAME_None;

	ShouldCreateLayer ();

	if (PropertyName == "OverrideMaterials")
	{
		LogText ("On Object Property Changed");
	}

	Save ();
}

void ULayerManager::OnApplyObjectOnActor(UObject* Object, AActor* Actor)
{
    if(!CurrentLayer){ return; }	

	ShouldCreateLayer ();
    
    UMaterialInterface* Material = Cast<UMaterialInterface>(Object);
    
    if(Material)
    {
        CurrentLayer->Update(Actor);
    }
    
    Save();
}

void ULayerManager::ShouldCreateLayer ()
{
	FModifierKeysState KeysState = FSlateApplication::Get ().GetModifierKeys ();

	if (KeysState.IsControlDown ())
	{
		AddLayer ();
		CurrentLayer = Layers.Last ();
	}
}

void ULayerManager::SetCurentLayer (UMaterialLayer* Layer)
{
	CurrentLayer = Layer;
}

