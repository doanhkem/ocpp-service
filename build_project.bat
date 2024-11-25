@REM tiến hành build
echo START BUILD && echo . && pio run
@REM tiến hành upload
echo START UPLOAD && echo . && pio run --target upload
@REM tiến hành serial monitor
echo START RUN && echo . && pio device monitor