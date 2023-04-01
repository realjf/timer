
BIN=bin/ctimer

PLAT ?= LINUX
TYPE ?= DEBUG
TEST ?= OFF
CLEAN ?= OFF


ROOT_PATH=$(shell pwd)
DEPS_BUILD_DIR=build_deps
DEPS_CMAKE=cmake/deps/
DEPS=$(foreach dir,$(DEPS_CMAKE),$(wildcard $(dir)*/*.txt))
DepNameTmp=

buildDeps = \
	printf "%-8s %s %s %s\n" "[BUILD]" $2 "[TO]" $1; \
	rm -rf $1; \
	mkdir -p $1; \
	if [ $(PLAT) = "WINDOWS" ]; then \
		cd $1 && cmake $2 -G "Visual Studio 16 2019" -A x64 -D BUILD_OUTPUT_PATH=$1; \
	elif [ $(PLAT) = "LINUX" ]; then \
		cd $1 && cmake $2 -G "Unix Makefiles" -D CMAKE_C_COMPILER=gcc-11 -D CMAKE_CXX_COMPILER=g++-11 -D BUILD_OUTPUT_PATH=$1; \
	elif [ $(PLAT) = "MACOS" ]; then \
		cd $1 && cmake $2 -G "Unix Makefiles" -D BUILD_OUTPUT_PATH=$1; \
	fi; \
	cd $(ROOT_PATH)/$1 && cmake --build . --config Release; \
	printf "change directory to %s\n" $(ROOT_PATH); \
	cd $(ROOT_PATH);


pp = \
	printf "%-8s %s\n" "[CMAKE]" $1;




deps:
	@printf "total dependencies: %s\n" $(words $(DEPS))
	@printf "rootpath: %s\n"$(ROOT_PATH)
	@$(foreach dir,$(DEPS),$(call pp,$(dir)))
	@$(foreach dir,$(DEPS),$(call buildDeps,$(DEPS_BUILD_DIR)/$(basename $(notdir $(patsubst %/,%,$(dir $(dir))).txt)),../../$(dir $(dir))))
	@printf "builded dependencies: %s\n" $(words $(DEPS))



rm_submod:
# 清空模块目录
	@-git submodule deinit -f ${MOD_NAME}
# 删除.gitmodules中的模块信息
	@-git rm -r --cached ${MOD_NAME}
# 清理.gitmodules!!!
# 删除目录
	@-sed -i "\#${MOD_NAME}#,+5d" .gitmodules
	@-rm -rf .git/modules/${MOD_NAME}
	@-rm -rf ${MOD_NAME}



M ?= "update"

push:
	@git add -A && git commit -m $(M) && git push origin master

pull:
	@git pull
	@git submodule update --init --checkout
	@git submodule sync

pull_mods:
# @cd deps/SDLMIXER/external && chmod +x ./download.sh && ./download.sh
	@cd deps/SDLMIXER && git submodule update --init --checkout && git submodule sync
#@git submodule foreach 'git pull '
#@-git submodule update --remote


BUILD_DIR=build
CMAKE_DIR=../

build:
	@rm -rf ${BUILD_DIR}
	@mkdir -p ${BUILD_DIR}
ifeq ($(PLAT),WINDOWS)
	@cd ${BUILD_DIR} && cmake ${CMAKE_DIR} -G "Visual Studio 16 2019" -A x64 -DTEST_APP=${TEST}
endif
ifeq ($(PLAT),LINUX)
	@cd ${BUILD_DIR} && cmake ${CMAKE_DIR} -G "Unix Makefiles" -D CMAKE_C_COMPILER=gcc-11 -D CMAKE_CXX_COMPILER=g++-11  -DTEST_APP=${TEST}
endif
ifeq ($(PLAT),MACOS)
	@cd ${BUILD_DIR} && cmake ${CMAKE_DIR} -G "Unix Makefiles"  -DTEST_APP=${TEST}
endif

ifeq (${TYPE},DEBUG)
	@cd ${BUILD_DIR} && cmake --build . --config Debug
endif
ifeq (${TYPE},RELEASE)
	@cd ${BUILD_DIR} && cmake --build . --config Release
endif
ifeq (${TYPE},RELDEBINFO)
	@cd ${BUILD_DIR} && cmake --build . --config ReleaseDebInfo
endif



BUILD_TEST_DIR=build_test
TEST_CMAKE_DIR=../tests


test:
	@rm -rf ${BUILD_TEST_DIR}
	@mkdir -p ${BUILD_TEST_DIR}
ifeq ($(PLAT),WINDOWS)
	@cd ${BUILD_TEST_DIR} && cmake ${TEST_CMAKE_DIR} -G "Visual Studio 16 2019" -A x64 -DTEST_APP=${TEST}
endif
ifeq ($(PLAT),LINUX)
	@cd ${BUILD_TEST_DIR} && cmake ${TEST_CMAKE_DIR} -G "Unix Makefiles" -D CMAKE_C_COMPILER=gcc-11 -D CMAKE_CXX_COMPILER=g++-11  -DTEST_APP=${TEST}
endif
ifeq ($(PLAT),MACOS)
	@cd ${BUILD_TEST_DIR} && cmake ${TEST_CMAKE_DIR} -G "Unix Makefiles"  -DTEST_APP=${TEST}
endif

ifeq (${TYPE},DEBUG)
	@cd ${BUILD_TEST_DIR} && cmake --build . --config Debug
endif
ifeq (${TYPE},RELEASE)
	@cd ${BUILD_TEST_DIR} && cmake --build . --config Release
endif
ifeq (${TYPE},RELDEBINFO)
	@cd ${BUILD_TEST_DIR} && cmake --build . --config ReleaseDebInfo
endif



run:
	@./${BIN}

run_test:
	@chmod +x ./bin_test/ctimer_test
	@./bin_test/ctimer_test


.PHONY: rm_submod run build pull test run push deps pull_mods
