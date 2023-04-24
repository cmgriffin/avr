
## These targets don't have files named after them
.PHONY: clean

clean:
	find . -type f \( -name "*.d" -o -name "*.o" -o -name "*.elf" -o -name "*.hex" -o -name "*.lst" -o -name "*.map" \) -delete
