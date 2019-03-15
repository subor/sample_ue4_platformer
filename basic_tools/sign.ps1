param (
    [Parameter(HelpMessage = "Windows SDK directory containing signtool.exe.  Only used if SignTool is ''")]
    [string]$SignTool = "C:\Program Files (x86)\Windows Kits\10\bin\x86\",
    [Parameter(HelpMessage = "Directory containing Subor SDK copy of signing binaries")]
    [string]$Tools = "$PSScriptRoot/windowsKits/",
    [Parameter(Mandatory=$true)][string]$password,
    [Parameter(Mandatory=$true)][string]$certificate,
    [string[]]$patterns = @("*.exe","*.dll","*.sys","*.cat","*.ocx"),
    [string]$path = "./output/Release/",
    [Parameter(HelpMessage = "Number of jobs that are allowed to run in parallel")]
    $Jobs = 2
)

# Make sure certificate exists and is a file/"leaf" and not a directory...
if (!(Test-Path $certificate -PathType Leaf)){
    Write-Error "$certificate not found"
    exit 1
}
if (!(Test-Path $path -PathType Container)){
    Write-Error "$path not found"
    exit 1
}

Push-Location $path

# Save current working directory so we can make all jobs run from the same place
$location = Get-Location
$total_start = Get-Date

if (!$Tools) {
    $Tools = $SignTool
}
if (!(Test-Path $Tools/signtool.exe -PathType Leaf)){
    Write-Error "$Tools/signtool.exe not found" -ErrorAction Stop
}
if (!(Test-Path $Tools/delcert.exe -PathType Leaf)){
    Write-Warning "$Tools/delcert.exe not found.  Will be unable to re-sign already signed files."
}

Get-Job | Remove-Job # Remove existing jobs
foreach ($pattern in $patterns) {
    $pattern_start = Get-Date
    Write-Output "Starting $path $pattern..."
    $dirs = Get-ChildItem -Recurse -Directory
    foreach ($dir in $dirs) {
        # Skip directories that don't contain any files matching the pattern
        $files = Get-ChildItem $dir.FullName $pattern | Resolve-Path -Relative
        if (!$files) {
            continue;
        }

        # Wait for one of the jobs to finish
        while ($(Get-Job -State Running).count -ge $Jobs) {
            Start-Sleep -Milliseconds 100
        }
        # This pushes job's stdout/stderr to console
        Get-Job | Receive-Job

        $job_script = {
            param (
                $location, $Tools, $certificate, $password, $files
            )
            # Make sure all jobs are running from same working directory
            Set-Location $location
            # Sign all files that match the pattern in the directory
            $cmd = "`"$Tools/signtool.exe`" sign /f $certificate /p $password /fd sha256 /tr http://sha256timestamp.ws.symantec.com/sha256/timestamp /as /v " + $files
            $res = Invoke-Expression "& $cmd 2>&1"
            # Check if failed to sign, but may be able to delete cert and sign.
            # Using `signtool.exe remove` fails with 0x00000057, so we use delcert.exe like devtool's signtool
            if ($LASTEXITCODE -and ($res -match "0x800700C1")) {
                & $Tools/delcert.exe $files
                # Try re-signing
                Invoke-Expression "& $cmd"
            }
        }
        Start-Job $job_script -ArgumentList @($location, $Tools, $certificate, $password, $files) | Out-Null
    }
    
    $duration = New-TimeSpan -Start $pattern_start -End (Get-Date)
    Write-Output "Duration of $pattern : $duration"
}

# Wait for all jobs to finish and clean them up
while ($(Get-Job -State Running)){
    Start-Sleep 1
}
Get-Job | Receive-Job
Get-Job | Remove-Job

Pop-Location

$duration = New-TimeSpan -Start $total_start -End (Get-Date)
Write-Output "Total Duration: $duration"