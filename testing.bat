@echo off
ipconfig /flushdns

for /L %%i in (1,1,510) do (
    echo Pinging %%i.com ...
    ping -n 1 %%i.com
)
echo Done.
pause