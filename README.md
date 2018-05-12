# wxBUP
## Backup Utility

##### I am posting this code to Github to, hopefully, make it useful for others because it does what I need at present & I no longer have the time for further major development. Please use the code as you see fit. I will try to help with getting anyone to compile the project under Windows, but my time is limited ...

#### All of my code is released under the wxWidgets license, the rest (mimetic & libcurl), under whichever license it was released.

Because all of the various backup programs, both free and commercial always left me either wishing for features I missed (sometimes badly) or frustrated because when it came time to try and restore some lost files the current and latest version was either unable to or way too inconvenient to restore what I needed. Either the backup format used when I backed up the files was no longer supported (and one would need to have the then current version of the backup utility still installed) or the only way I could restore just a small subset of a backup job was to clear enough disk space to accommodate extracting the complete set and then hunting through the restored data for the files I wanted.

As a result of all the accumulated frustration, here is yet another backup utility - but hopefully with enough differences to make it useful for others as well.

- The main difference and advantage, IMHO, of wxBUP is that it can pack specifiable media sizes, such a CD or DVDs, as fully as possible. The files which are already in a compressed format, such as JPEG or even existing zipped files are simply copied to the destination.
- you can chose the size of the target medium. Files will be packed, starting with the largest ones first and then the next largest which will still fit and so on.
- you can specify which files should be considered already compressed.
- you can specify which files or extensions are to be skipped altogether.
- along with the backed up files, the utility stores an inventory, which could be used to make it easier to restore. This has not been implemented yet, because it is easy enough to peruse the contents or search it with other available utilities.
- you can ask the program to run a file by file comparison when the backup is completed as extra insurance.
- work has started to take into account the reality that many source and even destination disk drives end up with different drive letters, at least under Windows, because they are removable.
- when a particular job finishes, wxBUP an be configured to post a completion message to a user controlled e-mail address, including, possibly the job's log file.
- basic code to stop specifiable processes is part of the application and is mainly intended to allow backing up data from processes which typically are expected to be running most of the time. In my case, this is Thunderbird and allows me to automatically stop it when the time comes to backup its data. Restarting of these tasks, is left as an exercise for the 'reader' :-)
- Shadow Copy is NOT (yet) part of the application.
- the selection of files to be backed up, is still rather primitive, but works for me.
- at the end of a job, the utility can be set to shut down the machine or ....

The accompanying help file gives more details.

## Prerequisites for the wxBUP project

1. wxWidgets 3.1.0 - compiled with MSVC 2015 for static linking. 
	Uses the environment variable $(WXWIN_310_2015) = path to the wxWidgets root

2. MSVC 2015 IDE - Express - Community version

3. Inno Install builder from http://www.jrsoftware.org/isinfo.php - uses the ISS package 
	from github.com/stfx/innodependencyinstaller. A useful helper script to control the Microsoft re-distributable files also came originally from CodeProject see: https://www.codeproject.com/Articles/20868/NET-Framework-Installer-for-InnoSetup.
 I believe the latest code has now moved to Github: https://github.com/stfx/innodependencyinstaller The .sln file is set up to copy the executable, the help file as well as any documentation files to the directory ./Inno/src and the resulting executable will be left in ./Inno/bin

4. Appropriate MSVC distributable package. The files are expected to be left in ./Inno/bin/myProgramDependencies. These are typically downloaded from Microsoft. Copies of the necessary files for the version of the MSVC IDE are part of the package.

5. RCStamp utility - the executable RCStamp.exe is included in the ./Executables directory 
and is used as part of the 'release' build process to update the 
major, minor & build numbers. To modify either by more than one increment or to modify them in any other way, it is best to edit the .rc file by hand to change any one or all to the new starting value. But note, all 'copies' of each number in the .RC must be modified in sync.
It is a command line utility and you can get some help by running it in a DOS window. You will want to move or copy it to a directory on the PATH so that Windows can find it or you can add the full path in the 'release' build pre-link build event.
It is an adaptation of a CodeProject utility and you can find my modified source and more comments at
tester0077/rcstamp.

6. Checksum - Jem Berkes' ver 1.2 MD5sums.exe -- http://www.pc-tools.net/win32/md5sums/

7. UPX file compressor - UPX 3.03w Markus Oberhumer, Laszlo Molnar & John Reiser   Apr 27th 2008 see https://upx.github.io/

8. HelpNDoc - The help files are built using the free version of HelpNDoc from https://www.helpndoc.com/
If you will be using the application for anything other than personal use, you may have to
buy a license or use some alternative means to build the help files. Using this application is entirely optional if you want to produce the help files manually or have some other application. You can inspect the .SLN and project files to see how HelpNDoc is integrated to create the help file as part of a build.

9. wxCrafter - The GUI is built using a licensed version of wxCrafter. It is expected that all features used by this project are covered by the free version, if not you'd have to use 
a different GUI builder of your choice or modify things by hand.

## Other Details
- A small PHP file is included in the directory wxBupVer. It is intended to be placed on a 
web server into the directory %HtmlRoot%/wxNsVer where the version string can be updated as needed and where the application
can check for newer versions. This is optional and can/could be dropped. The Web site URLs are set up in wxMsPreProcDefs.h and a different URL can be specified for the debug and release versions.

- All MSVC preprocessor directives have been moved to the file wxMSPreProcDefsh.h to ensure all 
are up-to-date for all sub projects. Any changes should be made in this file and doing so
will avoid issues with forgetting to propagate all changes to all sub-projects.
This file MUST be included in every source and MUST be the first included file. 

 - verLib - a small library used to obtain the application version information from the 
executable at run-time to display as part of 'About' dialog or wherever needed.

- exeHdrLib - a small library used to get information about the executable, in this case the linker date to get an accurate date for the build.

 - pre-compiled version of both my current version executables as well as the Inno installer executable for the same version along with the md5 checksum files can be found in the directory ./Executables.
 
- wxBUP keeps its configuration information in a directory in %USER%, specifically in 
C:\Users\userName\AppData\Roaming\wxBUP. The directory will be created if it does not exist. These files will be wxMS.INI (configuration information and), the help file wxBupHelp.chm, the install instructions and the change log, as well as the log file wxBupLog.txt. 

- the configuratuions for each backup 'job' will normally also be created and maintained in this directory.

- All preprocessor directives have been moved to the file wxMsPreProcDefsh.h to make it easier to keep them all in sync between the main as all sub projects.

