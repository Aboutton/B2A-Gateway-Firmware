@echo off

if "%~1"=="" (
    set /p MESSAGE="Enter commit message: "
) else (
    set MESSAGE=%*
)

git add .
git commit -m "%MESSAGE%"
git push

echo Done!
pause
