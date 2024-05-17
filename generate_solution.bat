@echo off

pushd %~dp0
call ..\utils\sharpmake\Sharpmake.Application.exe /debugSolutionPath('project_files/sharpmake') /sources('paw6.sharpmake.cs') /generateDebugSolution
popd