
targetName=zlibwt_unit_test

mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir		=  $(shell dirname $(mkfile_path))


firstTarget: all

include $(mkfile_dir)/../../common/common_mkfl/flagsandsys_common_private.unix.Makefile
include $(systemRepositoryRoot)/prj/common/common_mkfl/flagsandsys_common_private.unix.Makefile

UNIT_TEST_SRCS_DIR=$(zlibWithToolsRepositoryRoot)/src/tests/unit_test
CORE_SRCS_DIR=$(zlibWithToolsRepositoryRoot)/src/core
DIR_ITER_CORE_SRCS_DIR=$(directoryIteratorRepoRoot)/src/core

UNIT_TEST_SRCS	= $(shell find $(UNIT_TEST_SRCS_DIR) -name "*.cpp")
CORE_SRCS_CPP	= $(shell find $(CORE_SRCS_DIR) -name "*.cpp")
CORE_SRCS_C	= $(shell find $(CORE_SRCS_DIR) -name "*.c")
DIR_ITER_CORE_SRCS	= $(shell find $(DIR_ITER_CORE_SRCS_DIR) -name "*.c")

EXTRA_SOURCSES += $(cinternalRepoRoot)/src/core/cinternal_core_unit_test_checks.c
EXTRA_SOURCSES += $(cinternalRepoRoot)/src/core/cinternal_core_unit_test_main.c
EXTRA_SOURCSES += $(cinternalRepoRoot)/src/core/cinternal_core_logger.c

COMMON_FLAGS += -DCINTERNAL_UNIT_TEST_USE_GTEST_LIKE_MACROSES
# c=+ 11 is needed for google test
CPPFLAGS += -std=c++11

LIBS += -pthread
LIBS += -ldl
LIBS += -lzlib

all: $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/test/$(targetName)

$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/test/$(targetName): \
                        $(EXTRA_SOURCSES:%=$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)/%.o)	\
			$(UNIT_TEST_SRCS:%=$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)/%.o)				\
			$(CORE_SRCS_CPP:%=$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)/%.o)				\
			$(CORE_SRCS_C:%=$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)/%.o)					\
			$(DIR_ITER_CORE_SRCS:%=$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)/%.o)
	@mkdir -p $(@D)
	@$(LINK) $^ $(LIBS) $(LFLAGS) -o $@

.PHONY: clean
clean:
	@rm -rf $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)
	@rm -f  $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/test/$(targetName)
	@echo "  " cleaning of $(targetName) complete !!!
