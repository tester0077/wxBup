/*-----------------------------------------------------------------
 * Name:        wxBupBagSizeTypes.e
 * Purpose:
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 * Note:        All the common entries are now in ECommonTypes
 *              with the string types in EStringTypes
 *-------------------------------------------------------------- */
enum EBupBagSizeTypes {
  // 
  
  eBagSize_Unknown    = 0,  // unknown
  eBagSize_NoSplit,         // Single Volume, no split
  eBagSize_Custom,          // Custom size
  eBagSize_144MB,           // 1.44 MB Floppy
  eBagSize_5MB,             // 5 MB Attachment
  eBagSize_10MB,            // 10 MB Attachment
  eBagSize_650MB,           // 650 MB CD
  eBagSize_700MB,           // 700 MB CD
  eBagSize_3_7GB,           // 3.7 GB DVD
  eBagSize_4_7GB,           // 4.7 GB DVD
  eBagSize_8_5GB,           // 8.5 GB DVD-DL
  eBagSize_25GB,            // 25 GB Blueray    
};

// ------------------------------- eof ---------------------------
