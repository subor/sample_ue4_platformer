Since not everyone is familiar with the Unreal Engine 4 package procedure, I'll just roughly give a insturction here. You can find more info on 
Epic offical website, current address is https://docs.unrealengine.com/latest/INT/Engine/Basics/Projects/Packaging/index.html

1 Build project in VS with "Shipping" and "Development" config
2 Launch "ProjectLauncher" on menu icon bar. 
3 Create a new Launch Profile
4 Config the custom launch profile
(1) Project part: Set the "Project" to your Unreal project file
(2) Build part: hook nothing
(3) Cook part: "Cooked Platforms:" hook "WindowsNoEditor"
               "Cooked Cultures": hook "en-US"
               "Cooked Maps": hook nothing
               "Release/DLC/Patching Settings": hook and fill nothing
               "Advanced Settings": hook "Compress content", "Save packages without versions", "Store all content in a single file(UnrealPak)"
                                    choose "Shipping" in "Cooker build configuration"
(4) Package part: "How would you like to package the build?" choose "Package & store locally"
                  the path you can just use default one, which the built files will be store in "ProjectFolder/Saved/StagedBuilds"
                  hook "Is this build for distribution to the public"
(5) Archive part: hook nothing
(6) Deploy part: chose "Do not deploy"
5 then go back to Custom Launch Profile interface and click the launch profile icon of the profile, wait for several minutes.
if everything goes well, you can find all built files in "ProjectFolder/Saved/StageBuild/WindowsNoEditor"
6 Copy all those files and "libzmq.dll" in sdk files to the Project folder, eg here "RuyiSDKDemo" folder.
  Then package this folder with "res" folder and "RuyiManifest.json" file into a zip file. Make sure the information
  in .json file match your built file.