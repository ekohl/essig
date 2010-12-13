# Grammar config
EXAMPLE=atmel.dmo
ESSIG=grammar/target/essig-0.1-jar-with-dependencies.jar

# Programs
INSTALL=install
DESTDIR=$(CURDIR)/target
MAVEN=mvn
JAVA=java
GIT=git

# Install files
FILES=vm/cli.so vm/simulator.py

# Include VM rules
dir := vm
include $(dir)/Rules.mk

.PHONY: all code-samples grammar %.dmo install clean

all: code-samples vm/cli.so

code-samples:
	$(MAKE) $(MAKEOPTS) -C $@

grammar:
	$(MAVEN) -f grammar/pom.xml --quiet

%.dmo: grammar
	$(JAVA) -jar $(ESSIG) grammar/examples/$@

vm/generated_simulator.h vm/generated_simulator.c: $(EXAMPLE)

install: vm/cli.so
	$(INSTALL) -d $(DESTDIR)
	for file in $(FILES); do \
	  $(INSTALL) $$file $(DESTDIR); \
	done

clean:
	$(GIT) clean -d -X -f
