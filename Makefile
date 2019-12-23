### Testing out CSP client code 
### Use CSP library to play with CRC
### Use KISS/UART to groundstation
### Requires local libcsp (not the one in `nanomind/`), since the code will be modified.

CSP_DIR=libcsp

INCLUDES = \
-Iinclude \
-I$(CSP_DIR)/include \
-I$(CSP_DIR)/build/include \
-I$(CSP_DIR)/src/

SOURCES = \
src/main.c \
src/util.c \
src/cli.c \
src/server.c 

FLAGS = \
-Wall \
-std=gnu99 \
-pthread

EXEC = csp-client 

LIBS = \
-lpthread \
$(CSP_DIR)/build/libcsp.a

CSP_CONFIG = \
--with-os=posix \
--with-driver-usart=linux \
--enable-if-tnc \
--enable-crc32 \
--enable-rdp

## Cmds 

all: csp
	gcc $(FLAGS) $(INCLUDES) $(SOURCES) $(LIBS) -o $(EXEC)
csp:
	cd libcsp; ./waf configure $(CSP_CONFIG); ./waf build

clean:
	cd libcsp; ./waf clean; cd ..; rm $(EXEC); 

fast: 
	gcc $(FLAGS) $(INCLUDES) $(SOURCES) $(LIBS) -o $(EXEC)
	clear
	./csp-client


