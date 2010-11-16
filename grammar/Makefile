essig:
	mvn --quiet

test:
	mvn test

examples: essig
	java -jar target/essig-0.1-jar-with-dependencies.jar examples/atmel.dmo

clean:
	git clean -d -X -f
