#!/bin/bash
CC      := gcc
TARGET  := findFiles

INCLUDE := include
SRCDIR  := .
FLAGS   := -O5 -Wall -g
LIBS	:= -lrt

SOURCE_LIST := $(wildcard $(foreach path, $(SRCDIR), $(path)/*.c))
OBJ_LIST    := $(SOURCE_LIST:.c=.o)

CLEAN_LIST  := "*.o" "*.*~" "*~" "*.d"

all: $(OBJ_LIST)
	@$(CC) $(LIBS) $(OBJ_LIST) -o $(TARGET)
	@echo  
		@echo  "**** Successfully compiled **** now you can run"  ./$(TARGET)
	@echo
	
%.o:%.c
	@echo " * Compiling - "$^
	@$(CC) -c $(FLAGS) -I$(INCLUDE) $^ -o $@

list: 
	@echo 
	@echo —————————————————— SRCDIR ————————————————
	@echo 
	@echo $(SRCDIR)
	@echo 
	@echo ———————————————— SOURCE_LIST —————————————
	@echo 
	@echo $(SOURCE_LIST)
	@echo 
	@echo ————————————————— OBJ_LIST ———————————————
	@echo 
	@echo $(OBJ_LIST)
	@echo 

.PHONY : clean
clean:      
	@echo ============= CLEAN =============
	@find . \( -name $(word 1, $(CLEAN_LIST)) $(foreach filter, $(CLEAN_LIST),-o -name $(filter)) \) -delete -print
	@rm -f -v $(TARGET)
	@echo
	
