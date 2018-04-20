#pragma once

#include "AllowWindowsPlatformTypes.h"
#include "Windows/WindowsPlatformProcess.h"
#include "HideWindowsPlatformTypes.h"
#include "HAL/ThreadingBase.h"

#include "PlatformerHUD.h"

//#include "CoreMinimal.h"
#include "RuyiSDK.h"

/*
	Ruyi SDK Api, most api function involes network. We recommend you use it in subthread
	Or it maybe will cause user exprience issue if you call them in main thread
*/

#define JSON_RESPONSE_OK 200

UENUM(BlueprintType)
enum class RuyiSDKRequestType : uint8
{
	RuyiSDKRequestTypeNone = 0,
	//when user logged in main client, get their login profile
	RuyiSDKRequestGetProfile,
	RuyiSDKPostScoreToLeadboard,
	RuyiSDKGetLeadboardPage,
};



class FRuyiSDKManager : public FRunnable
{
public:
	static FRuyiSDKManager* Instance();
	Ruyi::RuyiSDK* SDK();
	void Start();
	bool IsSDKReady();

	APlatformerHUD* MainHUD;

#pragma region Ruyi SDK function

	void StartRuyiSDKGetProfile();
	void RuyiSDKAsyncGetProfile();

	void StartRuyiSDKPostScoreToLeadboard(float score);
	void RuyiSDKAsyncPostScoreToLeadboard();

	void StartRuyiSDKGetLeadboardPage(Ruyi::SDK::BrainCloudApi::SortOrder::type sort, int startIndex, int endIndex);
	void RuyiSDKAsyncGetLeadboardPage();
#pragma endregion

#pragma region Ruyi SDK Data 
	//Ruyi SDK related data, I just put it here for convenience
	//you can wrap it for using
	FString m_ProfileId;
	FString m_PlayerName;

	float m_Score;
	//search type
	Ruyi::SDK::BrainCloudApi::SortOrder::type m_SortType;
	//page index
	int m_StartIndex;
	int m_EndIndex;
	//leaderboard data;
	
#pragma endregion

#pragma region FRunnable
	virtual bool Init() override;
	virtual uint32 Run() override;
#pragma endregion

private:
	FRuyiSDKManager();
	Ruyi::RuyiSDK* m_RuyiSDK;
	void InitRuyiSDK();
	bool m_IsSDKReady;

	RuyiSDKRequestType m_RuyiSDKRequestType;

	//multi-thread
	void StartThread();
	void EndThread();
	FRunnableThread* m_Thread;
	FCriticalSection m_mutex;
	bool m_ThreadEnd;
	bool m_ThreadBegin;
};