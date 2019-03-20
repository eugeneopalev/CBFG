OUT_DIR = Release

RES = $(OUT_DIR)\bfg.res
BIN = $(OUT_DIR)\bfg.exe

DEFS = /DNDEBUG
LIBS = user32.lib gdi32.lib comdlg32.lib opengl32.lib

CC = @cl /nologo /c
RC = @rc /nologo
LD = @link /NOLOGO
MD = @mkdir
RM = @del /q

CFLAGS = /MP /W4 /O2 /GS- /EHsc /MD /Fo$(OUT_DIR)\ $(DEFS)
RCFLAGS =
LDFLAGS = /OPT:REF,ICF

OBJS = \
	$(OUT_DIR)\about.obj \
	$(OUT_DIR)\bfg.obj \
	$(OUT_DIR)\config.obj \
	$(OUT_DIR)\filedlg.obj \
	$(OUT_DIR)\font.obj \
	$(OUT_DIR)\pch.obj \
	$(OUT_DIR)\preview.obj \
	$(OUT_DIR)\saveopt.obj \
	$(OUT_DIR)\utils.obj

all: $(OUT_DIR) $(BIN)

$(OUT_DIR):
	$(MD) $(OUT_DIR)

.cpp{$(OUT_DIR)}.obj::
	$(CC) $(CFLAGS) $<

.rc{$(OUT_DIR)}.res:
	$(RC) $(RCFLAGS) /fo$@ $<

$(BIN): $(OBJS) $(RES)
	$(LD) $(LDFLAGS) /OUT:$@ $(OBJS) $(RES) $(LIBS)

clean:
	$(RM) $(OBJS)
	$(RM) $(RES)
	$(RM) $(BIN)
