CC := g++
PROTO_COMPILER := protoc
SRCDIR := src
BUILDDIR := build
GENDIR := gen
PROTODIR := proto
BINDIR := bin

SRCEXT := cc
PROTOEXT := proto
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
PROTO_FILES := $(shell find $(PROTODIR) -type f -name *.proto)
PROTO_SOURCES := $(patsubst $(PROTODIR)/%,%,$(PROTO_FILES:.proto=.pb))
PROTO_OBJECTS := $(patsubst $(PROTODIR)/%,$(BUILDDIR)/%,$(PROTO_FILES:.proto=.pb.o))
CFLAGS := -std=c++11 -g # -Wall
LIB := -lre2 -pthread -L lib `pkg-config --cflags --libs python2` -Wl,-rpath=/opt/gstreamer-sdk/lib `pkg-config --cflags --libs gstreamer-0.10` -lprotobuf
INC := -I include -I gen

RUNNERS := $(shell ls $(SRCDIR)/*-runner.$(SRCEXT) | sed s/\.$(SRCEXT)$$/.o/g | sed s/^$(SRCDIR)/$(BUILDDIR)/g)

OBJECTS_NO_RUNNERS := $(shell echo $(OBJECTS) $(PROTO_OBJECTS) | sed s/\ /\\n/g | grep -ve '-runner\.o$$')

all: server client

client: $(OBJECTS) | proto
	@echo " Linking..."
	@mkdir -p $(BINDIR)
	@echo " $(CC) $(OBJECTS_NO_RUNNERS) build/client-runner.o -o $(BINDIR)/client $(LIB)"; $(CC) $(OBJECTS_NO_RUNNERS) build/client-runner.o -o $(BINDIR)/client $(LIB)

server: $(OBJECTS) | proto
	@echo " Linking..."
	@mkdir -p $(BINDIR)
	@echo " $(CC) $(OBJECTS_NO_RUNNERS) build/server-runner.o -o $(BINDIR)/server $(LIB)"; $(CC) $(OBJECTS_NO_RUNNERS) build/server-runner.o -o $(BINDIR)/server $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT) | proto
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) $(LIB) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) $(LIB) -c -o $@ $<

proto:
	@mkdir -p $(GENDIR)
	@mkdir -p $(BUILDDIR)
	@echo " $(PROTO_COMPILER) --cpp_out=$(GENDIR) $(PROTODIR)/*"
	@$(PROTO_COMPILER) --cpp_out=$(GENDIR) $(PROTODIR)/*
	@for i in $(PROTO_SOURCES); do echo " $(CC) $(CFLAGS) $(INC) $(LIB) -c -o $(BUILDDIR)/$$i.o $(GENDIR)/proto/$$i.cc"; $(CC) $(CFLAGS) $(INC) $(LIB) -c -o $(BUILDDIR)/$$i.o $(GENDIR)/proto/$$i.cc; done;

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(GENDIR) $(BINDIR)"; $(RM) -r $(BUILDDIR) $(GENDIR) $(BINDIR)

# Tests
tester:
	$(CC) $(CFLAGS) test/tester.cc $(INC) $(LIB) -o bin/tester

# Spikes
ticket:
	$(CC) $(CFLAGS) spikes/ticket.cc $(INC) $(LIB) -o bin/ticket

.PHONY: clean proto
