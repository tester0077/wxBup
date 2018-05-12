:: Converted to a batch file
:: %1 <- $(SolutionDir)
:: %2 <- $(SolutionName)
:: %3 <- $(Configuration)

::  copy the files to the Inno Installer Script src directory 
::::: NOTE the copy commands will NOT tolerate the back slash '/'
:: help file
:: J:\pkg\wx\MSVC2015\_3.1-2010\HummerSVN\wxHex>buildInno  J:\pkg\wx\MSVC2015\_3.1-2010\HummerSVN\wxHex wxHex RelUniStat
echo on
if  not exist %1Inno\src mkdir %1Inno\src\
copy %1help\%2Help.chm %1Inno\src\
:: remove the old executable
if exist %1Inno\src\%2.exe del %1Inno\src\%2.exe
:: compress the executable & copy it
echo upx: c:\bin\upx  %1\%3/%2.exe -o   %1\Inno\src\%2.exe
c:\bin\upx   %1\%3\%2.exe -o   %1\Inno\src\%2.exe
:: The License text
echo copy license: %1doc\%2-License.txt %1Inno\src\
copy %1doc\%2-License.txt %1Inno\src\
:: the readme file
echo copy WhatsNew: %1doc\%2Whats-New.txt %1Inno\src\
copy %1doc\%2-WhatsNew.txt %1Inno\src\
:: the install file
echo copy Install: %1doc\%2-Install.txt %1Inno\src\
copy %1doc\%2-Install.txt %1Inno\src\
:: sound files
echo copy Sound: %1sound\Alarm10.wav %1Inno\src\
copy %1sound\Alarm10.wav %1Inno\src\
echo copy Sound: %1sound\foghorn-daniel_simon-short.wav %1Inno\src\
copy %1sound\foghorn-daniel_simon-short.wav %1Inno\src\
echo copy Sound: %1sound\railroad_crossing_bell-Brylon_Terry-7.wav %1Inno\src\
copy %1sound\railroad_crossing_bell-Brylon_Terry-7.wav %1Inno\src\
echo copy Sound: %1sound\Ring10.wav %1Inno\src\
copy %1sound\Ring10.wav %1Inno\src\
:: Now compile    <<<<< can't have bare '::' on a line
:: the
:: Inno Setup Script to build the installer
:: echo
if exist "c:/Program files (x86)/Inno Script Studio/ISStudio.exe" goto compile
echo Can't find "c:/Program files (x86)/Inno Script Studio/ISStudio.exe"
echo 
goto end
:compile
if exist %1\Inno\setup.iss goto compile2
echo Can't find: %1\Inno\setup.iss
echo 
goto end
:compile2
echo compiling: %1\Inno\setup.iss
echo Command: "c:\Program files (x86)\Inno setup 5\compil32.exe" 
"c:\Program files (x86)\Inno Setup 5\compil32.exe"/cc %1\Inno\setup.iss
:: echo
:: MUST use Jem Berkes' ver 1.2 MD5sums.exe to get the plain output I want.
:: first delete the old files otherwise we get leftovers
echo deleting %1Inno\bin\%2_exe.md5
if exist %1Inno\bin\%2_exe.md5 del %1Inno\bin\%2_exe.md5
echo deleting %1Inno\bin\%2_Setup.md5
if exist %1Inno\bin\%2_Setup.md5 del %1Inno\bin\%2_Setup.md5
::
echo md5 1: %1Inno\src\%2.exe
c:\bin\md5sums -u %1\Inno\src\%2.exe >  %1Inno\bin\%2_exe.md5
echo md5 2: %1Inno\bin\%2-*.exe
c:\bin\md5sums -u %1Inno\bin\%2-*.exe > %1Inno\bin\%2_Setup.md5
:end

