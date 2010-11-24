essig:
	mvn --quiet

test:
	mvn test

examples: essig execute

execute:
	java -jar target/essig-0.1-jar-with-dependencies.jar -dot examples/atmel.dmo

run: examples
	make run -C ../vm

clean:
	git clean -d -X -f
