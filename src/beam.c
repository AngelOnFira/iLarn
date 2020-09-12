#include "palm.h"
#include "iLarnRsc.h"
#include "Globals.h"

#ifdef I_AM_COLOR
static Err beam_myself() SEC_3;
static Err beam_constdata() SEC_3;
// We're beaming databases to the default Applications Launcher
// (not to iLarn) so iLarn needs no code for receiving, only for sending.
void beam_iLarn()
{
  if (0 == beam_myself())
    if (0 == FrmAlert(BeamNextP))
      if (0 == beam_constdata())
	; // success!
}

Err beam_myself_thunk(const void *data, ULong *size, void *userData)
{
  Err err = 0;
  *size = ExgSend((ExgSocketType *)userData, (void *)data, *size, &err);
  return err;
}

static Err beam_myself()
{
  // This is borrowed from example by Aaron Ardiri in palm-dev forum
  UShort        card;
  LocalID       dbID;
  Char          strName[32];
  Char          strDescription[32];
  ExgSocketType socket;
  Err           err, e;

  // let's get the current application details
  SysCurAppDatabase(&card, &dbID);

  // define the strings
  StrCopy(strName,        "iLarn.prc");
  StrCopy(strDescription, "iLarn");

  // configure
  MemSet(&socket, sizeof(ExgSocketType), 0);
  socket.target      = sysFileCLauncher;         // who will receive it
  socket.description = strDescription;           // what to show user :)
  socket.name        = strName;                  // filename for target

  // send it over!
  err = errNone;
  e = ExgPut(&socket);
  err |= e;
  if (err == errNone) {
    e = ExgDBWrite(beam_myself_thunk, &socket, NULL, dbID, card);
    err |= e;
    e = ExgDisconnect(&socket, err);
    err |= e;
  }
  if (err != errNone)
    SndPlaySystemSound(sndError);
  return err;
}

// just like the above, almost.
static Err beam_constdata()
{
  UShort        card = 0;
  LocalID       dbID;
  Char          strName[32];
  Char          strDescription[32];
  ExgSocketType socket;
  Err           err, e;
  dbID = DmFindDatabase(card, iLarnDBName);
  if (!dbID) { // this is impossible, but let's check for missing-db anyway.
    // hm I have no handy little error-popup do I? eh who cares: unreachable!
    SndPlaySystemSound(sndError);
    return exgErrNotFound;
  }
  StrCopy(strName,        "iLarnDB.pdb");
  StrCopy(strDescription, "iLarnDB");
  MemSet(&socket, sizeof(ExgSocketType), 0);
  socket.target      = sysFileCLauncher;
  socket.description = strDescription;
  socket.name        = strName;
  err = errNone;
  e = ExgPut(&socket);
  err |= e;
  if (err == errNone) {
    e = ExgDBWrite(beam_myself_thunk, &socket, NULL, dbID, card);
    err |= e;
    e = ExgDisconnect(&socket, err);
    err |= e;
  }
  if (err != errNone)
    SndPlaySystemSound(sndError);
  return err;
}

#endif
