# Rules for Borland's Win32 toolchain

INIT_DEFAULT_OUTPUT = Makefile.bor
INIT_KEYWORDS       = win32 bcc32

INIT_VAR_CXX        = bcc32
INIT_VAR_CC         = bcc32 -P
INIT_VAR_TLIB       = tlib
INIT_VAR_RM         = rm -f

INIT_FILES          = mklib.c


### Standard Application
begin app
OUTPUT_$(NAME) := $(NAME).exe
OBJECT_EXT     := obj

$(OUTPUT): $(OBJECTS) $(OUTPUT_$(DEPEND)) $(LIBDEPEND)
        $(CXX) $(LDFLAGS) $(LINKFLAGS_$(DEPEND)) -e$(OUTPUT) $(OBJECTS) $(LINK_$(DEPEND)) $(LIBS)

clean:
	$(RM) $(OBJECTS)

distclean:
        $(RM) $(OUTPUT)

ALL_TARGETS += $(OUTPUT)
end


### Static Library
begin lib
OUTPUT_$(NAME) := $(NAME).lib
OBJECT_EXT     := obj
LINK_$(NAME)   := $(OUTPUT)

$(OUTPUT): $(OBJECTS) $(OUTPUT_$(DEPEND)) mklib.exe
        mklib $(TLIB) $(OUTPUT) $(OBJECTS) $(OUTPUT_$(DEPEND))

distclean:
        $(RM) $(OUTPUT)

clean:
	$(RM) $(OBJECTS)

ALL_TARGETS += $(OUTPUT)
end


### Subproject
begin subproject
OBJECT_EXT     := obj
OUTPUT_$(NAME) := $(OBJECTS)
LINK_$(NAME)   := $(OBJECTS) $(LINK_$(DEPEND))

clean:
        $(RM) $(OBJECTS)
end


### Prebuilt library
# Note: the use of "LINKFLAGS" is a special-purpose hack. We should
# have a way to postprocess "LINK" somehow to separate options and
# libraries for compilers that need it.
begin prebuilt-lib
OUTPUT_$(NAME) :=
LINK_$(NAME) := $(NAME).lib
LINKFLAGS_$(NAME) := -L$(LIBDIR)
end


### Basic Rules

# C compiler:
.c.obj:
	$(CC) $(CFLAGS) -o$@ -c $<

.c.s:
	$(CC) $(CFLAGS) -o$@ -S $<

# C++ compiler:
.cc.obj:
	$(CXX) $(CXXFLAGS) -o$@ -c $<

.cpp.obj:
	$(CXX) $(CXXFLAGS) -o$@ -c $<

.cc.s:
	$(CXX) $(CXXFLAGS) -o$@ -S $<

.cpp.s:
	$(CXX) $(CXXFLAGS) -o$@ -S $<

# Administrativa:
distclean: clean

.PHONY: all clean distclean

# Win32 stuff:
mklib.exe: $(PROJ_DIR)/mklib.c
	$(CXX) -e$@ $<
