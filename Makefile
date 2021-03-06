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
# Seems to cause a lot of overhead in saving the screen log (std_cout)
#CFLAGS          += -DCOMPRESS_OUTPUT

### Following is needed for compression. Don't touch!
ifneq (,$(findstring -DCOMPRESS_OUTPUT,$(CFLAGS)))
LDFLAGS += -lz
endif
### End of compression block

# Project is a library. Include the makefile for build and install.
include makefiles/Makefile.library


