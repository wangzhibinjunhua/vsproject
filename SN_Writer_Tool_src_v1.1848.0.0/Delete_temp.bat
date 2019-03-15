
:: delete all temp file, for release source code and check-in to code database

@echo delete all temp file...

del /f /s /q Output\SN_Writer_d*
del /f /s /q Output\*.pdb
del /f /s /q Output\MtkCommIODLL.log

del /f /s /q /a:h *.ncb
del /f /s /q /a:h *.user
del /f /s /q /a:h *.suo

copy /b Output\SN_Setup.ini.bak Output\SN_Setup.ini
