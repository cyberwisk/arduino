# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile
PROJECT_DIR = /home/rene/Arduino_make_apps/Spectrum/spectrum
ARDMK_DIR = $(ARDUINO_DIR)
ARDUINO_DIR = /usr/share/arduino
#Caso as bibliotecas estejam na pasta do projeto
#USER_LIB_PATH := $(PROJECT_DIR)/lib 
BOARD_TAG = uno
ARDUINO_LIBS = LedControl FHT
MONITOR_BAUDRATE = 115200
AVR_TOOLS_DIR = /usr
AVRDUDE = /usr/bin/avrdude
CFLAGS_STD = -std=gnu11
CXXFLAGS_STD = -std=gnu++11
CXXFLAGS += -pedantic -Wall -Wextra
ARDUINO_PORT = /dev/ttyACM0
MONITOR_PORT = /dev/ttyACM0
CURRENT_DIR = $(shell basename $(CURDIR))
OBJDIR = $(PROJECT_DIR)/bin/$(BOARD_TAG)/$(CURRENT_DIR)
include $(ARDUINO_DIR)/Arduino.mk 

