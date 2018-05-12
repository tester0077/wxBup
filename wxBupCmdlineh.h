//-------------------------------------------------------------------
// Name:        wxBupCmdlineh.h
// Purpose:     command line parser header for wxBup
// Author:      taken partly from wxWidgets wiki
// Copyright:   (c) ??
// Licence:     wxWidgets licence
//-------------------------------------------------------------------
// ------------------------------------------------------------------ 
// command line processing

#if defined (WANT_CMD_LINE)
#include "wx/cmdline.h"
// ------------------------------------------------------------------ 
// command line processing - definitions

static const wxCmdLineEntryDesc g_cmdLineDesc[] =
{
  { wxCMD_LINE_SWITCH, "h", "help", "displays help on the command line parameters" },
  { wxCMD_LINE_SWITCH, "?", "help", "displays help on the command line parameters" },
  { wxCMD_LINE_OPTION, "B", "backup", "backup job file", wxCMD_LINE_VAL_STRING },
  { wxCMD_LINE_OPTION, "b", "bagname", "bag base name", wxCMD_LINE_VAL_STRING },
  { wxCMD_LINE_SWITCH, "c", "copyonly", "defaults to zip/copy" },
  { wxCMD_LINE_SWITCH, "C", "console", "console mode, no GUI" },
  { wxCMD_LINE_OPTION, "d", "destroot", "Destination root", wxCMD_LINE_VAL_STRING },
  { wxCMD_LINE_OPTION, "e", "excludes", "List of exclude patterns", wxCMD_LINE_VAL_STRING },
  { wxCMD_LINE_SWITCH, "l", "log", "add logs to bags", wxCMD_LINE_VAL_STRING,
                                                          wxCMD_LINE_SWITCH_NEGATABLE },
  { wxCMD_LINE_SWITCH, "m", "moveonly", "List of move-as-is patterns", wxCMD_LINE_VAL_STRING },
  { wxCMD_LINE_OPTION, "n", "name", "name of new job file", wxCMD_LINE_VAL_STRING },
  { wxCMD_LINE_SWITCH, "q", "quiet", "no questions asked" },
  { wxCMD_LINE_OPTION, "s", "sources", "List of sources", wxCMD_LINE_VAL_STRING },
  { wxCMD_LINE_OPTION, "t", "type", "media type - bag size", wxCMD_LINE_VAL_STRING },
  { wxCMD_LINE_OPTION, "T", "test", "Test the given path against the original", wxCMD_LINE_VAL_STRING },
#if defined( WANT_CMDLINE_V )  
  { wxCMD_LINE_SWITCH, "V", "version", "print version" },
#endif
  { wxCMD_LINE_OPTION, "v", "verbosity", "specify verbosity level", wxCMD_LINE_VAL_NUMBER },
  // no parameters allowed
//  { wxCMD_LINE_PARAM, NULL, NULL, "input file",   // allow multiple optional files or directories
//      wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE },
  { wxCMD_LINE_NONE }
};

#endif
// ------------------------------- eof ------------------------------ 

