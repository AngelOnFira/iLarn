TARGET = iLarn
APPNAME = "iLarn"
APPID = "Larn"
# NOTE: I have registered this app id.

# how to split?  hm
FORMO = form_about.o form_cast.o form_evader.o form_hwbuttons.o \
  form_invselect.o form_lrs.o form_map.o form_messagelog.o form_pad.o \
  form_pickch.o form_prefs.o form_question.o form_score.o form_store.o \
  form_trans.o
OBJS1 = alloc_env.o cast.o create.o dead.o display.o global.o inv.o main.o \
        md_malloc.o monster.o movem.o moveplayer.o obj.o object.o \
        object_actions.o object_actions_cont.o pickchar.o random.o regen.o \
        savelev.o sphere.c stats.o store.o sum.o util.o \
	color.o win.o beam.o
OBJS = $(FORMO) $(OBJS1) $(TARGET)-sections.o

INCS = iLarnRsc.h Globals.h larn_prototypes.h types.h

CC = m68k-palmos-gcc
#CFLAGS = -Wall -g -O2 -I. -palmos2 -DI_AM_OS_2
##CFLAGS = -Wall -g -O2 -I. -palmos3.1 -DNON_INTERNATIONAL
CFLAGS = -Wall -g -O2 -palmos3.5 -DNON_INTERNATIONAL -DI_AM_COLOR

PILRC = pilrc
#OBJRES = m68k-palmos-obj-res
BUILDPRC = build-prc

#SHRINC = /usr/local/pilot/sdk/sdk/include/Core/

all: iLarn.prc

.S.o:
	$(CC) $(TARGETFLAGS) -c $<

.c.s:
	$(CC) $(CSFLAGS) $<

$(TARGET).prc: $(TARGET) bin.stamp
	$(BUILDPRC) $(TARGET).def $(TARGET) *.bin

$(TARGET): $(OBJS) $(TARGET)-sections.ld
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(TARGET)-sections.ld

bin.stamp: $(TARGET).rcp $(TARGET).pbm Font56.txt
	$(PILRC) $(TARGET).rcp .
	touch bin.stamp

$(TARGET)-sections.o: $(TARGET)-sections.s
	$(CC) -c $(TARGET)-sections.s

$(TARGET)-sections.s $(TARGET)-sections.ld: $(TARGET).def
	multigen $(TARGET).def


send: $(TARGET).prc
	$(PILOTXFER) -i $(TARGET).prc

depend:
	makedepend -Y  *.c
#	makedepend -Y -I/usr/local/pilot/m68k-palmos/include/ -I$(SHRINC) -I$(SHRINC)/UI -I$(SHRINC)/System  -I$(SHRINC)/Hardware *.c

dingy:
	-rm -f *.[oa] $(TARGET)

clean:
	-rm -f *.[oa] $(TARGET) *.bin bin.stamp *.grc Makefile.bak

veryclean: clean
	-rm -f $(TARGET).prc pilot.ram pilot.scratch *.ld *.s

# DO NOT DELETE

alloc_env.o: palm.h Globals.h header.h defines.h types.h larn_prototypes.h
alloc_env.o: sections.h
cast.o: palm.h Globals.h header.h defines.h types.h larn_prototypes.h
cast.o: sections.h iLarnRsc.h
color.o: palm.h sections.h defines.h types.h color.h
create.o: palm.h Globals.h header.h defines.h types.h larn_prototypes.h
create.o: sections.h
dead.o: palm.h Globals.h header.h defines.h types.h larn_prototypes.h
dead.o: sections.h iLarnRsc.h
display.o: palm.h Globals.h header.h defines.h types.h larn_prototypes.h
display.o: sections.h iLarnRsc.h win.h
form_about.o: palm.h iLarnRsc.h Globals.h header.h defines.h types.h
form_about.o: larn_prototypes.h sections.h
form_cast.o: palm.h iLarnRsc.h Globals.h header.h defines.h types.h
form_cast.o: larn_prototypes.h sections.h
form_evader.o: palm.h iLarnRsc.h Globals.h header.h defines.h types.h
form_evader.o: larn_prototypes.h sections.h
form_hwbuttons.o: palm.h iLarnRsc.h Globals.h header.h defines.h types.h
form_hwbuttons.o: larn_prototypes.h sections.h
form_invselect.o: palm.h iLarnRsc.h Globals.h header.h defines.h types.h
form_invselect.o: larn_prototypes.h sections.h
form_lrs.o: palm.h iLarnRsc.h Globals.h header.h defines.h types.h
form_lrs.o: larn_prototypes.h sections.h
form_map.o: palm.h iLarnRsc.h Globals.h header.h defines.h types.h
form_map.o: larn_prototypes.h sections.h
form_messagelog.o: palm.h iLarnRsc.h Globals.h header.h defines.h types.h
form_messagelog.o: larn_prototypes.h sections.h
form_pad.o: palm.h iLarnRsc.h Globals.h header.h defines.h types.h
form_pad.o: larn_prototypes.h sections.h
form_pickch.o: palm.h iLarnRsc.h Globals.h header.h defines.h types.h
form_pickch.o: larn_prototypes.h sections.h
form_prefs.o: palm.h iLarnRsc.h Globals.h header.h defines.h types.h
form_prefs.o: larn_prototypes.h sections.h color.h
form_question.o: palm.h iLarnRsc.h Globals.h header.h defines.h types.h
form_question.o: larn_prototypes.h sections.h
form_score.o: palm.h iLarnRsc.h Globals.h header.h defines.h types.h
form_score.o: larn_prototypes.h sections.h
form_store.o: palm.h iLarnRsc.h Globals.h header.h defines.h types.h
form_store.o: larn_prototypes.h sections.h
form_trans.o: palm.h iLarnRsc.h Globals.h header.h defines.h types.h
form_trans.o: larn_prototypes.h sections.h
global.o: palm.h Globals.h header.h defines.h types.h larn_prototypes.h
global.o: sections.h
inv.o: palm.h Globals.h header.h defines.h types.h larn_prototypes.h
inv.o: sections.h iLarnRsc.h
main.o: palm.h callback.h iLarnRsc.h Globals.h header.h defines.h types.h
main.o: larn_prototypes.h sections.h
md_malloc.o: palm.h
monster.o: palm.h Globals.h header.h defines.h types.h larn_prototypes.h
monster.o: sections.h iLarnRsc.h
movem.o: palm.h Globals.h header.h defines.h types.h larn_prototypes.h
movem.o: sections.h
moveplayer.o: palm.h Globals.h header.h defines.h types.h larn_prototypes.h
moveplayer.o: sections.h
obj.o: palm.h Globals.h header.h defines.h types.h larn_prototypes.h
obj.o: sections.h
object.o: palm.h Globals.h header.h defines.h types.h larn_prototypes.h
object.o: sections.h iLarnRsc.h
object_actions.o: palm.h Globals.h header.h defines.h types.h
object_actions.o: larn_prototypes.h sections.h iLarnRsc.h
object_actions_cont.o: palm.h Globals.h header.h defines.h types.h
object_actions_cont.o: larn_prototypes.h sections.h
pickchar.o: palm.h Globals.h header.h defines.h types.h larn_prototypes.h
pickchar.o: sections.h
random.o: palm.h
regen.o: palm.h Globals.h header.h defines.h types.h larn_prototypes.h
regen.o: sections.h
savelev.o: palm.h Globals.h header.h defines.h types.h larn_prototypes.h
savelev.o: sections.h bit.h
sphere.o: palm.h Globals.h header.h defines.h types.h larn_prototypes.h
sphere.o: sections.h
stats.o: palm.h Globals.h header.h defines.h types.h larn_prototypes.h
stats.o: sections.h win.h
store.o: palm.h Globals.h header.h defines.h types.h larn_prototypes.h
store.o: sections.h iLarnRsc.h
sum.o: palm.h
util.o: palm.h defines.h types.h
win.o: palm.h sections.h defines.h types.h win.h
