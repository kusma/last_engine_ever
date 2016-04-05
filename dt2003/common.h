#ifndef COMMON_H
#define COMMON_H

#define dit (*this)

#define TRACKCOUNT 19
enum Tracks { tPart, tCamPosX, tCamPosY, tCamPosZ, tCamTarX, tCamTarY, tCamTarZ, tCamRelative, 
tBlurPasses, tBlurStrength, tFlash, tFadeOut, tCamSelector, tCamRoll, tKwlOuter, tKwlInner, tKwlOuterR, tKwlInnerR, tMarsShow };
enum TrackTab { ttCam, ttBlur, ttFlash, ttKwl };

#define ORDERDATASIZE 0x100

#define MAXPTNLEN 0x100
#define MAXPTNS 0x100
#define TRACKLENGTH (MAXPTNLEN * MAXPTNS)
#define TRACKDATASIZE (TRACKCOUNT*TRACKLENGTH)
#define MAXTRACKS 60
#define FILENAME "PANTOFFELDIERTJES_ARE_GOOD"
#define FILEHEADER "PANTOFFELDIERTJES ARE JUST TOTALLY OKAY!"
#define THE_DEMOTITLE "POO BY LIMP NINJA"
#define THE_TRACKERTITLE "THE TRACKER!!!"
#define BPM 150
#define MOO "MOOoo"


/*
#ifdef TRACKER
#define T_OFFSET 1
#else
#define T_OFFSET 0
#endif

#define T_ABSPOS (T_OFFSET + 0)
#define T_RELPOS (T_OFFSET + 3)
#define T_ABSTAR (T_OFFSET + 6)
#define T_RELTAR (T_OFFSET + 9)
#define T_ROLL (T_OFFSET + 12)
#define T_FX1 (T_OFFSET + 14)
#define T_FX2 (T_OFFSET + 15)
#define T_CS (T_OFFSET + 16)
#define T_LS (T_OFFSET + 17)
*/

//effects
#define RELTOPOS 0x20
#define RELTOORG 0x10

#define CM_MOVEPOS (WM_USER + 0x101)
#define CM_PAUSE (WM_USER + 0x102)
#define CM_RELOAD (WM_USER + 0x103) 
#endif