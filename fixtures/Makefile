# Self-Documented Makefile
# http://marmelab.com/blog/2016/02/29/auto-documented-makefile.html
.PHONY: help
help: ## Show this help message.
	@grep -E '^[0-9a-zA-Z_\.-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-20s\033[0m %s\n", $$1, $$2}'

CC=/opt/llvm-6.0.0/bin/clang
# CC=/opt/llvm-6.0.0/bin/clang
SYSROOT=-isysroot $(shell xcrun --sdk macosx --show-sdk-path)

MACOS_FRAMEWORKS_DIR=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/Library/Frameworks
# XCTEST_PATH=$(MACOS_FRAMEWORKS_DIR)/XCTest.framework/XCTest
# XCTEST_FLAGS=-framework XCTest

CCFLAGS=$(SYSROOT) -F$(MACOS_FRAMEWORKS_DIR)

OUTPUT_PATH=$(PWD)/bitcode

FIXTURES_IN=001_minimal_test \
            002_calling_class_method \
            003_calling_a_super_method_on_instance \
            004_calling_a_super_method_on_class \
            005_ivars_of_class_and_superclass \
            006_properties_of_class_and_superclass \
            xctest_objc_001_minimal_xctestcase_run

FIXTURES_OUT_BC=$(FIXTURES_IN:%=$(OUTPUT_PATH)/%.bc)
FIXTURES_OUT_LL=$(FIXTURES_IN:%=$(OUTPUT_PATH)/%.ll)
FIXTURES_OUT_O=$(FIXTURES_IN:%=$(OUTPUT_PATH)/%.o)

all: $(FIXTURES_OUT_BC) # $(FIXTURES_OUT_LL) $(FIXTURES_OUT_O) ## Build all fixtures.

$(OUTPUT_PATH)/%.bc: | $(OUTPUT_PATH)
	cd $(OUTPUT_PATH) && \
	  $(CC) $(CCFLAGS) -fobjc-arc -emit-llvm $(PWD)/$*/$*.m -c -o $@

$(OUTPUT_PATH)/%.ll: | $(OUTPUT_PATH)
	cd $(OUTPUT_PATH) && \
	  $(CC) $(CCFLAGS) -fobjc-arc -emit-llvm -S $(PWD)/$*/$*.m -c -o $@

$(OUTPUT_PATH)/%.o: | $(OUTPUT_PATH)
	cd $(OUTPUT_PATH) && \
	  $(CC) $(CCFLAGS) -fobjc-arc $(PWD)/$*/$*.m -c -o $@

$(OUTPUT_PATH):
	mkdir -p $(OUTPUT_PATH)

clean: ## Cleans the build folder.
	rm -rfv $(OUTPUT_PATH)
