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
#define TEST_APP_ID "30006"
#define TEST_APP_SECRET "1fbd6992-2684-426e-8399-892e8a5a7ccb"
#define TEST_LEADERBOARD_ID "testCreate" 

UENUM(BlueprintType)
enum class RuyiSDKRequestType : uint8
{
	RuyiSDKRequestTypeNone = 0,
	//when user logged in main client, get their login profile
	RuyiSDKRequestGetProfile,
	RuyiSDKPostScoreToLeadboard,
	RuyiSDKGetLeadboardPage,
	RuyiSDKListFriends,
	RuyiSDKGetPartyInfo,
	RuyiSDKCreateLobby,
	RuyiSDKStartTelemetrySession,
	RuyiSDKUpdateVideo,
	RuyiSDKDownloadVideo,
	RuyiSDKAwardAchievement,
};

using namespace Ruyi;
using namespace Ruyi::SDK::Online;
using namespace Ruyi::SDK::CommonType;

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

	void StartRuyiSDKListFriends();
	void RuyiSDKAsyncListFriends();

	void StartRuyiSDKGetPartyInfo();
	void RuyiSDKAsyncGetPartyInfo();

	void StartRuyiSDKStartTelemetrySession();
	void RuyiSDKAsyncStartTelemetrySession();

	void StartRuyiSDKUpdateVideo(FString& cloudFileName, FString& localFilePath);
	void RuyiSDKAsyncUpdateVideo();

	void StartRuyiSDKDownloadVideo(FString& cloudFileName);
	void RuyiSDKAsyncDownloadVideo();

	void StartRuyiSDKAwardAchievement(int score);
	void RuyiSDKAsyncAwardAchievement();
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

	std::string PlayerIDs[1];

	std::string m_localFilePath;
	std::string m_CloudFileName;
};