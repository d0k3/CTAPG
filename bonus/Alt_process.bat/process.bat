@echo off
md zzz_catalog
md zzz_catalog\412x480
md zzz_catalog\full_size
for /D %%x in (%1) do (
	ECHO .
	ECHO PROCESSING FOLDER %%x...
	IF EXIST "%%x"\body_LZ.bin (
		cd "%%x"
		..\DSDecmp -d body_LZ.bin body_LZ.dec
		..\3DSthmXtb body_LZ.dec
		..\nconvert -quiet -overwrite -out png -o "..\zzz_catalog\412x480\%%~nx.png" preview.bmp
		..\nconvert -quiet -overwrite -out png -o "..\zzz_catalog\full_size\%%~nx.png" preview_ext.bmp
		..\nconvert -quiet -overwrite -out png -o preview.png preview_chmm.bmp
        REM ..\nconvert -quiet -overwrite -crop 136 96 48 48 -out png -o icon.png preview_chmm.bmp
        ..\smdhtool --create "%%~nx" "preview generated with CTAPG by d0k3" "unknown" "icon.png" "info.smdh" 
		REM del /Q body_LZ.dec icon.png *.bmp
        del /Q body_LZ.dec *.bmp
		cd ..
		ECHO ...DONE!
	) else (
		ECHO NOT A CUSTOM THEME FOLDER
	)
	ECHO .
)