all: is

OS := $(shell uname)
LBITS := $(shell getconf LONG_BIT)

include toolchain.mk


.PHONY: is
is: toolchain
	@$(MAKE) -f application.is.mk

.PHONY: mp
mp: toolchain
	@$(MAKE) -f application.is.mk mp

.PHONY: clean
clean:
	@$(MAKE) -f application.is.mk clean

debug: toolchain
	@$(MAKE) -f application.is.mk debug

flash: toolchain
	@$(MAKE) -f application.is.mk flash
	
setup:
	@$(MAKE) -f application.is.mk $(MAKECMDGOALS)	

dump:
	@$(MAKE) -f application.is.mk dump

#*****************************************************************************#
#                         MATTER MAKE DEFINES                                 #
#*****************************************************************************#

ENABLE_MATTER = 0

ifeq ($(ENABLE_MATTER), 1)

BASEDIR = $(shell pwd)
MATTER_BUILDDIR = $(shell pwd)/../../../component/common/application/matter/project/amebaz2plus

include $(MATTER_BUILDDIR)/Makefile

.PHONY: is_matter
is_matter: toolchain_matter
	cp $(MATTER_BUILDDIR)/partition_matter.json partition.json
	@$(MAKE) -f application.is.matter.mk

.PHONY: clean_matter
clean_matter:
	@$(MAKE) -f application.is.matter.mk clean

endif
