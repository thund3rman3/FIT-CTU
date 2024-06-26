LD=g++
CXX=g++
CXXFLAGS=-std=c++14 -Wall -pedantic -Wextra -g

TARGETS=src/main.o src/notes/CNote.o src/notes/CTextNote.o src/notes/CShopList.o src/notes/CToDoList.o src/notes/CFolder.o \
	src/notes/CFileManager.o src/additional_for_notes/CTag.o src/menus/CLeftFileMenu.o src/menus/CLeftFolderMenu.o \
	src/menus/CMenu.o src/menus/CLeftMenuBar.o src/CApplication.o src/menus/CLeftEditMenu.o\
	src/additional_for_notes/CDate.o

LIBS=-lncurses
OUT=jechjose

run: compile
	./$(OUT)

all: deps compile doc

compile: $(OUT)

$(OUT): $(TARGETS)
	$(LD) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf *.o src/*.o src/*/*.o $(OUT) doc/ Makefile.d

deps:
	$(CXX) -MM src/*.cpp > Makefile.d
	$(CXX) -MM src/*/*.cpp >> Makefile.d

doc:
	doxygen Doxyfile

-include Makefile.d