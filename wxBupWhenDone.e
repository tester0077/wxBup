/*-----------------------------------------------------------------
 * Name:        wxBupWhenDone.e
 * Purpose:
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 * Note:        All the common entries are now in ECommonTypes
 *              with the string types in EStringTypes
 * enum to handle the action when wxBup has finished a job
 *-------------------------------------------------------------- */
enum EWhenDoneTypes {
  // 
  eWhenDone_Unknown    = 0,   // unknown
  eWhenDone_DoNothing,        // do nothing
  eWhenDone_Exit,             // exit
  eWhenDone_Hibernate,        // hibernate
  eWhenDone_LogOff,           // logoff
  eWhenDone_Shutdown,         // shutdown
//  eWhenDone_Sleep,            // sleep  // don't know how to do this with wxWidgets - yet
};

// ------------------------------- eof ---------------------------

