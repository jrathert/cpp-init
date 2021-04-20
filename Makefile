#
# cpp-init - a small tool to initialize C++ projects
#
# Makefile
#
# (c) 2021 Jonas Rathert
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy of this 
# software and associated documentation files (the "Software"), to deal in the Software
# without restriction, including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
# to whom the Software is furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all copies or 
# substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
# BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

BINDIR     := bin
OBJDIR     := obj
SRCDIR     := src
DEPDIR     := .dep
INCLUDEDIR := include

STD_VERSION := c++20

DEBUG_FLAGS := -g3 -ggdb3 -DDEBUG=1
OPTIM_FLAGS := -O3
WARN_FLAGS  := -W -Wall

CXX         := /usr/bin/g++-10
CPPFLAGS    := -I$(INCLUDEDIR)
CXXFLAGS    := -std=$(STD_VERSION) $(WARN_FLAGS) 
CXXDEFS	    :=
LDFLAGS     :=    
LDLIBS      := 

TARGET      := cpp-init

#
# Usually, no changes needed below this line
#

SRCFILES := $(notdir $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/**/*.cpp))
DEPFILES := $(SRCFILES:%.cpp=$(DEPDIR)/%.d)
OBJFILES := $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(SRCFILES)))

#
# set up dependency generation to speed up compilation after changes
#   - define flags as well as change compilation command
#
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

ifdef optimize
	CXXFLAGS += $(OPTIM_FLAGS)
else
	CXXFLAGS += $(DEBUG_FLAGS)
endif

COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

### MAIN TARGET ###

.PHONY : all clean

all: $(BINDIR)/$(TARGET)

clean:
	-rm -rf $(BINDIR) $(OBJDIR) $(DEPDIR)  

### OTHER TARGETS ###

$(BINDIR)/$(TARGET): $(OBJFILES) | $(BINDIR)
	$(LINK.cc) -o $@ $^ $(LDLIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(DEPDIR)/%.d | $(OBJDIR) $(DEPDIR)
	$(COMPILE.cc) $< $(OUTPUT_OPTION)

$(OBJDIR): ; @mkdir -p $(OBJDIR)

$(BINDIR): ; @mkdir -p $(BINDIR)

$(DEPDIR): ; @mkdir -p $(DEPDIR)

$(DEPFILES):

-include $(wildcard $(DEPFILES))
