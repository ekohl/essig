ULIMIT=ulimit -v $$((1024 * 2048))
MAVEN=$(ULIMIT); mvn
JAVA=$(ULIMIT); java
EXAMPLE=atmel.dmo
ESSIG=target/essig-0.1-jar-with-dependencies.jar

.PHONY: essig test
essig:
	$(MAVEN) --quiet

test:
	$(MAVEN) -Dmaven.test.skip=false test

# Compile and execute the example
example: essig execute

# Execute example
execute: $(EXAMPLE)

# Example files
%.dmo:
	$(JAVA) -jar $(ESSIG) examples/$@

# Run the example
run: example
	$(MAKE) run -C ../vm

clean:
	git clean -d -X -f
