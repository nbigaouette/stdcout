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

# ### Following is needed for compression. Don't touch!
# ifneq (,$(findstring -DCOMPRESS_OUTPUT,$(CFLAGS)))
# libz_OBJ = $(addprefix $(build_dir)/, adler32.o compress.o crc32.o deflate.o gzclose.o gzlib.o gzread.o gzwrite.o infback.o inffast.o inflate.o inftrees.o trees.o uncompr.o zutil.o)
# LIB_OBJ := $(libz_OBJ) $(LIB_OBJ)
# # Extract all objects files from static library in build
# $(libz_OBJ): /usr/lib/libz.a
# 	cd $(build_dir) && ar x /usr/lib/libz.a && cd ..
# endif

# Project is a library. Include the makefile for build and install.
include makefiles/Makefile.library


