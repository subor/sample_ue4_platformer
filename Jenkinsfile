#!groovy
pipeline {
	agent {
		node{
			label 'unreal'
		}
	}
	
	parameters {
		booleanParam(name: 'CLEAN_BUILD', defaultValue: true, description: 'Options for doing a clean build when is true')
		booleanParam(name: 'CODE_SIGN', defaultValue: true, description: 'Options to sign the build when is true')
		string(name: 'REF_BUILD_NUMBER', defaultValue: '', description: 'Specified build number to copy dependencies(E.g.: 22, using latest build when empty)')
		string(name: 'JOB_NAME', defaultValue: 'RUYI-Platform-CleanBuild', description: 'Specified job name to copy dependencies')
		booleanParam(name: 'MAIL_ON_FAILED', defaultValue: true, description: 'Options for sending mail when failed')
	}
	
	options{
		//Set a timeout period for the Pipeline run, after which Jenkins should abort the Pipeline.
		timeout(time: 2, unit: 'HOURS')	
		//Persist artifacts and console output for the specific number of recent Pipeline runs. 
		buildDiscarder(logRotator(numToKeepStr: '30', artifactNumToKeepStr: '30'))
		//Skip checking out code from source control by default in the agent directive.
		skipDefaultCheckout()
		//Disallow concurrent executions of the Pipeline.
		//disableConcurrentBuilds()
	}
	
	environment{
		//Windows command prompt encoding(65001=UTF8;437=US.English;936=GBK)
		WIN_CMD_ENCODING = '65001'
		//Nuget Packages home
		NUGET_PACKAGES = "c:/jenkins/packages"
		//Unreal Engine root
		UE_ROOT = "C:/Program Files/Epic Games/UE_4.21/Engine"
		//Temp folder
		TEMP_DIR = 'temp'
		//Root directory for all build tools
		WIN32_TOOLS = "c:/jenkins/tools"
		//Unreal Demo Root
		DEMO_PROJECT_ROOT = "sample_ue4_platformer"
		//DEMO SDK CPP folder
		DEMO_SDKCPP_ROOT = "${DEMO_PROJECT_ROOT}\\Source\\PlatformerGame\\RuyiSDK"
		//Unreal packed target
		COOKED_ROOT = "${DEMO_PROJECT_ROOT}\\Pack"
		//CodeSigning
		CODESIGNING_HOME = "${WIN32_TOOLS}/CodeSigning"
		SCRIPTS = "${workspace}/${DEMO_PROJECT_ROOT}/basic_tools"
		MSBUILD = "${tool 'MSBuild'}"
		//File path for saving commit id
		COMMIT_ID_FILE = "${COOKED_ROOT}\\commit-id"
		//Mail recipient on failed
		MAIL_RECIPIENT = 'sw-engr@playruyi.com,cc:chris.zhang@playruyi.com'
	}
		
	stages {
		stage('Initialize') {
			steps{
				//clean workspace
				script{
					//Never did a clean build in multi-branch pipeline task lol
					if(env.BRANCH_NAME == null && params.CLEAN_BUILD){
						deleteDir()
					}else{
						step([$class: 'WsCleanup', patterns: [[pattern: "**/Binaries/**/**", type: 'INCLUDE'],[pattern: "**/Pack/PlatformerGame/**/**", type: 'INCLUDE'],[pattern: "**/Source/PlatformerGame/lib/**/**", type: 'INCLUDE'],[pattern: "**/Source/PlatformerGame/include/**/**", type: 'INCLUDE'],[pattern: "**/.git/*.lock", type: 'INCLUDE']]])
					}
									
					checkout changelog: false, poll: false, scm: [$class: 'GitSCM', branches:  scm.branches,
						 doGenerateSubmoduleConfigurations: false, extensions: [
							[$class: 'RelativeTargetDirectory', relativeTargetDir: "${DEMO_PROJECT_ROOT}"],
							[$class: 'CleanBeforeCheckout'],
							[$class: 'CheckoutOption', timeout: 60],
							[$class: 'CloneOption', honorRefspec: true, depth: 0, noTags: true, reference: '', shallow: true,timeout: 60]
						 ], 
						 submoduleCfg: [], 
						 userRemoteConfigs: [[credentialsId: scm.userRemoteConfigs[0].credentialsId, url: scm.userRemoteConfigs[0].url]]
					]
				}
			}
			
			post {
				success {
					stage_success env.STAGE_NAME
				}
				failure {
					stage_failed env.STAGE_NAME
				}
			}
		}
		
		stage('Copy dependencies'){
			steps{
				script{
					def jobName = params.JOB_NAME
					def sel = [$class:'StatusBuildSelector',stable:false]
					
					if(params.REF_BUILD_NUMBER?.trim())
						sel = specific("${params.REF_BUILD_NUMBER}")
						
					step([$class:'CopyArtifact',filter:'RuyiSDKCpp/**/*,DevToolsInternal/**/*',target:"${TEMP_DIR}",projectName: "${jobName}",selector: sel])
					
					powershell '''
					& "$env:SCRIPTS/ue4.ps1" -Commands copy_sdk -SourceSDK $env:TEMP_DIR -DestSDK $env:DEMO_SDKCPP_ROOT
					'''
				}
			}
			
			post {
				success {
					stage_success env.STAGE_NAME
				}
				failure {
					stage_failed env.STAGE_NAME
				}
			}
		}
		
		stage('Build Libs'){
			steps{
				withEnv(["PATH+NUGET_PACKAGES=${NUGET_PACKAGES}"]){
					powershell '''
					& "$env:SCRIPTS/ue4.ps1" -Commands build -Msbuild "$env:MSBUILD" -Sln "$env:DEMO_PROJECT_ROOT/PlatformerGame.sln" -SourceSDK $env:TEMP_DIR
					'''
				}
			}
			
			post {
				success {
					stage_success env.STAGE_NAME
				}
				failure {
					stage_failed env.STAGE_NAME
				}
			}
		}
		
		stage('Cook Demo'){
			steps{
				powershell '''
				& "$env:SCRIPTS/ue4.ps1" -Commands cook -UE4 $env:UE_ROOT -SourceSDK $env:TEMP_DIR
				'''

				script {
					if (params.CODE_SIGN) {
						codeSign()
					}
				}
			}
			
			post {
				success {
					stage_success env.STAGE_NAME
				}
				failure {
					stage_failed env.STAGE_NAME
				}
			}
		}
			
		stage('Archive'){
			steps{
				script{
					bat """
						pushd ${DEMO_PROJECT_ROOT}
						git rev-parse HEAD > ${workspace.replaceAll('/','\\\\')}\\${COMMIT_ID_FILE}
						popd
						exit 0
					"""

					echo 'Start archiving artifacts ...'
					archiveArtifacts artifacts: "${COOKED_ROOT}/**/**", onlyIfSuccessful: true
				}
			}
			
			post {
				success {
					stage_success env.STAGE_NAME
				}
				failure {
					stage_failed env.STAGE_NAME
				}
			}
		}
				
		stage('Finalize'){
			steps{
				dir(TEMP_DIR){
					deleteDir()
				}
			}
			
			post {
				success {
					stage_success env.STAGE_NAME
				}
				failure {
					stage_failed env.STAGE_NAME
				}
			}
		}
	}
	
	post{
		failure{
			script{
				if(env.FAILURE_STAGE==null)
					env.FAILURE_STAGE = 'Unknown'

				if(params.MAIL_ON_FAILED){
					emailext mimeType:"text/html",
						 replyTo:"Jenkins@playruyi.com",
						 subject:"\u2639 ${env.JOB_NAME} #${env.BUILD_NUMBER} failed in [${env.FAILURE_STAGE}] stage.",
						 to:"${MAIL_RECIPIENT}",
						 body:'${JELLY_SCRIPT,template="jenkins-email-ext-clangScanReport-template.jelly"}'
				}
			}
		}
	}
}

void stage_success(stage){
	echo "[${stage}] stage successfully completed"
}

void stage_failed(stage){
	echo "[${stage}] stage failed"
	
	if(env.FAILURE_STAGE==null)
		env.FAILURE_STAGE = stage
}

void codeSign(){
	echo 'Start processing code signing'
	withCredentials([[$class: 'UsernamePasswordMultiBinding', credentialsId: 'credentials_ruyi_codesign',
			usernameVariable: 'USERNAME', passwordVariable: 'PASSWORD']]) {
		def res = powershell returnStatus: true, script: '''
			& "$env:SCRIPTS/sign.ps1" -path "$env:workspace/$env:COOKED_ROOT" -Tools $env:CODESIGNING_HOME/x64/ -password $env:PASSWORD -certificate $env:CODESIGNING_HOME/RUYI-CERT.pfx
			'''
		if (res != 0) {
		    currentBuild.result = 'UNSTABLE'
		}
	}
}