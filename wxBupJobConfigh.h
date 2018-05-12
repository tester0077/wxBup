/*-----------------------------------------------------------------
 * Name:        wxBupJobConfigh.h
 * Purpose:
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *-------------------------------------------------------------- */
#ifndef _WXBUP_JOB_CONFIG_H
#define _WXBUP_JOB_CONFIG_H
#include "wxBupConfh.h"
// Pref version
#define CURRENT_JOB_PREF_VERSION  1
// ------------------------------------------------------------------
/* Note: these entries define the sequence in iniPrefs
 */
enum iniJobEnts
{
  IEJ_JOB_MIN  = 0,
  IEJ_JOB_NAME = IEJ_JOB_MIN, // job name only - no path, no extension
  IEJ_SOURCE_PATHS,
  IEJ_DESTN_ROOT_PATH,
  IEJ_DESTN_USE_DRIVE_LETTER,
  IEJ_DESTN_USE_VOL_NAME,
  IEJ_BAG_NAME_BASE,
  IEJ_BAG_TYPE,
  IEJ_EXCLUSION_LIST_FILES,
  IEJ_EXCLUSION_LIST_DIRS,
  IEJ_JUST_MOVE_LIST,
  IEJ_COPY_ONLY_WANTED,       // zip or copy only
  IEJ_INTERFERING_TASKS,
  IEJ_INCL_MT_DIRS,
  IEJ_JOB_ENABLED,
  IEJ_INCL_SUB_DIRS,
  IEJ_BACKUP_TYPE,            // full or differential - defaults to 'Full"
  IEJ_VERIFY_JOB,             // true if we are to verify the job when completed
  IEJ_JOB_COMMENT,            // description/explanation of job's purpose/limitations
#if defined( WANT_LAST_JOB_DATE )
  IEJ_LAST_BACKUP_DATE,       // last backup date
#endif
  IEJ_EMAIL_WANTED,           // email notificatiion wanted
  IEJ_EMAIL_ERROR_ONLY,       //   "   for errors only
  IEJ_EMAIL_ADD_LOG,          //   "   attach log
  IEJ_EMAIL_SMTP,             //   "   SMTP URL
  IEJ_EMAIL_FROM,             //   "   'From' line
  IEJ_EMAIL_TO,               //   "   'To' line
  IEJ_EMAIL_SUBJECT,          //   "   'Subject' line

  // new dest'n dir options
  // true => use serial number, false => use volume name by default
  // can be overridden by user
  IEJ_DEST_DEFAULT_SERNO_OR_VOL,
  // true => use serial number, false => use volume name
  IEJ_DEST_USE_SERNO_OR_VOL,
  // volume name, if any - not drive letter. which can change for USB drives
  IEJ_DEST_VOL_NAME,
  // serial number as string 
  IEJ_DEST_SER_NO,
  

  IEJ_JOB_MAX                 // keep as last - determines size of struct iniPrefs
};

typedef struct _jobPrefs_t
{
  long      lPrefVer;   // magic number
  entry     data[IEJ_JOB_MAX];
} jobPrefs_t; 

#endif  // _WXBUP_JOB_CONFIG_H
// ------------------------------- eof ---------------------------

