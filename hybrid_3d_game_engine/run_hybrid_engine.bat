@echo off
setlocal
cd /d "%~dp0"
dotnet run --project "csharp/Hybrid3DGame/Hybrid3DGame.csproj" -- 8 --interactive
pause
