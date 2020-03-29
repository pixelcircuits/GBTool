# Target Name
TARGET=GBTool

# Directories
BUILDDIR=build
SOURCEDIR=source

# Objects to Build
OBJECTS=$(BUILDDIR)/main.o $(BUILDDIR)/spi.o $(BUILDDIR)/egpio.o $(BUILDDIR)/gbx.o \
	$(BUILDDIR)/gbc.o $(BUILDDIR)/gbc_cart.o $(BUILDDIR)/gbc_rom.o $(BUILDDIR)/gbc_mbc1.o $(BUILDDIR)/gbc_mbc2.o $(BUILDDIR)/gbc_mbc3.o $(BUILDDIR)/gbc_mbc5.o \
	$(BUILDDIR)/gba.o $(BUILDDIR)/gba_cart.o $(BUILDDIR)/gba_rom.o $(BUILDDIR)/gba_save.o $(BUILDDIR)/gba_sram.o $(BUILDDIR)/gba_flash.o $(BUILDDIR)/gba_eeprom.o 

# Libraries to Include
LIBRARIES=-lcrypto

# Compiler
CC=gcc
CXX=g++

# Flags
CFLAGS=
CXXFLAGS=$(CFLAGS)

#===============================================================================

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBRARIES)

$(BUILDDIR)/%.o : $(SOURCEDIR)/gba/%.c
	$(CC) -I$(SOURCEDIR) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/%.o : $(SOURCEDIR)/gbc/%.c
	$(CC) -I$(SOURCEDIR) $(CFLAGS) -c -o $@ $<
	
$(BUILDDIR)/%.o : $(SOURCEDIR)/%.c
	$(CC) -I$(SOURCEDIR) $(CFLAGS) -c -o $@ $<

$(shell mkdir -p $(BUILDDIR))

clean:
	rm -f $(TARGET)
	rm -f $(BUILDDIR)/*.o

.PHONY: FORCE