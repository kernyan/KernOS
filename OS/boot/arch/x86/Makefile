all: boot.ao

boot.ao: boot.S
	@echo "as --32 -o $@"; as --32 -o $@ $< # $@ refers to target, $< refers to all pre-reqs
