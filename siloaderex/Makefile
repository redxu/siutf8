##############################################################################
##
##  Makefile for Detours Test Programs.
##
##  Microsoft Research Detours Package, Version 3.0.
##
##  Copyright (c) Microsoft Corporation.  All rights reserved.
##

!include ..\common.mak

##############################################################################

all: dirs   \
    $(BIND)\loaderex.exe \
!IF $(DETOURS_SOURCE_BROWSING)==1
    $(OBJD)\loaderex.bsc
!ENDIF

clean:
    -del *~ 2>nul
    -del $(BIND)\loaderex.* 2>nul
    -rmdir /q /s $(OBJD) 2>nul

realclean: clean
    -rmdir /q /s $(OBJDS) 2>nul

##############################################################################

dirs:
    if not exist $(BIND) mkdir $(BIND)
    if not exist $(OBJD) mkdir $(OBJD)

$(OBJD)\loaderex.obj : loaderex.cpp

$(BIND)\loaderex.exe : $(OBJD)\loaderex.obj $(DEPS)
    cl $(CFLAGS) /Fe$@ /Fd$(BIND)\loaderex.pdb $(OBJD)\loaderex.obj \
        /link $(LINKFLAGS) $(LIBS) /subsystem:windows /entry:mainCRTStartup /incremental:no

$(OBJD)\loaderex.bsc : $(OBJD)\loaderex.obj
    bscmake /v /n /o $@ $(OBJD)\loaderex.sbr

##############################################################################


################################################################# End of File.
