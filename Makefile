ifeq ($(BOLOS_SDK),)
$(error Environment variable BOLOS_SDK is not set)
endif
include $(BOLOS_SDK)/Makefile.defines

APPNAME = "Tezos Baking"

APP_SOURCE_PATH = src

VARIANT_PARAM  = APP
VARIANT_VALUES = tezos_baking

# OPTION

DISABLE_STANDARD_APP_SYNC_RAPDU = 1

ENABLE_BLUETOOTH = 1
ENABLE_NBGL_QRCODE = 1

# APP_LOAD_PARAMS

HAVE_APPLICATION_FLAG_GLOBAL_PIN = 1
HAVE_APPLICATION_FLAG_BOLOS_SETTINGS = 1

CURVE_APP_LOAD_PARAMS = ed25519 secp256k1 secp256r1
PATH_APP_LOAD_PARAMS  = "44'/1729'"

# VERSION

APPVERSION_M=2
APPVERSION_N=4
APPVERSION_P=7
APPVERSION=$(APPVERSION_M).$(APPVERSION_N).$(APPVERSION_P)

DEFINES   += VERSION=\"$(APPVERSION)\"

# COMMIT

GIT_DESCRIBE ?= $(shell git describe --tags --abbrev=8 --always --long --dirty 2>/dev/null)
VERSION_TAG ?= $(shell echo "$(GIT_DESCRIBE)" | cut -f1 -d-)
COMMIT ?= $(shell echo "$(GIT_DESCRIBE)" | awk -F'-g' '{print $2}' | sed 's/-dirty/*/')

DEFINES   += COMMIT=\"$(COMMIT)\"

# Only warn about version tags if specified/inferred
ifeq ($(VERSION_TAG),)
  $(warning VERSION_TAG not checked)
else
  ifneq (v$(APPVERSION), $(VERSION_TAG))
    $(warning Version-Tag Mismatch: v$(APPVERSION) version and $(VERSION_TAG) tag)
  endif
endif

ifeq ($(COMMIT),)
  $(warning COMMIT not specified and could not be determined with git from "$(GIT_DESCRIBE)")
else
  $(info COMMIT=$(COMMIT))
endif

# ICONS

ICON_NANOS  = icons/nano-s-tezos.gif
ICON_NANOX  = icons/nano-x-tezos.gif
ICON_NANOSP = $(ICON_NANOX)
ICON_STAX   = icons/stax_tezos.gif

################
# Default rule #
################
all: show-app default


.PHONY: show-app
show-app:
	@echo ">>>>> Building at commit $(COMMIT)"

CFLAGS   += -Wno-incompatible-pointer-types-discards-qualifiers

include $(BOLOS_SDK)/Makefile.standard_app
