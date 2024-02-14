.PHONY: deps build check clean detect query run test user

BASEDIR=$(PWD)/scripts

deps:
	$(BASEDIR)/deps

build:
	$(BASEDIR)/build

check:
	$(BASEDIR)/check

clean:
	$(BASEDIR)/clean

detect:
	$(BASEDIR)/detect $(@) $(host) $(container)

query:
	$(BASEDIR)/query $(@)

run:
	$(BASEDIR)/run $(container)

user:
	$(BASEDIR)/user

container:
	$(BASEDIR)/container
