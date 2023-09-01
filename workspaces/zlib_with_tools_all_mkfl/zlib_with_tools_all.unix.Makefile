

targetName=cpputils_all

mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir		=  $(shell dirname $(mkfile_path))

ifndef qtutilsRepoRoot
        qtutilsRepoRoot	:= $(shell curDir=`pwd` && cd $(mkfile_dir)/../.. && pwd && cd ${curDir})
endif


all:
	$(MAKE) -f $(qtutilsRepoRoot)/prj/tests/qtutils_unit_test_mult/qtutils_unit_test.unix.Makefile			&& \
	$(MAKE) -f $(qtutilsRepoRoot)/prj/tests/any_quick_test_mkfl/any_quick_test.unix.Makefile

.PHONY: clean
clean:
	$(MAKE) -f $(qtutilsRepoRoot)/prj/tests/qtutils_unit_test_mult/qtutils_unit_test.unix.Makefile clean
	$(MAKE) -f $(qtutilsRepoRoot)/prj/tests/any_quick_test_mkfl/any_quick_test.unix.Makefile clean
	
