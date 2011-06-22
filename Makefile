#################################################################
# Main makefile
# Type "make help" for usage
#
# To compile (optimized) and install static and shared library
# with each avaible compilers, type:
# for c in pgi intel gcc 'sun studio12'; do make c $c optimized static shared install; done
#################################################################

# Logging library

# Project options
LIB             := stdcout
BIN              = $(LIB)
SRCDIRS          = src
TESTDIRS         = unit_testing
SRCEXT           = cpp
HEADEXT          = hpp
LANGUAGE         = CPP

# Include the generic rules
include makefiles/Makefile.rules

#################################################################
# Project specific options

LIB_OBJ          = $(OBJ)

# Compression. Uncomment to enable compression. Requires libz
CFLAGS          += -DCOMPRESS_OUTPUT

### Following is needed for compression. Don't touch!
ifneq (,$(findstring -DCOMPRESS_OUTPUT,$(CFLAGS)))
libz_loc = ../libz.git/src
CFLAGS          += -I$(libz_loc)/
LIBZ_OBJ = adler32.o compress.o crc32.o deflate.o gzclose.o gzlib.o gzread.o gzwrite.o infback.o inffast.o inflate.o inftrees.o trees.o uncompr.o zutil.o
LIBZ_OBJ_build = $(addprefix $(build_dir)/libz/, $(notdir $(LIBZ_OBJ)) )

LIB_OBJ += $(LIBZ_OBJ_build)

$(build_dir)/libz :
	mkdir -p $(build_dir)/libz
$(build_dir)/libz/%.o : ../libz.git/src/%.o $(build_dir)/libz
	cp $< $@
endif
### End of compression block

# Project is a library. Include the makefile for build and install.
include makefiles/Makefile.library


