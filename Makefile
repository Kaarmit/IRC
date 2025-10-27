# **************************************************************************** #
#                                  Makefile                                    #
# **************************************************************************** #

# Nom de l'exécutable
NAME        = irc_server

# Compilateur et flags
CXX         = c++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++98

# Dossiers
SRC_DIR     = requirements
OBJ_DIR     = obj

# Fichiers sources (récursif dans requirements)
SRCS        = $(SRC_DIR)/main.cpp \
			  $(SRC_DIR)/server/server.cpp \
			  $(SRC_DIR)/client/client.cpp \
			  $(SRC_DIR)/channel/channel.cpp \
			  $(SRC_DIR)/message/message.cpp \
			  $(SRC_DIR)/server/utils.cpp \
			  $(SRC_DIR)/server/commands/invite.cpp $(SRC_DIR)/server/commands/join.cpp $(SRC_DIR)/server/commands/kick.cpp \
			  $(SRC_DIR)/server/commands/log.cpp $(SRC_DIR)/server/commands/mode.cpp $(SRC_DIR)/server/commands/nick.cpp \
			  $(SRC_DIR)/server/commands/part.cpp $(SRC_DIR)/server/commands/pass.cpp $(SRC_DIR)/server/commands/ping.cpp \
			  $(SRC_DIR)/server/commands/privMSG.cpp $(SRC_DIR)/server/commands/quit.cpp $(SRC_DIR)/server/commands/topic.cpp \
			  $(SRC_DIR)/server/commands/user.cpp $(SRC_DIR)/server/commands/who.cpp 

# Fichiers objets correspondants
OBJS        = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Commandes
RM          = rm -rf
MKDIR       = mkdir -p

# **************************************************************************** #

# Règle principale
all: $(NAME)

# Compilation de l'exécutable
$(NAME): $(OBJS)
	@echo "🔧 Linking $(NAME)..."
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "✅ Build complete!"

# Compilation des .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(MKDIR) $(dir $@)
	@echo "🧱 Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage des objets
clean:
	@echo "🧹 Cleaning object files..."
	$(RM) $(OBJ_DIR)

# Nettoyage complet
fclean: clean
	@echo "🗑️ Removing executable..."
	$(RM) $(NAME)

# Rebuild
re: fclean all

# **************************************************************************** #

.PHONY: all clean fclean re
