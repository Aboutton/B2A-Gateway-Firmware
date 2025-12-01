@echo off
if "%~1"=="" (
    set MESSAGE=Update
) else (
    set MESSAGE=%*
)

git add .
git commit -m "%MESSAGE%"
git push

echo Done!
pause
