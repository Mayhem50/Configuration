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

#include "Tools.h"

FMaterialLayer::FMaterialLayer(const FString& InName):
Enabled(true),
Name(InName)
{
    
}

FMaterialLayer::~FMaterialLayer()
{
    
}

TSharedPtr<FMaterialLayer> FMaterialLayer::Clone(){
    TSharedPtr<FMaterialLayer> NewMaterialLayer = MakeShareable(new FMaterialLayer);
    NewMaterialLayer->PrimitiveMaterialsMap = PrimitiveMaterialsMap;
    NewMaterialLayer->Name = Name + "_";
    
    return NewMaterialLayer;
}

void FMaterialLayer::Update(AActor* Actor)
{
    Update(Cast<UPrimitiveComponent>(Actor));
}

void FMaterialLayer::Update(UPrimitiveComponent* Primitive)
{
    if(Primitive){
        TArray<UMaterialInterface*> Materials;
        Primitive->GetUsedMaterials(Materials);
        
        for(auto Material : Materials){
            PrimitiveMaterialsMap[Primitive->GetName()].Add(GetObjPath(Material));
        }
    }
}

void FMaterialLayer::Apply()
{
    ApplyToStaticMesh();
    ApplyToSkeletalMesh();
}

void FMaterialLayer::ApplyToStaticMesh(){
    TActorIterator< AStaticMeshActor > ActorItr = TActorIterator< AStaticMeshActor >(GEditor->GetWorld());
    
    while (ActorItr)
    {
        TArray<FName>* Paths = PrimitiveMaterialsMap.Find(ActorItr->GetName());
        
        if(Paths){
            UStaticMeshComponent* StaticMesh = ActorItr->GetStaticMeshComponent();
            UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(StaticMesh);
            
            if(Primitive){
                TArray<UMaterialInterface*> Materials;
                for(auto MaterialPath : *Paths){
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

void FMaterialLayer::ApplyToSkeletalMesh(){
    TActorIterator< ASkeletalMeshActor > ActorItr = TActorIterator< ASkeletalMeshActor >(GEditor->GetWorld());
    
    while (ActorItr)
    {
        TArray<FName>* Paths = PrimitiveMaterialsMap.Find(ActorItr->GetName());
        
        if(Paths){
            USkeletalMeshComponent* SkeletalMesh = ActorItr->GetSkeletalMeshComponent();
            UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(SkeletalMesh);
            
            if(Primitive){
                TArray<UMaterialInterface*> Materials;
                for(auto MaterialPath : *Paths){
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

void FMaterialLayer::ApplyToPrimitiveComponent(UPrimitiveComponent* Primitive, const TArray<UMaterialInterface*> Materials){
    for(int idx = 0; idx < Materials.Num(); ++idx){
        Primitive->SetMaterial(idx, Materials[idx]);
    }
}

FORCEINLINE FArchive& operator<<(FArchive &Ar, FMaterialLayer &MaterialLayer){
    FString Name = MaterialLayer.GetName();
    bool Enabled = MaterialLayer.IsEnabled();
    TMap<FString, TArray<FName>> PrimitiveToMaterialsMap = MaterialLayer.GetPrimitiveToMaterialsMap();
    
    Ar << Name;
    Ar << Enabled;
    Ar << PrimitiveToMaterialsMap;
    
    MaterialLayer.SetName(Name);
    MaterialLayer.SetIsEnabled(Enabled);
    MaterialLayer.SetPrimitiveToMaterialsMap(PrimitiveToMaterialsMap);
    
    
    return Ar;
}


