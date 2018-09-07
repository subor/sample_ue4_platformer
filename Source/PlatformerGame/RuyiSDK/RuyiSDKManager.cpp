#include "PlatformerGame.h"
#include "RuyiSDKManager.h"

#include "boost/container/detail/json.hpp"

#include "RuyiNet/Response/RuyiNetResponse.h"
#include "RuyiNet/Response/RuyiNetProfile.h"
#include "RuyiNet/Response/RuyiNetFindPlayersResponse.h"
#include "RuyiNet/Response/RuyiNetGetPartyInfoResponse.h"
#include "RuyiNet/Response/RuyiNetTelemetrySessionResponse.h"
#include "RuyiNet/Response/RuyiNetUploadFileResponse.h"

#include "RuyiNet/Service/Friend/RuyiNetFriendService.h"
#include "RuyiNet/Service/Party/RuyiNetPartyService.h"
#include "RuyiNet/Service/Telemetry/RuyiNetTelemetryService.h"
#include "RuyiNet/Service/Video/RuyiNetVideoService.h"
#include "RuyiNet/Service/Gamification/RuyiNetGamificationService.h"
#include "RuyiNet/Service/Gamification/RuyiNetAchievement.h"

#include "RuyiNet/Service/Leaderboard/RuyiNetLeaderboardEntry.h"
#include "RuyiNet/Service/Leaderboard/RuyiNetLeaderboardPage.h"
#include "RuyiNet/Service/Leaderboard/RuyiNetLeaderboardType.h"
#include "RuyiNet/Service/Leaderboard/RuyiNetRotationType.h"
#include "RuyiNet/Service/Leaderboard/RuyiNetLeaderboardService.h"
#include "RuyiNet/Service/Leaderboard/Response/RuyiNetGetGlobalLeaderboardEntryCountResponse.h"

#pragma region initilize
FRuyiSDKManager* FRuyiSDKManager::Instance() 
{
	static FRuyiSDKManager* m_Instance = new FRuyiSDKManager();
	return m_Instance;
}

FRuyiSDKManager::FRuyiSDKManager() 
{
	m_ProfileId = TEXT("");
	m_PlayerName = TEXT("");
	PlayerIDs[0] = { "bfdcafbf-b15d-4c01-93b0-b363b310ef80" };
	m_IsSDKReady = false;
}

void FRuyiSDKManager::Start()
{
	m_RuyiSDK = nullptr;
	
	m_ThreadEnd = false;
	m_ThreadBegin = false;
	m_Thread = nullptr;

	InitRuyiSDK();
}

void FRuyiSDKManager::InitRuyiSDK()
{
	try
	{
		auto context = new Ruyi::RuyiSDKContext(Ruyi::RuyiSDKContext::Endpoint::PC, "localhost");
		m_RuyiSDK = Ruyi::RuyiSDK::CreateSDKInstance(*context);

		m_IsSDKReady = true;

		UE_LOG(LogPlatformer, Log, TEXT("InitRuyiSDK Successful !!!"));

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Init RuyiSDK Successful !!!"));
		}

	} catch(std::exception e)
	{
		UE_LOG(LogPlatformer, Log, TEXT("InitRuyiSDK Exception !!!"));
	}
}

Ruyi::RuyiSDK* FRuyiSDKManager::SDK()
{
	return m_RuyiSDK;
}

bool FRuyiSDKManager::IsSDKReady()
{
	return m_IsSDKReady;
}

#pragma endregion

#pragma region Ruyi SDK Api related
void FRuyiSDKManager::StartRuyiSDKGetProfile() 
{
	if (!m_IsSDKReady) 
	{ 
		MainHUD->IsReady = true;

		return; 
	}

	MainHUD->ShowWaitingPanel(true);

	m_RuyiSDKRequestType = RuyiSDKRequestType::RuyiSDKRequestGetProfile;
	
	StartThread();
}

void FRuyiSDKManager::RuyiSDKAsyncGetProfile()
{
	try
	{
		std::string ret;
		FString fRet;
		//m_RuyiSDK->BCService->Authentication_ClearSavedProfileID(0);
		//m_RuyiSDK->BCService->Authentication_AuthenticateEmailPassword(ret, "godenzzm", "111", true, 0);

		//fRet = UTF8_TO_TCHAR(ret.c_str());
		//UE_LOG(LogPlatformer, Log, TEXT("RuyiSDKAsynceGetProfile Authentication_AuthenticateEmailPassword ret:%s !!!"), *fRet);

		m_RuyiSDK->BCService->Identity_SwitchToSingletonChildProfile(ret, "11499", false, 0);//ÍâÍø
		//m_RuyiSDK->BCService->Identity_SwitchToSingletonChildProfile(ret, "30002", true, 0);//ÄÚÍø

		fRet = UTF8_TO_TCHAR(ret.c_str());
		UE_LOG(LogPlatformer, Log, TEXT("RuyiSDKAsynceGetProfile Identity_SwitchToSingletonChildProfile ret:%s !!!"), *fRet);
		
		TSharedPtr<FJsonObject> jsonParsed;
		TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(fRet);

		if (FJsonSerializer::Deserialize(jsonReader, jsonParsed))
		{
			int status = jsonParsed->GetIntegerField("status");

			if (JSON_RESPONSE_OK == status)
			{
				TSharedPtr<FJsonObject> dataObject = jsonParsed->GetObjectField("data");
				if (dataObject.IsValid())
				{
					if (dataObject->TryGetStringField(TEXT("playerName"), m_PlayerName)
						&& dataObject->TryGetStringField(TEXT("parentProfileId"), m_ProfileId))
					{
						UE_LOG(LogPlatformer, Log, TEXT("FRuyiSDKManager::RuyiSDKAsynceGetProfile ProfileId:%s PlayerName:%s !!!"), *m_ProfileId, *m_PlayerName);

						string playerName = TCHAR_TO_UTF8(*m_PlayerName);
						string parentProfileId = TCHAR_TO_UTF8(*m_ProfileId);

						MainHUD->RuyiUserProfile = FString::Printf(TEXT("Id:%s name:%s"), *m_ProfileId, *m_PlayerName);

						//string payloadString = "{\"profileId\":\"" + parentProfileId + "\"}";
						//FString fPayload = UTF8_TO_TCHAR(payloadString.c_str());
						//UE_LOG(LogPlatformer, Log, TEXT("Ruyi_AsyncSDKGetProfile payloadString:%s !!!"), *fPayload);

						//m_RuyiSDK->BCService->Script_RunParentScript(ret, "GetProfile", payloadString, "RUYI", 0);
						//fRet = UTF8_TO_TCHAR(ret.c_str());

						//UE_LOG(LogPlatformer, Log, TEXT("Ruyi_AsyncSDKGetProfile RunParentScript ret:%s !!!"), *fRet);
					}
				}
			} 
		}
	} catch(std::exception e)
	{
		UE_LOG(LogPlatformer, Log, TEXT("Ruyi_AsyncSDKGetProfile Exception !!!"));
	}

	MainHUD->IsReady = true;

	EndThread();
}

void FRuyiSDKManager::StartRuyiSDKPostScoreToLeadboard(float score) 
{
	if (!m_IsSDKReady) return;

	m_Score = score;

	MainHUD->ShowWaitingPanel(true);

	m_RuyiSDKRequestType = RuyiSDKRequestType::RuyiSDKPostScoreToLeadboard;

	StartThread();
}

void FRuyiSDKManager::RuyiSDKAsyncPostScoreToLeadboard() 
{
	try
	{
		bool isSuccess = false;

		std::string leaderboardId = "1";

		m_RuyiSDK->RuyiNet->GetLeaderboardService()->PostScoreToLeaderboard(isSuccess, 0, (int)m_Score, leaderboardId);

		if (isSuccess) 
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("PostScoreToLeaderboard Success !!!"));
		} else 
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("PostScoreToLeaderboard Fail !!!"));
		}

	} catch(std::exception e)
	{
		UE_LOG(LogPlatformer, Log, TEXT("RuyiSDKAsyncPostScoreToLeadboard exception !!!"));
	}

	EndThread();
}

void FRuyiSDKManager::StartRuyiSDKGetLeadboardPage(Ruyi::SDK::BrainCloudApi::SortOrder::type sort, int startIndex, int endIndex)
{
	if (!m_IsSDKReady) return;

	m_SortType = sort;
	m_StartIndex = startIndex;
	m_EndIndex = endIndex;

	MainHUD->ShowWaitingPanel(true);

	m_RuyiSDKRequestType = RuyiSDKRequestType::RuyiSDKGetLeadboardPage;

	StartThread();
}

void FRuyiSDKManager::RuyiSDKAsyncGetLeadboardPage()
{
	try
	{
		RuyiNetGetGlobalLeaderboardEntryCountResponse response;
		std::string leaderboardId = "1";
		m_RuyiSDK->RuyiNet->GetLeaderboardService()->GetGlobalLeaderboardEntryCount(0, leaderboardId, response);

		std::string entryCountStr = "RuyiSDKAsyncGetLeadboardPage entryCount:" + response.data.entryCount;
		FString fstrEntryCountStr = UTF8_TO_TCHAR(entryCountStr.c_str());
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, *fstrEntryCountStr);

		RuyiNetLeaderboardPage page;
		SortOrder::type sort = SortOrder::type::HIGH_TO_LOW;
		int startIndex = 0;
		int endIndex = 9;
		m_RuyiSDK->RuyiNet->GetLeaderboardService()->GetGlobalLeaderboardPage(0, leaderboardId, sort, startIndex, endIndex, page);

		std::vector<RuyiNetLeaderboardEntry*>& entries = page.GetEntries();

		std::for_each(entries.begin(), entries.end(), [&](RuyiNetLeaderboardEntry* _entry)
		{
			std::string entryStr = "RuyiSDKAsyncGetLeadboardPage playId:" + _entry->GetPlayerId() + " score:" + to_string(_entry->GetScore());
			FString fstrEntryStr = UTF8_TO_TCHAR(entryCountStr.c_str());
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, *fstrEntryStr);

		});

	}catch(std::exception e)
	{
		UE_LOG(LogPlatformer, Log, TEXT("RuyiSDKAsyncGetLeadboardPage exception !!!"));
	}

	EndThread();
}

void FRuyiSDKManager::StartRuyiSDKListFriends()
{
	if (!m_IsSDKReady) return;

	MainHUD->ShowWaitingPanel(true);

	m_RuyiSDKRequestType = RuyiSDKRequestType::RuyiSDKListFriends;

	StartThread();
}

void FRuyiSDKManager::RuyiSDKAsyncListFriends()
{
	try
	{
		RuyiNetFriendListResponse response;
		m_RuyiSDK->RuyiNet->GetFriendService()->ListFriends(0, response);

		typedef RuyiNetFriendListResponse::Data::Response::Friend ruyiFriend;
		
		if (response.data.response.friends.size() > 0) 
		{
			std::for_each(response.data.response.friends.begin(), response.data.response.friends.end(), [&](ruyiFriend& friendData)
			{
				if (GEngine)
				{
					string info = friendData.playerId + " " + friendData.name;
					FString fInfo = UTF8_TO_TCHAR(info.c_str());
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, *fInfo);
				}
			});
		} else 
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("No Friends Data !!!"));
			}
		}
	} catch (std::exception e)
	{
		UE_LOG(LogPlatformer, Log, TEXT("RuyiSDKAsyncListFriends exception !!!"));
	}

	EndThread();
}

void FRuyiSDKManager::StartRuyiSDKGetPartyInfo()
{
	if (!m_IsSDKReady) return;

	MainHUD->ShowWaitingPanel(true);

	m_RuyiSDKRequestType = RuyiSDKRequestType::RuyiSDKGetPartyInfo;

	StartThread();
}

void FRuyiSDKManager::RuyiSDKAsyncGetPartyInfo() 
{
	try
	{
		RuyiNetGetPartyInfoResponse response;
		m_RuyiSDK->RuyiNet->GetPartyService()->GetPartyInfo(0, response);

		typedef RuyiNetGetPartyInfoResponse::Data::Response::Group ruyiPartyGroup;

		if (response.data.response.groups.size() > 0) 
		{
			std::for_each(response.data.response.groups.begin(), response.data.response.groups.end(), [&](ruyiPartyGroup& groupData) 
			{
				if (GEngine)
				{
					string info = groupData.name + " " + groupData.groupId;
					FString fInfo = UTF8_TO_TCHAR(info.c_str());
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, *fInfo);
				}
			});		
		} else 
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("No Party Data "));
			}
		}
	} catch (std::exception e)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Get Party Info Exception !!!"));
		}
	}

	EndThread();
}

void FRuyiSDKManager::StartRuyiSDKStartTelemetrySession() 
{
	if (!m_IsSDKReady) return;

	MainHUD->ShowWaitingPanel(true);

	m_RuyiSDKRequestType = RuyiSDKRequestType::RuyiSDKStartTelemetrySession;

	StartThread();
}

void FRuyiSDKManager::RuyiSDKAsyncStartTelemetrySession()
{
	try 
	{
		RuyiNetTelemetrySession session;
		RuyiNetTelemetrySessionResponse response;
		m_RuyiSDK->RuyiNet->GetTelemetryService()->StartTelemetrySession(0, response, session);

		if (0 != session.GetId().compare("")) 
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Start Telemetry Service !!!"));
			}

			std::map<std::string, std::string> customData;
			customData["position"] = "[10, 10]";

			RuyiNetResponse response1;
			m_RuyiSDK->RuyiNet->GetTelemetryService()->LogTelemetryEvent(0, session.GetId(), "ATTACK", customData, response1);

			if (JSON_RESPONSE_OK == response1.status) 
			{
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Log Telemetry Service !!!"));
				}
			}

			m_RuyiSDK->RuyiNet->GetTelemetryService()->EndTelemetrySession(0, session.GetId(), response1);

			if (JSON_RESPONSE_OK == response1.status)
			{
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("End Telemetry Service !!!"));
				}
			}
		}
		
	} catch (std::exception e) 
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Telemetry Service Exception !!!"));
		}
	}

	EndThread();
}

void FRuyiSDKManager::StartRuyiSDKUpdateVideo(FString& cloudFileName, FString& localFilePath)
{
	if (!m_IsSDKReady) return;

	m_CloudFileName = TCHAR_TO_UTF8(*cloudFileName);
	m_localFilePath = TCHAR_TO_UTF8(*localFilePath);

	MainHUD->ShowWaitingPanel(true);

	m_RuyiSDKRequestType = RuyiSDKRequestType::RuyiSDKUpdateVideo;

	StartThread();
}

void FRuyiSDKManager::RuyiSDKAsyncUpdateVideo() 
{
	try
	{
		RuyiNetUploadFileResponse response;

		m_RuyiSDK->RuyiNet->GetVideoService()->UploadVideo(0, m_CloudFileName, m_localFilePath, response);

		if (JSON_RESPONSE_OK == response.status)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Update Video finish !!!"));
			}
		}
	} catch (std::exception e) 
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Update Video Exception !!!"));
		}
	}

	EndThread();
}

void FRuyiSDKManager::StartRuyiSDKDownloadVideo(FString& cloudFileName)
{
	if (!m_IsSDKReady) return;
	
	m_CloudFileName = TCHAR_TO_UTF8(*cloudFileName);

	MainHUD->ShowWaitingPanel(true);

	m_RuyiSDKRequestType = RuyiSDKRequestType::RuyiSDKDownloadVideo;

	StartThread();
}

void FRuyiSDKManager::RuyiSDKAsyncDownloadVideo() 
{
	try
	{
		RuyiNetResponse response;

		m_RuyiSDK->RuyiNet->GetVideoService()->DownloadVideo(0, m_CloudFileName, response);

		if (JSON_RESPONSE_OK == response.status)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Download Video finish !!!"));
			}
		}
	} catch (std::exception e)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Download Video Exception !!!"));
		}
	}

	EndThread();
}

void FRuyiSDKManager::StartRuyiSDKAwardAchievement(int score)
{
	m_Score = score;

	if (m_Score > 45)
	{
		if (GEngine)
		{
			//PopupNotification a = new PopupNotification();
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("The NEXT Usain Bolt"));
		}
	} else if (m_Score > 35 && m_Score <= 45)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("I can do better than that"));
		}
	} else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("My grandma is faster than this"));
		}
	}
	
	if (!m_IsSDKReady) return;

	MainHUD->ShowWaitingPanel(true);

	m_RuyiSDKRequestType = RuyiSDKRequestType::RuyiSDKAwardAchievement;

	StartThread();
}

void FRuyiSDKManager::RuyiSDKAsyncAwardAchievement() 
{
	try 
	{
		std::string achievementId = "0";
		if (m_Score > 45)
		{		
			achievementId = "3";
		} else if (m_Score > 35 && m_Score <= 45)
		{		
			achievementId = "2";
		} else 
		{			
			achievementId = "1";
		}
		
		std::vector<RuyiNetAchievement*> achievements;
		m_RuyiSDK->RuyiNet->GetGamificationService()->ReadAchievedAchievements(0, 0, achievements);
		std::for_each(achievements.begin(), achievements.end(), [&](RuyiNetAchievement* pAchievement)
		{
			std::string AchievedAchievementStr = "Achieved Achievement Id:" + pAchievement->AchievementId;
			FString fstrAchievedAchievement = UTF8_TO_TCHAR(AchievedAchievementStr.c_str());
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, *fstrAchievedAchievement);
		});

		RuyiNetAchievement achievement;
		m_RuyiSDK->RuyiNet->GetGamificationService()->AwardAchievement(0, achievementId, achievement);
		
	} catch(std::exception e)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("AwardAchievement exception !!!"));
		}
	}
}

#pragma endregion

#pragma region FRunnable & multi-thread
void FRuyiSDKManager::StartThread()
{
	m_ThreadBegin = false;
	m_ThreadEnd = false;
	m_Thread = FRunnableThread::Create(this, TEXT("Ruyi-Subthread"), 0, TPri_BelowNormal);
}

void FRuyiSDKManager::EndThread()
{
	m_ThreadEnd = true;

	MainHUD->ShowWaitingPanel(false);

	if (nullptr != m_Thread)
	{
		m_Thread->Kill();
		delete m_Thread;
		m_Thread = nullptr;
	}
}

bool FRuyiSDKManager::Init() 
{
	m_ThreadEnd = false;
	m_ThreadBegin = false;
	m_Thread = nullptr;

	return true;
}

uint32 FRuyiSDKManager::Run() 
{
	while (!m_ThreadEnd)
	{ 
		if (!m_ThreadBegin)
		{
			m_ThreadBegin = true;

			switch(m_RuyiSDKRequestType)
			{
			case RuyiSDKRequestType::RuyiSDKRequestGetProfile:
				RuyiSDKAsyncGetProfile();
				break;
			case RuyiSDKRequestType::RuyiSDKPostScoreToLeadboard:
				RuyiSDKAsyncPostScoreToLeadboard();
				break;
			case RuyiSDKRequestType::RuyiSDKGetLeadboardPage:
				RuyiSDKAsyncGetLeadboardPage();
			case RuyiSDKRequestType::RuyiSDKListFriends:
				RuyiSDKAsyncListFriends();
				break;
			case RuyiSDKRequestType::RuyiSDKGetPartyInfo:
				RuyiSDKAsyncGetPartyInfo();
				break;
			case RuyiSDKRequestType::RuyiSDKStartTelemetrySession:
				RuyiSDKAsyncStartTelemetrySession();
				break;
			case RuyiSDKRequestType::RuyiSDKUpdateVideo:
				RuyiSDKAsyncUpdateVideo();
				break;
			case RuyiSDKRequestType::RuyiSDKDownloadVideo:
				RuyiSDKAsyncDownloadVideo();
				break;
			case RuyiSDKRequestType::RuyiSDKAwardAchievement:
				RuyiSDKAsyncAwardAchievement();
				break;
			default:break;
			}
		}
	}

	return 0;
}

#pragma endregion