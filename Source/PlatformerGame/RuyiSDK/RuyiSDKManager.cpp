#include "PlatformerGame.h"
#include "RuyiSDKManager.h"

#include "boost/container/detail/json.hpp"

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
		UE_LOG(LogPlatformer, Log, TEXT("RuyiSDKAsyncPostScoreToLeadboard score:%d !!!"), (int)m_Score);

		/*
		nlohmann::json payload =
		{
			{ "leaderboardId", "11499_Shooter" },
			{ "score", (int)m_Score }
		};
		*/

		int score = 1000 - m_Score;

		string payloadString1 = "{\"leaderboardId\":\"11499_Shooter\",\"score\":" 
			+ to_string(score) + "}";

		FString fPayload = UTF8_TO_TCHAR(payloadString1.c_str());
		UE_LOG(LogPlatformer, Log, TEXT("RuyiSDKAsyncPostScoreToLeadboard payloadString:%s !!!"), *fPayload);

		std::string ret;
		m_RuyiSDK->BCService->Script_RunParentScript(ret, "PostScoreToLeaderboard", payloadString1, "RUYI", 0);
		FString fRet = UTF8_TO_TCHAR(ret.c_str());

		UE_LOG(LogPlatformer, Log, TEXT("RuyiSDKAsyncPostScoreToLeadboard ret:%s !!!"), *fRet);

		std::string sortString = (m_SortType == Ruyi::SDK::BrainCloudApi::SortOrder::HIGH_TO_LOW) ? "HIGH_TO_LOW" : "LOW_TO_HIGH";

		string payloadString2 = "{\"leaderboardId\":\"11499_Shooter\",\"sort\":\"" + sortString +
			"\",\"startIndex\":\"0\",\"endIndex\":\"10\"}";

		m_RuyiSDK->BCService->Script_RunParentScript(ret, "GetGlobalLeaderboardPage", payloadString2, "RUYI", 0);
		fRet = UTF8_TO_TCHAR(ret.c_str());

		UE_LOG(LogPlatformer, Log, TEXT("RuyiSDKAsyncGetLeadboardPage ret:%s !!!"), *fRet);

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
					TSharedPtr<FJsonObject> responseObject = dataObject->GetObjectField("response");
					if (responseObject.IsValid())
					{
						TArray<TSharedPtr<FJsonValue>> leaderboardScores = responseObject->GetArrayField("leaderboard");

						TArray<FRuyiLeaderBoardScore> scores;
						for (int i = 0; i < leaderboardScores.Num(); ++i)
						{
							TSharedPtr<FJsonObject> tmp = leaderboardScores[i]->AsObject();
							FString playerId = tmp->GetStringField("playerId");
							FString name = tmp->GetStringField("name");
							int rank = tmp->GetIntegerField("rank");
							int score = tmp->GetIntegerField("score");

							if (name.IsEmpty() || name == TEXT(""))
							{
								name = TEXT("No Name");
							}

							UE_LOG(LogPlatformer, Log, TEXT("RuyiSDKAsyncGetLeadboardPage rank:%d score:%d !!!"), rank, score);

							FRuyiLeaderBoardScore tmpScore;
							tmpScore.profileId = playerId;
							tmpScore.name = name;
							tmpScore.rank = rank;
							tmpScore.score = 1000 - score;
							if (tmpScore.score > 1000) tmpScore.score = 1000;
							if (tmpScore.score < 0) tmpScore.score = 0;
							scores.Add(tmpScore);
						}

						MainHUD->UpdateHighscoreNamesAndScores(scores);
					}
				}
			}
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
		std::string sortString = (m_SortType == Ruyi::SDK::BrainCloudApi::SortOrder::HIGH_TO_LOW) ? "HIGH_TO_LOW" : "LOW_TO_HIGH";
		/*
		nlohmann::json payload =
		{
			{ "leaderboardId", "11499_Shooter" },
			{ "sort", sortString },
			{ "startIndex", m_StartIndex },
			{ "endIndex", m_EndIndex }
		};*/

		string payloadString = "{\"leaderboardId\":\"11499_Shooter\",\"sort\":\"" + sortString + 
			"\",\"startIndex\":\"" + to_string(m_StartIndex) + 
			"\",\"endIndex\":\"" + to_string(m_EndIndex) + "\"}";

		FString fPayload = UTF8_TO_TCHAR(payloadString.c_str());
		UE_LOG(LogPlatformer, Log, TEXT("RuyiSDKAsyncGetLeadboardPage payloadString:%s !!!"), *fPayload);

		std::string ret;
		m_RuyiSDK->BCService->Script_RunParentScript(ret, "GetGlobalLeaderboardPage", payloadString, "RUYI", 0);
		FString fRet = UTF8_TO_TCHAR(ret.c_str());

		UE_LOG(LogPlatformer, Log, TEXT("RuyiSDKAsyncGetLeadboardPage ret:%s !!!"), *fRet);

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
					TSharedPtr<FJsonObject> responseObject = dataObject->GetObjectField("response");
					if (responseObject.IsValid()) 
					{
						TArray<TSharedPtr<FJsonValue>> leaderboardScores = responseObject->GetArrayField("leaderboard");
						
						TArray<FRuyiLeaderBoardScore> scores;
						for (int i = 0; i < leaderboardScores.Num(); ++i)
						{
							TSharedPtr<FJsonObject> tmp = leaderboardScores[i]->AsObject();
							FString playerId = tmp->GetStringField("playerId");
							FString name = tmp->GetStringField("name");
							int rank = tmp->GetIntegerField("rank");
							int score = tmp->GetIntegerField("score");

							if (name.IsEmpty() || name == TEXT(""))
							{
								name = TEXT("No Name");
							}

							UE_LOG(LogPlatformer, Log, TEXT("RuyiSDKAsyncGetLeadboardPage rank:%d score:%d !!!"), rank, score);

							FRuyiLeaderBoardScore tmpScore;
							tmpScore.profileId = playerId;
							tmpScore.name = name;
							tmpScore.rank = rank;
							tmpScore.score = 1000 - score;
							if (tmpScore.score > 1000) tmpScore.score = 1000;
							if (tmpScore.score < 0) tmpScore.score = 0;
							scores.Add(tmpScore);
						}

						MainHUD->UpdateHighscoreNamesAndScores(scores);
					}
				}
			}	
		}
	}catch(std::exception e)
	{
		UE_LOG(LogPlatformer, Log, TEXT("RuyiSDKAsyncGetLeadboardPage exception !!!"));
	}

	EndThread();
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
				break;
			default:break;
			}
		}
	}

	return 0;
}

#pragma endregion