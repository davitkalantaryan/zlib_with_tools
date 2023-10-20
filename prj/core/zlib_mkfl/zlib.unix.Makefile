
targetName=libzlib.so

mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir		=  $(shell dirname $(mkfile_path))

firstTarget: all

include $(mkfile_dir)/../../common/common_mkfl/flagsandsys_common_private.unix.Makefile

COMMON_FLAGS += -fPIC
COMMON_FLAGS += -include"zlib_with_tools/zlib_first_include.h"
COMMON_FLAGS += -Wimplicit-fallthrough=0

SOURCES += $(zlibWithToolsRepositoryRoot)/contrib/zlib/adler32.c	
SOURCES += $(zlibWithToolsRepositoryRoot)/contrib/zlib/compress.c
SOURCES += $(zlibWithToolsRepositoryRoot)/contrib/zlib/crc32.c	
SOURCES += $(zlibWithToolsRepositoryRoot)/contrib/zlib/deflate.c	
SOURCES += $(zlibWithToolsRepositoryRoot)/contrib/zlib/gzclose.c	
SOURCES += $(zlibWithToolsRepositoryRoot)/contrib/zlib/gzlib.c	
SOURCES += $(zlibWithToolsRepositoryRoot)/contrib/zlib/gzread.c	
SOURCES += $(zlibWithToolsRepositoryRoot)/contrib/zlib/gzwrite.c	
SOURCES += $(zlibWithToolsRepositoryRoot)/contrib/zlib/infback.c	
SOURCES += $(zlibWithToolsRepositoryRoot)/contrib/zlib/inffast.c	
SOURCES += $(zlibWithToolsRepositoryRoot)/contrib/zlib/inflate.c	
SOURCES += $(zlibWithToolsRepositoryRoot)/contrib/zlib/inftrees.c
SOURCES += $(zlibWithToolsRepositoryRoot)/contrib/zlib/trees.c	
SOURCES += $(zlibWithToolsRepositoryRoot)/contrib/zlib/uncompr.c	
SOURCES += $(zlibWithToolsRepositoryRoot)/contrib/zlib/zutil.c			



all: $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/lib/$(targetName)

$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/lib/$(targetName): \
                $(SOURCES:%=$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)/%.o)	   
	@mkdir -p $(@D)
	@$(LINK) $^ -shared -fPIC $(LIBS) $(LFLAGS) -o $@

.PHONY: clean
clean:
	@rm -rf $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)
	@rm -f  $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/test/$(targetName)
	@echo "  " cleaning of $(targetName) complete !!!
