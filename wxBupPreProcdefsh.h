/*-----------------------------------------------------------------
 * Name:        wxBupPreProcDefsh.h
 * Purpose:		  by including this file in the main project app header
 *              it is hoped that all sub-projects will be compiled with
 *              the same #defines
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *-------------------------------------------------------------- */
#ifndef _WX_BUP_PRE_PROC_H
#define _WX_BUP_PRE_PROC_H
/*
 * Move project #define value to a separate header file
 * remove them from the Project Properties -> Preproc
 * and add them to separate header file for inclusion with all other
 * source files for all sub-projects
 *
 * This, hopefully, prevents the problem of different #defines because
 * some project preprocessor values are not updated consistently
 *
 * Also defining the wxWidgets version to be used with this project
 * makes it simpler to port between versions, allowing the generic
 * WXWIN to be used
 * Does NOT work !!-???? can't find included files
 */
// ------------------------------------------------------------------
#if !defined( wxUSE_UNICODE )
#define wxUSE_UNICODE
#endif
#if !defined( wxUNICODE )
#define wxUNICODE
#endif
// ------------------------------------------------------------------
#define _CRT_SECURE_NO_WARNINGS
#define EMAIL_ATTACHMENT_LIMIT  (5 * 1024 * 1024) // 5 MB
#define WS_LOG_FILE_NAME _("wxBupLog.txt")
#define COPYRIGHT_YEAR_1 2016;
// ------------------------------------------------------------------

#define WANT_BACKUP_TYPE
#define WANT_SINGLE_INSTANCE
#define WANT_DND_NO
#define WANT_STD_PATHS
#define WANT_CMD_LINE
#define WANT_HELP
#define WANT_ZIP_HELP_NO
#define WANT_LOG
#define WANT_FILEVERSION
#define WANT_EXE_LINK_DATE
#define WANT_VERSION_IN_TITLE
#define WANT_DBGRPRT
#define WANT_TRAVERSER_COUNT
#define WANT_LOG_CHAIN
#define WANT_FILEHISTORY
#define WANT_CONFIG_APP
#define EXIT_EVENT
#define LOG_EVENT
#define RESTORE_EXIT_EVENT
#define WANT_WXFILE
// if enabled, these shorten the overall destination path length
#define WANT_SOURCE_VOLUME_NO
#define WANT_FULLSOURCE_PATH_NO
#define WANT_COUNT_IN_THREAD
#define WANT_CLEANUP_IN_THREAD
#define WANT_GRID_NO
#define WANT_SVN_REV_NO
#define WANT_MULTIPLE_SOURCE
#define WANT_TREEBOOK
#define WANT_APP_DND
#define WANT_DIRTY_VALIDATE_NO
#define WANT_SQLITE_NO
#define WANT_NEW_TITLE
#define WANT_DATE_DIR     // include {date} in dest'n root dir
#define WANT_SOUND
#define WANT_MEMORY_SOUND_NO
#define WANT_EXCLUSION_CLASS
#define WANT_CURL
#define WANT_LAST_JOB_DATE
#define CURL_STATICLIB      // needed for libcurl - static
#define WANT_CMDLINE_V_NO   // make it easy to try or disable the -V version
                            // command line option - it does nothing - yet
#define WANT_TEMP_CHECK     // check temp directory validity
#define WANT_POWER_BLOCKER  // we want the code handling the power resource blocker
                            // which is supposed to block users shutting down while a
                            // backup job is in progress - not yet implemented for restore

#define WANT_NAME_LEN_CHECK_IN_COUNT  // decide where to do the check for file & path lengths
                                      // in wxBupSplit or during initial file count
#define WANT_LOG_LIMIT      // enforce a log size limit
                            // make it the same as EMAIL_ATTACHMENT_LIMIT less .5 MB
// ------------------------------------------------------------------
// version specific #defines
#if defined (_DEBUG )
#  define CHECK_4_UPDATE_URL _T("http://???????/wxMSversion.php");
#define WANT_UT
#else
#  define CHECK_4_UPDATE_URL _T("http://???????/wxMSversion.php");
#define NDEBUG              // seems to be required to avoid wxASSERT
                            // code in release version; see:
                            // http://wxwidgets.blogspot.com/2009/09/debug-build-changes-in-wx3.html
#endif  // _DEBUG

#if !defined( _MSC_VER )  // overrides for linux
#if !defined ( HAVE_STDINT_H )
#  define HAVE_STDINT_H
#endif
#undef WANT_FILEVERSION
#undef WANT_UT
#undef WANT_EXE_LINK_DATE
#undef WANT_VERSION_IN_TITLE
#endif

#endif  // _WX_BUP_PRE_PROC_H
// ------------------------------- eof ---------------------------
