# Makefile for building embedded application.
# by Brian Fraser

# Edit this file to compile extra C files into their own programs.
TARGET= beatbox
PROJECT_NAME=beatbox-server
DEPLOY_PATH= $(HOME)/cmpt433/public/myApps/$(PROJECT_NAME)-copy

SOURCES= main.c audioMixer.c utils.c DrumBeatsPlayer.c ./Hardware/joystick.c ZenCapeInput.c ./Hardware/accelerometer.c UDP_Listener.c


PUBDIR = $(HOME)/cmpt433/public/myApps
OUTDIR = $(PUBDIR)
CROSS_TOOL = arm-linux-gnueabihf-
CC_CPP = $(CROSS_TOOL)g++
CC_C = $(CROSS_TOOL)gcc

CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror


# Asound process:
# get alibsound2 lib on target:
# 	# apt-get install libasound2
# Copy target's /usr/lib/arm-linux-gnueabihf/libasound.so.2.0.0 
#      to host  ~/public/asound_lib_BBB/libasound.so
# Copy to just base library:

LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB


# -pg for supporting gprof profiling.
#CFLAGS += -pg



all: wav 
	$(CC_C) $(CFLAGS) $(SOURCES) -o $(OUTDIR)/$(TARGET)  $(LFLAGS) -lpthread -lasound

# Copy wave files to the shared folder
wav:
	mkdir -p $(PUBDIR)/beatbox-wav-files/
	cp beatbox-wav-files/* $(PUBDIR)/beatbox-wav-files/ 

clean:
	rm -f $(OUTDIR)/$(TARGET)
	PROJECT_NAME=10-UdpRelay

deploy:
	@echo 'COPYING THE NODE.JS FILES TO $(DEPLOY_PATH)'
	@echo ''
	mkdir -p $(DEPLOY_PATH)
	chmod a+rwx $(DEPLOY_PATH)
	cp -a ./server/. $(DEPLOY_PATH)
	@echo 'Do not edit any files in this folder; they are copied!' > $(DEPLOY_PATH)/DO_NOT_EDIT-FILES_COPIED.txt
	@echo ''
	@echo 'NOTE: On the host, in $(DEPLOY_PATH), it is best to run: npm install'
	@echo '      Or, just run the node_install target in this makefile.'

node_install:
	@echo ''
	@echo ''
	@echo 'INSTALLING REQUIRED NODE PACKAGES'
	@echo '(This may take some time)'
	@echo ''
	cd $(DEPLOY_PATH) && npm install