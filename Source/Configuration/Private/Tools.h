//
//  Tools.h
//  UE4PluginsDev
//
//  Created by Benjamin Regnier on 10/11/2016.
//  Copyright © 2016 Epic Games, Inc. All rights reserved.
//

#ifndef Tools_h
#define Tools_h

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
#endif /* Tools_h */
