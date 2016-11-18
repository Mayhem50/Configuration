//
//  Tools.h
//  UE4PluginsDev
//
//  Created by Benjamin Regnier on 10/11/2016.
//  Copyright © 2016 Epic Games, Inc. All rights reserved.
//

#pragma once

DECLARE_LOG_CATEGORY_CLASS(LogBRRPlugin, Log, All)

FORCEINLINE FName GetObjPath(const UObject* Obj)
{
    if(!Obj)
    {
        return NAME_None;
    }
    //~
    
    FStringAssetReference ThePath = FStringAssetReference(Obj);
    
    if(!ThePath.IsValid()) return NAME_None;
    
    //The Class FString Name For This Object
    FString Str = Obj->GetClass()->GetDescription();
    
    Str += "'";
    Str += ThePath.ToString();
    Str += "'";
    
    return FName(*Str);
}

FORCEINLINE void LogText (const FString& String)
{
	UE_LOG (LogBRRPlugin, Warning, TEXT ("%s"), *String);
}

void DisplayNotification (const FString& String, float Duration = 1.5f);
