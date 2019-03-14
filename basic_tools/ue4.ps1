Param(
    [Parameter(HelpMessage = "Path to root of UE4 project")]
    [string]
    $ProjectRoot = "$PSScriptRoot/..",
    [ValidateSet('copy_sdk', 'build', 'cook')]
    [string[]]$Commands = @('copy_sdk', 'build', 'cook'),

    [Parameter(Mandatory = $true, HelpMessage = "Path to copy C++ SDK and DevToolsInternal from (e.g. output/Release/)")]
    [string]
    $SourceSDK,
    [Parameter(HelpMessage = "e.g. Source/PlatformerGame/RuyiSDK/")]
    $DestSDK,
    
    [Parameter(HelpMessage = "Path to UE4 engine")]
    [string]
    $UE4 = "C:/Program Files/Epic Games/UE_4.21/Engine",
    [string]
    $Msbuild = "msbuild",
    [Parameter(HelpMessage = "Visual Studio .sln file")]
    [string]
    $Sln,
    [string[]]
    $Configurations = @("Development Editor", "Development", "Shipping"),
    [Parameter(HelpMessage = "UE4 .uproject")]
    [string]
    $Project,

    [Parameter(HelpMessage = "Output path for cooked data")]
    [string]
    $CookedOutput = "$ProjectRoot/Pack"
)


$CppSDK = "$SourceSDK/RuyiSDKCpp"
if (-not (Test-Path $CppSDK -PathType Container)) {
    Write-Error "C++ SDK not found: $CppSDK" -ErrorAction Stop
}
if (!$Project) {
    $Project = (Get-ChildItem -Filter *.uproject $ProjectRoot | Select-Object -First 1).FullName
    Write-Output "Using UE4 project: $Project"
}
if (!$Sln) {
    $Sln = Get-ChildItem -Filter *.sln $ProjectRoot | Select-Object -First 1
    Write-Output "Using VS Soluiton: $Sln"
}

function Build {
    param(
        [string]$Config
    )
    & "$Msbuild" "/t:restore;build" "$Sln" "/m:4" "/p:Configuration=`"$Config`"" "/p:Platform=Win64"
}

function CopyDlls {
    param(
        [string]$Dest
    )
    Get-ChildItem -Recurse -Filter *.dll $CppSDK/lib | ForEach-Object($_) {
        Write-Output "Copying $($_.FullName) to $Dest"
        Copy-Item $_.FullName "$Dest/$($_.Name)" -Force -ErrorAction Ignore
    }
}

if ($commands.Contains("copy_sdk")) {
    Write-Output "Copying SDK from $SourceSDK to $DestSDK.  Be patient, this takes ~1 minute"
    Copy-Item -Force -Recurse -Path $CppSDK/lib,$CppSDK/include -Destination $DestSDK
}

if ($commands.Contains("build")) {
    Write-Output "Generating VS project files..."
    & "$UE4/Binaries/DotNET/UnrealBuildTool.exe" -projectfiles -project="$Project" -CurrentPlatform -2017 -game -rocket -progress
    foreach ($config in $Configurations) {
        Write-Output "Building $config..."
        Build $config
    }
}

if ($commands.Contains("cook")) {
    CopyDlls $ProjectRoot/Binaries/Win64/
    Write-Output "Cooking $Project to $CookedOutput..."
    Remove-Item -Recurse -Force -ErrorAction Ignore $CookedOutput
    & "$UE4/Build/BatchFiles/RunUAT.bat" BuildCookRun -project="$Project" -noP4 -platform=Win64 -clientconfig=Development -serverconfig=Development -cook -maps=AllMaps --NoCompile -stage -pak -archive -archivedirectory="$CookedOutput"
    CopyDlls $CookedOutput/WindowsNoEditor
}
