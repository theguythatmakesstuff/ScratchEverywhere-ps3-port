PLATFORM ?= 3ds

ifeq ($(PLATFORM),3ds)
    include Makefile_3ds
else ifeq ($(PLATFORM),pc)
    include Makefile_pc
else
    $(error Unknown platform: $(PLATFORM))
endif
