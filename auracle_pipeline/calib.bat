cls
@DEL /S /Q /F "rec\*.*"
..\bin\Win32\vc10\xmlpipe.exe theeyeyetribe-calib.pipeline

set hour=%time:~0,2%
if "%hour:~0,1%" == " " set hour=0%hour:~1,1%
echo hour=%hour%
set min=%time:~3,2%
if "%min:~0,1%" == " " set min=0%min:~1,1%
echo min=%min%
set secs=%time:~6,2%s
if "%secs:~0,1%" == " " set secs=0%secs:~1,1%
echo secs=%secs%
set year=%date:~-4%
echo year=%year%
echo date=%date%
set month=%date:~-10,2%
if "%month:~0,1%" == " " set month=0%month:~1,1%
echo month=%month%
set day=%date:~-7,2%
if "%day:~0,1%" == " " set day=0%day:~1,1%
echo day=%day%
set outdir=data\calib_%year%-%month%-%day%_%hour%-%min%-%secs%\
@echo "store recorded data..."
@mkdir %outdir%
@move rec\*.stream~ %outdir%
@move rec\*.stream %outdir%
@move rec\*.events %outdir%
@echo "done! data moved to:" 
@echo %outdir%
