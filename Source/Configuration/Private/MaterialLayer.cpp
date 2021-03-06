//
//  MaterialLayer.cpp
//  UE4PluginsDev
//
//  Created by Benjamin Regnier on 06/11/2016.
//  Copyright © 2016 Epic Games, Inc. All rights reserved.
//

#include "ConfigurationPrivatePCH.h"

#include "MaterialLayer.h"
#include "EngineUtils.h"
#include "LevelEditor.h"
#include "SLevelViewport.h"

#include "Tools.h"

#define LOCTEXT_NAMESPACE "FConfigurationModule"

UMaterialLayer::UMaterialLayer(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

UMaterialLayer::~UMaterialLayer()
{

}

UMaterialLayer* UMaterialLayer::Clone()
{
	UMaterialLayer* NewMaterialLayer = NewObject<UMaterialLayer>();
	NewMaterialLayer->PrimitiveMaterialsMap = PrimitiveMaterialsMap;
	NewMaterialLayer->LayerName = LayerName + "_";

	return NewMaterialLayer;
}

void UMaterialLayer::Update(AActor* Actor)
{
	AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(Actor);

	if(StaticMeshActor)
	{
		UStaticMeshComponent* Component = StaticMeshActor->GetStaticMeshComponent();
		TArray<UMaterialInterface*> Materials;
		Component->GetUsedMaterials(Materials);

		PrimitiveMaterialsMap.Add(StaticMeshActor->GetName());
		if(!PrimitiveMaterialsMap[StaticMeshActor->GetName()])
			PrimitiveMaterialsMap[StaticMeshActor->GetName()] = NewObject<UNameList>(this);

		for(auto Material : Materials)
		{
			LogText(GetObjPath(Material).ToString());
			PrimitiveMaterialsMap[StaticMeshActor->GetName()]->Names.Add(GetObjPath(Material));
		}
	}
}

void UMaterialLayer::Update(UPrimitiveComponent* Primitive)
{
	if(Primitive)
	{
		TArray<UMaterialInterface*> Materials;
		Primitive->GetUsedMaterials(Materials);

		PrimitiveMaterialsMap.Add(Primitive->GetName());
		if(!PrimitiveMaterialsMap[Primitive->GetName()])
			PrimitiveMaterialsMap[Primitive->GetName()] = NewObject<UNameList>(this);

		for(auto Material : Materials)
		{
			PrimitiveMaterialsMap[Primitive->GetName()]->Names.Add(GetObjPath(Material));
		}
	}
}

void UMaterialLayer::Apply()
{
	for(auto Tuple : PrimitiveMaterialsMap)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor Name is %s: "), *Tuple.Key);

		for(FName Material : Tuple.Value->Names)
			UE_LOG(LogTemp, Warning, TEXT("Material Path is %s: "), *Material.ToString());
	}

	ApplyToStaticMesh();
	ApplyToSkeletalMesh();
}

void UMaterialLayer::ApplyToStaticMesh()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	TSharedPtr<SLevelViewport> Viewport = StaticCastSharedPtr<SLevelViewport>(LevelEditorModule.GetFirstActiveViewport());

	TActorIterator< AStaticMeshActor > ActorItr = TActorIterator< AStaticMeshActor >(Viewport->GetWorld());

	while(ActorItr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor Name is %s: "), *ActorItr->GetName());
		UNameList** Names = PrimitiveMaterialsMap.Find(ActorItr->GetName());

		if(Names)
		{
			UStaticMeshComponent* StaticMesh = ActorItr->GetStaticMeshComponent();
			UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(StaticMesh);

			if(Primitive)
			{
				TArray<UMaterialInterface*> Materials;

				for(auto MaterialPath : (*Names)->Names)
				{
					UMaterialInterface* Material = LoadObject<UMaterialInterface>(nullptr, *MaterialPath.ToString());
					Materials.Add(Material);
				}
				ApplyToPrimitiveComponent(Primitive, Materials);
			}
		}

		//next actor
		++ActorItr;
	}
}

void UMaterialLayer::ApplyToSkeletalMesh()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	TSharedPtr<SLevelViewport> Viewport = StaticCastSharedPtr<SLevelViewport>(LevelEditorModule.GetFirstActiveViewport());

	TActorIterator< ASkeletalMeshActor > ActorItr = TActorIterator< ASkeletalMeshActor >(Viewport->GetWorld());

	while(ActorItr)
	{
		UNameList** Names = PrimitiveMaterialsMap.Find(ActorItr->GetName());

		if(Names)
		{
			USkeletalMeshComponent* SkeletalMesh = ActorItr->GetSkeletalMeshComponent();
			UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(SkeletalMesh);

			if(Primitive)
			{
				TArray<UMaterialInterface*> Materials;
				for(auto MaterialPath : (*Names)->Names)
				{
					UMaterialInterface* Material = LoadObject<UMaterialInterface>(nullptr, *MaterialPath.ToString());
					Materials.Add(Material);
				}
				ApplyToPrimitiveComponent(Primitive, Materials);
			}
		}

		//next actor
		++ActorItr;
	}

}

void UMaterialLayer::ApplyToPrimitiveComponent(UPrimitiveComponent* Primitive, const TArray<UMaterialInterface*> Materials)
{
	for(int idx = 0; idx < Materials.Num(); ++idx)
	{
		Primitive->SetMaterial(idx, Materials[idx]);
	}
}

#undef LOCTEXT_NAMESPACE
