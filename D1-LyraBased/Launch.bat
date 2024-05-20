@REM dotnet "C:\Program Files\Epic Games\UE_5.4\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.dll" ^
@REM -ProjectFiles -Automated -Minimize -Platforms=Win64 -TargetTypes=Editor -TargetConfigurations=Development "%~dp0D1.uproject"

call "C:/Program Files/Epic Games/UE_5.4/Engine/Build/BatchFiles/Build.bat" ^
-projectfiles -project="%~dp0\D1.uproject" -game -rocket -progress

call "C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\Build.bat" ^
D1Editor Win64 Development -Project="%~dp0\D1.uproject" -WaitMutex -FromMsBuild -architecture=x64

call "%~dp0\D1.uproject"