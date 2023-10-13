# *********************************************************************************** #
#       :::    ::: ::::::::  :::::::::   ::::::::  :::    :::     :::     ::::::::::: #
#      :+:   :+: :+:    :+: :+:    :+: :+:    :+: :+:   :+:    :+: :+:       :+:      #
#     +:+  +:+  +:+    +:+ +:+    +:+ +:+    +:+ +:+  +:+    +:+   +:+      +:+       #
#    +#++:++   +#+    +:+ +#+    +:+ +#+    +:+ +#++:++    +#++:++#++:     +#+        #
#   +#+  +#+  +#+    +#+ +#+    +#+ +#+    +#+ +#+  +#+   +#+     +#+     +#+         #
#  #+#   #+# #+#    #+# #+#    #+# #+#    #+# #+#   #+#  #+#     #+#     #+#          #
# ###    ### ########  #########   ########  ###    ### ###     ### ###########       #
#                        <kodokai.featheur@gmail.com>                                 #
# *********************************************************************************** #

EXEC 		= vulkan_project
MAIN 		= main.c
CC 			= gcc
SPVC 		= glslc
DEBUGGER 	= ddd
FLAG 		= -Wall -Wextra
PATH_HEAD	= ./headers/
PATH_SRC 	= ./sources/
PATH_SHD 	= ./shaders/
PATH_LIB 	= ./lib/
LIB 		= $(wildcard $(PATH_LIB)*.a) -lglfw -lvulkan -lm
SRC 		= $(wildcard $(PATH_SRC)*.c)
SHD 		= $(wildcard $(PATH_SHD)*.vert) $(wildcard $(PATH_SHD)*.frag)
SPV			= $(patsubst %.vert, %.spv, $(patsubst %.frag, %.spv, $(SHD)))
OBJ 		= $(SRC:.c=.o)
MAIN_O 		= $(MAIN:.c=.o)

all: $(EXEC)

debug: fclean debug_flags all 
	$(DEBUGGER) debug

debug_flags:
	$(eval FLAG += -g)
	$(eval EXEC = debug)

$(EXEC): $(OBJ) $(MAIN_O) $(SPV)
	$(CC) $(FLAG) -I $(PATH_HEAD) $(MAIN_O) $(OBJ) $(LIB) -o $(EXEC) 

%.o: %.c
	$(CC) $(FLAG) -c -I $(PATH_HEAD) $< -o $@

$(SPV): $(SHD) 
	$(SPVC) $(firstword $(SHD)) -o $(firstword $(SPV))
	$(eval SPV = $(wordlist 2, $(words $(SPV)), $(SPV)) $(firstword $(SPV)))
	$(eval SHD = $(wordlist 2, $(words $(SHD)), $(SHD)) $(firstword $(SHD)))
	
clean:
	rm -f $(OBJ) $(MAIN_O)

fclean: clean
	rm -f $(EXEC) $(SPV) debug

re: fclean all

.PHONY:		all debug debug_flags clean fclean re
