#include "ConfigurationPrivatePCH.h"

#include "Tools.h"

#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

void DisplayNotification (const FString& String, float Duration)
{
	FNotificationInfo Info (FText::FromString (String));
	Info.FadeInDuration = 0.1f;
	Info.FadeOutDuration = 0.5f;
	Info.ExpireDuration = Duration;
	Info.bUseThrobber = false;
	Info.bUseSuccessFailIcons = true;
	Info.bUseLargeFont = true;
	Info.bFireAndForget = false;
	Info.bAllowThrottleWhenFrameRateIsLow = false;
	auto NotificationItem = FSlateNotificationManager::Get ().AddNotification (Info);
	NotificationItem->SetCompletionState (SNotificationItem::CS_Success);
	NotificationItem->ExpireAndFadeout ();
}