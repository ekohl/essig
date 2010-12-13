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

.PHONY: all code-samples grammar %.dmo vm install clean

all: code-samples vm

code-samples:
	$(MAKE) $(MAKEOPTS) -C $@

grammar:
	$(MAVEN) -f grammar/pom.xml --quiet

%.dmo: grammar
	$(JAVA) -jar $(ESSIG) grammar/examples/$@

vm: $(EXAMPLE)
	$(MAKE) $(MAKEOPTS) -C $@ cli.so

install: vm
	$(INSTALL) -d $(DESTDIR)
	for file in $(FILES); do \
	  $(INSTALL) $$file $(DESTDIR); \
	done

clean:
	$(GIT) clean -d -X -f
