DIR = Release

RES = $(DIR)\bfg.res
BIN = $(DIR)\bfg.exe

OBJS = \
	$(DIR)\bfg.obj \
	$(DIR)\config.obj \
	$(DIR)\font.obj \
	$(DIR)\pch.obj \
	$(DIR)\preview.obj \
	$(DIR)\saveopt.obj \
	$(DIR)\utils.obj

DEFS = /DNDEBUG
LIBS = user32.lib gdi32.lib comdlg32.lib msimg32.lib zlib\zlib.lib

CC = @cl /nologo /c /MP
RC = @rc /nologo
LD = @link /NOLOGO
MD = @mkdir
RM = @del /q

CFLAGS = /W4 /O2 /GS-
RCFLAGS =
LDFLAGS = /OPT:REF,ICF

all: $(DIR) $(BIN)

$(DIR):
	$(MD) $(DIR)

.cpp{$(DIR)}.obj::
	$(CC) $(CFLAGS) /MD /EHsc $(DEFS) /Fo$(DIR)\ $<

.rc{$(DIR)}.res:
	$(RC) $(RCFLAGS) /fo$@ $<

$(BIN): $(OBJS) $(RES)
	$(LD) $(LDFLAGS) /OUT:$@ $(OBJS) $(RES) $(LIBS)

clean:
	$(RM) $(OBJS)
	$(RM) $(RES)
	$(RM) $(BIN)
