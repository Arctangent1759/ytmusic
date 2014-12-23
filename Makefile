CC := g++
PROTO_COMPILER := protoc
SRCDIR := src
BUILDDIR := build
GENDIR := gen
PROTODIR := proto
TARGET := bin/runner

SRCEXT := cc
PROTOEXT := proto
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
PROTO_FILES := $(shell find $(PROTODIR) -type f -name *.proto)
PROTO_SOURCES := $(patsubst $(PROTODIR)/%,%,$(PROTO_FILES:.proto=.pb))
CFLAGS := -std=c++11 -g # -Wall
LIB := -lre2 -pthread -L lib `pkg-config --cflags --libs python2` -Wl,-rpath=/opt/gstreamer-sdk/lib `pkg-config --cflags --libs gstreamer-0.10` -lprotobuf
INC := -I include -I gen

$(TARGET): $(OBJECTS) | proto
	@echo " Linking..."
	@echo " $(CC) $(BUILDDIR)/*.o -o $(TARGET) $(LIB)"; $(CC) $(BUILDDIR)/*.o -o $(TARGET) $(LIB)

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
	@echo " $(RM) -r $(BUILDDIR) $(GENDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(GENDIR) $(TARGET)

# Tests
tester:
	$(CC) $(CFLAGS) test/tester.cc $(INC) $(LIB) -o bin/tester

# Spikes
ticket:
	$(CC) $(CFLAGS) spikes/ticket.cc $(INC) $(LIB) -o bin/ticket

.PHONY: clean proto
