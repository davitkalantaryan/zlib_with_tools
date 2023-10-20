

targetName=zlib_with_tools_all

mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir		=  $(shell dirname $(mkfile_path))

ifndef zlibWithToolsRepositoryRoot
        zlibWithToolsRepositoryRoot	:= $(shell curDir=`pwd` && cd $(mkfile_dir)/../.. && pwd && cd ${curDir})
endif


all:
	$(MAKE) -f $(zlibWithToolsRepositoryRoot)/prj/core/zlib_mkfl/zlib.unix.Makefile										&& \
	$(MAKE) -f $(zlibWithToolsRepositoryRoot)/prj/tests/zlibwt_unit_test_mult/zlibwt_unit_test.unix.Makefile			&& \
	$(MAKE) -f $(zlibWithToolsRepositoryRoot)/prj/tests/any_quick_test_mkfl/any_quick_test.unix.Makefile

.PHONY: clean
clean:
	$(MAKE) -f $(zlibWithToolsRepositoryRoot)/prj/core/zlib_mkfl/zlib.unix.Makefile clean
	$(MAKE) -f $(zlibWithToolsRepositoryRoot)/prj/tests/zlibwt_unit_test_mult/zlibwt_unit_test.unix.Makefile clean
	$(MAKE) -f $(zlibWithToolsRepositoryRoot)/prj/tests/any_quick_test_mkfl/any_quick_test.unix.Makefile clean
	
