#################################################################################
##
##   GLUE V1.5 Makefile
##
##   !!! Editoi allaolevat variablet !!!
##
##   Jos teet jotain omia moduuleita niin pist‰ ne tuonne SRC_OBJS-listaan.
##   Lis‰ksi lopusta lˆytyy lista mist‰ fileista eri moduulit tehd‰‰n eli
##   make osaa kattoa timestamppeja ja k‰‰nt‰‰ ainoastaan muuttuneet osat.
##   Jos duunaat jotain moduulista toiseen viittauksia niin noita kannattaa
##   p‰ivitell‰ sen mukaan.
##
##   K‰‰nt‰minen: mingw32-make <DEMOZ>.exe, esim:
##   C:\LOL>mingw32-make test3d.exe
##
##   Debuggaus: CFLAGSeihin -g, poista -O2 ja LDFLAGSeista -s pois 
##   sit vaan gdb test3d, run, ja backtrace ku crashaa
##
#################################################################################



# Laita t‰h‰n mik‰ on projektin ja main .c/.exe:n nimi
DEMOZ = test3d


# Laita t‰h‰n polku miss‰ sorsat (eritoten glue_setup.h) lˆytyy
FOLDERZ = .


# Laita t‰h‰n mist‰ GLUE lˆytyy
GLUEDIRZ = ../glue1.5


# Laita t‰h‰n mist‰ gcc.exe lˆytyy
#GCCPATHZ = c:/progra~1/MinGW/bin
GCCPATHZ = c:/MinGW/bin
#GCCPATHZ = c:/mingwstudio/MinGW/bin

# Laita t‰h‰n k‰ytett‰v‰n dialogin nimi (huom. tarvii windres.exe:n ett‰
# rc->res -k‰‰nt‰minen onnistuu)
DIALOGZ = kewlers_lowend



#################################################################################


PROJECT = $(DEMOZ)
#CC = $(GCCPATHZ)/gcc.exe
CC = $(GCCPATHZ)/gcc.exe

WINDRES = $(COMPILER_BIN)windres.exe

OBJ_DIR = Release
OUTPUT_DIR = .
TARGET = $(DEMOZ).exe
#C_INCLUDE_DIRS = -I"$(FOLDERZ)" -I../cg/include -I"$(GLUEDIRZ)/libmpeg2" -I"$(GLUEDIRZ)/libjpeg" -I"$(GLUEDIRZ)/fmod"
#C_INCLUDE_DIRS = -I"$(FOLDERZ)" -I../cg/include -I"$(GLUEDIRZ)/libjpeg" -I"$(GLUEDIRZ)/fmod"
C_INCLUDE_DIRS = -I"$(FOLDERZ)" -I"$(GLUEDIRZ)/libjpeg"
C_PREPROC =
CFLAGS = -pipe -Wall -Wno-unused -Wno-parentheses -O2 -std=c99
RC_INCLUDE_DIRS =
RC_PREPROC =
RCFLAGS =
LIB_DIRS =
#$(GLUEDIRZ)/
LIBS = -lopengl32 -lglu32 -lkernel32 -luser32 -lgdi32 -ljpeg.dll -lwinmm -lbass -lwsock32
#-lmpeg2 -lmpeg2convert  -lhid -lsetupapi -lfmod
LDFLAGS = -pipe -s  -Wl,--subsystem,windows -mwindows -L $(GLUEDIRZ) -L $(GLUEDIRZ)/libjpeg -L .
#-L $(GLUEDIRZ)/libmpeg2 -L $(GLUEDIRZ)/fmod


ifeq ($(OS),Windows_NT)
  NULL =
else
  NULL = nul
endif

SRC_OBJS = \
  $(OBJ_DIR)/$(DEMOZ).o	\
  $(OBJ_DIR)/glue.o	\
  $(OBJ_DIR)/glued.o	\
  $(OBJ_DIR)/glueext.o	\
  $(OBJ_DIR)/gluefont.o	\
  $(OBJ_DIR)/gluegen.o	\
  $(OBJ_DIR)/gluemem.o	\
  $(OBJ_DIR)/glueobj.o	\
  $(OBJ_DIR)/gluepart.o	\
  $(OBJ_DIR)/gluesort.o	\
  $(OBJ_DIR)/gluetinyfont.o	\
  $(OBJ_DIR)/gluetxtr.o	\
  $(OBJ_DIR)/gluewin.o	\
  $(OBJ_DIR)/gluecube.o \
  $(OBJ_DIR)/gluefbo.o \
  $(OBJ_DIR)/gluenoise.o \
  $(OBJ_DIR)/glueovrl.o \
  $(OBJ_DIR)/glueglsl.o \
  $(OBJ_DIR)/gluesect.o \
  $(OBJ_DIR)/device.o \
  $(OBJ_DIR)/track.o \
  $(OBJ_DIR)/data.o \
  $(OBJ_DIR)/lodder.o

#  $(OBJ_DIR)/glue3dx.o \
#  $(OBJ_DIR)/gluempeg.o \
#  $(OBJ_DIR)/gluesmap.o \
#  $(OBJ_DIR)/fmodcrap.o	\


RSRC_OBJS = \
  $(OBJ_DIR)/$(DIALOGZ).res

define build_target
@echo Linking...
@$(CC) -o "$(OUTPUT_DIR)\$(TARGET)" $(SRC_OBJS) $(RSRC_OBJS) $(LIB_DIRS) $(LIBS) $(LDFLAGS)
endef

define compile_resource
@echo Compiling $<
@$(WINDRES) $(RCFLAGS) $(RC_PREPROC) $(RC_INCLUDE_DIRS) -O COFF -i "$<" -o "$@"
endef

define compile_source
@echo Compiling $<
@$(CC) $(CFLAGS) $(C_PREPROC) $(C_INCLUDE_DIRS) -c "$<" -o "$@"
endef

.PHONY: print_header directories

print_header:
	@echo ---------- Now cooking: $(DEMOZ) -----------

directories:
	-@if not exist "$(OUTPUT_DIR)\$(NULL)" mkdir "$(OUTPUT_DIR)"
	-@if not exist "$(OBJ_DIR)\$(NULL)" mkdir "$(OBJ_DIR)"

$(TARGET): print_header directories $(RSRC_OBJS) $(SRC_OBJS)
	$(build_target)

.PHONY: clean cleanall
cleanall:
	@echo Deleting intermediate files for '$(DEMOZ) - $(CFG)'
	-@del $(OBJ_DIR)\*.o
	-@del $(OBJ_DIR)\*.res
	-@del "$(OUTPUT_DIR)\$(TARGET)"
	-@rmdir "$(OUTPUT_DIR)"
clean:
	@echo Deleting intermediate files for '$(DEMOZ) - $(CFG)'
	-@del $(OBJ_DIR)\*.o
	-@del $(OBJ_DIR)\*.res

$(OBJ_DIR)/$(DIALOGZ).res: $(GLUEDIRZ)/dialog/$(DIALOGZ).rc
	$(compile_resource)

$(OBJ_DIR)/$(DEMOZ).o: $(DEMOZ).c	\
glue_setup.h \
shaders.c \
wtf.c \
rocket.c \
sync/base.h \
gluemesh.c \
smap.c
	$(compile_source)


#$(OBJ_DIR)/fmodcrap.o: $(GLUEDIRZ)/fmodcrap.c	\
#$(FOLDERZ)/glue_setup.h \
#$(GLUEDIRZ)/glue.h	\
#$(GLUEDIRZ)/fmodcrap.h	\
#$(GLUEDIRZ)/fmod/fmod.h	\
#$(GLUEDIRZ)/fmod/fmod_errors.h	\
#$(GLUEDIRZ)/gluewin.h
#	$(compile_source)

$(OBJ_DIR)/glue.o: $(GLUEDIRZ)/glue.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/glue.h	\
$(GLUEDIRZ)/gluewin.h	\
$(GLUEDIRZ)/gluetxtr.h	\
$(GLUEDIRZ)/glueext.h	\
$(GLUEDIRZ)/gluegen.h	\
$(GLUEDIRZ)/glueobj.h	\
$(GLUEDIRZ)/glueovrl.h \
$(GLUEDIRZ)/gluefbo.h
	$(compile_source)

$(OBJ_DIR)/gluedensgrid.o: $(GLUEDIRZ)/gluedensgrid.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/gluedensgrid.h	\
$(GLUEDIRZ)/glue.h \
$(GLUEDIRZ)/gluegen.h
	$(compile_source)

$(OBJ_DIR)/glued.o: $(GLUEDIRZ)/glued.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/glued.h
	$(compile_source)

$(OBJ_DIR)/glueext.o: $(GLUEDIRZ)/glueext.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/glueext.h	\
$(GLUEDIRZ)/glue.h	\
$(GLUEDIRZ)/gluewin.h
	$(compile_source)

$(OBJ_DIR)/gluefont.o: $(GLUEDIRZ)/gluefont.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/gluefont.h	\
$(GLUEDIRZ)/glue.h	\
$(GLUEDIRZ)/gluetxtr.h	\
$(GLUEDIRZ)/gluegen.h	\
$(GLUEDIRZ)/glueovrl.h	\
$(GLUEDIRZ)/gluewin.h
	$(compile_source)

$(OBJ_DIR)/glueovrl.o: $(GLUEDIRZ)/glueovrl.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/glueovrl.h	\
$(GLUEDIRZ)/glue.h	\
$(GLUEDIRZ)/glueext.h	\
$(GLUEDIRZ)/gluegen.h	\
$(GLUEDIRZ)/gluetxtr.h	\
$(GLUEDIRZ)/gluemem.h	\
$(GLUEDIRZ)/glueobj.h
	$(compile_source)

$(OBJ_DIR)/gluegen.o: $(GLUEDIRZ)/gluegen.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/gluegen.h	\
$(GLUEDIRZ)/gluewin.h	\
$(GLUEDIRZ)/glue.h
	$(compile_source)

$(OBJ_DIR)/gluemem.o: $(GLUEDIRZ)/gluemem.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/glue.h	\
$(GLUEDIRZ)/gluemem.h	\
$(GLUEDIRZ)/gluewin.h
	$(compile_source)

$(OBJ_DIR)/glueobj.o: $(GLUEDIRZ)/glueobj.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/glueobj.h	\
$(GLUEDIRZ)/glue.h	\
$(GLUEDIRZ)/glueext.h	\
$(GLUEDIRZ)/gluegen.h	\
$(GLUEDIRZ)/gluewin.h
	$(compile_source)

$(OBJ_DIR)/gluepart.o: $(GLUEDIRZ)/gluepart.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/gluepart.h	\
$(GLUEDIRZ)/glue.h	\
$(GLUEDIRZ)/gluemem.h	\
$(GLUEDIRZ)/gluegen.h	\
$(GLUEDIRZ)/glueext.h	\
$(GLUEDIRZ)/glueobj.h	\
$(GLUEDIRZ)/glued.h	\
$(GLUEDIRZ)/gluetxtr.h	\
$(GLUEDIRZ)/gluesort.h
	$(compile_source)

$(OBJ_DIR)/gluesort.o: $(GLUEDIRZ)/gluesort.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/gluesort.h	\
$(GLUEDIRZ)/glue.h	\
$(GLUEDIRZ)/gluewin.h
	$(compile_source)


$(OBJ_DIR)/gluetinyfont.o: $(GLUEDIRZ)/gluetinyfont.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/gluetinyfont.h	\
$(GLUEDIRZ)/gluetxtr.h	\
$(GLUEDIRZ)/glue.h	\
$(GLUEDIRZ)/gluegen.h
	$(compile_source)

$(OBJ_DIR)/gluetxtr.o: $(GLUEDIRZ)/gluetxtr.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/gluetxtr.h	\
$(GLUEDIRZ)/glue.h	\
$(GLUEDIRZ)/glueext.h	\
$(GLUEDIRZ)/gluemem.h	\
$(GLUEDIRZ)/gluewin.h	\
$(GLUEDIRZ)/gluegen.h	\
$(GLUEDIRZ)/libjpeg/jpeglib.h
	$(compile_source)

$(OBJ_DIR)/gluewin.o: $(GLUEDIRZ)/gluewin.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/gluewin.h	\
$(GLUEDIRZ)/glue.h	\
#$(GLUEDIRZ)/fmodcrap.h	\
#$(GLUEDIRZ)/fmod/fmod.h	\
#$(GLUEDIRZ)/fmod/fmod_errors.h	\
$(GLUEDIRZ)/glueext.h	\
$(GLUEDIRZ)/gluegen.h	\
$(GLUEDIRZ)/gluetinyfont.h	\
$(GLUEDIRZ)/gluefbo.h	\
$(GLUEDIRZ)/dialog/resource.h
	$(compile_source)

$(OBJ_DIR)/gluempeg.o: $(GLUEDIRZ)/gluempeg.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/gluempeg.h \
$(GLUEDIRZ)/glue.h	\
$(GLUEDIRZ)/gluewin.h	\
$(GLUEDIRZ)/libmpeg2/mpeg2convert.h	\
$(GLUEDIRZ)/libmpeg2/mpeg2.h
	$(compile_source)

$(OBJ_DIR)/gluesmap.o: $(GLUEDIRZ)/gluesmap.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/gluesmap.h
	$(compile_source)

$(OBJ_DIR)/gluenoise.o: $(GLUEDIRZ)/gluenoise.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/gluenoise.h
	$(compile_source)

$(OBJ_DIR)/gluecube.o: $(GLUEDIRZ)/gluecube.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/gluecube.h \
$(GLUEDIRZ)/glue.h \
$(GLUEDIRZ)/glueext.h
	$(compile_source)

$(OBJ_DIR)/glue3dx.o: $(GLUEDIRZ)/glue3dx.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/glue3dx.h \
$(GLUEDIRZ)/glue.h \
$(GLUEDIRZ)/gluewin.h
	$(compile_source)

$(OBJ_DIR)/gluefbo.o: $(GLUEDIRZ)/gluefbo.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/glueext.h	\
$(GLUEDIRZ)/gluefbo.h \
$(GLUEDIRZ)/gluewin.h \
$(GLUEDIRZ)/gluegen.h \
$(GLUEDIRZ)/gluetxtr.h \
$(GLUEDIRZ)/glueovrl.h
	$(compile_source)

$(OBJ_DIR)/glueglsl.o: $(GLUEDIRZ)/glueglsl.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/glueglsl.h \
$(GLUEDIRZ)/glext.h
	$(compile_source)

$(OBJ_DIR)/gluesect.o: $(GLUEDIRZ)/gluesect.c	\
$(FOLDERZ)/glue_setup.h \
$(GLUEDIRZ)/gluesect.h \
$(GLUEDIRZ)/glue.h
	$(compile_source)


# GNU Rocket

$(OBJ_DIR)/data.o: $(FOLDERZ)/sync/data.c	\
$(FOLDERZ)/sync/data.h \
$(FOLDERZ)/sync/base.h
	$(compile_source)

$(OBJ_DIR)/device.o: $(FOLDERZ)/sync/device.c	\
$(FOLDERZ)/sync/device.h \
$(FOLDERZ)/sync/base.h
	$(compile_source)

$(OBJ_DIR)/track.o: $(FOLDERZ)/sync/track.c	\
$(FOLDERZ)/sync/track.h \
$(FOLDERZ)/sync/base.h
	$(compile_source)

$(OBJ_DIR)/lodder.o: $(FOLDERZ)/lodder.c	\
$(FOLDERZ)/lodder.h \
$(GLUEDIRZ)/glue.h \
$(GLUEDIRZ)/gluewin.h \
$(GLUEDIRZ)/gluegen.h
	$(compile_source)


