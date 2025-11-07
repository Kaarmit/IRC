# **************************************************************************** #
#                                  Makefile                                    #
# **************************************************************************** #

# Noms des exécutables
NAME        = irc_server
BONUS_NAME  = irc_bot

# Compilateur et flags
CXX         = c++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++98 -g3

# Dossiers
SRC_DIR     = requirements
BOT_DIR     = bot
OBJ_DIR     = obj

# Fichiers sources du serveur
SRCS        = $(SRC_DIR)/main.cpp \
			  $(SRC_DIR)/server/server.cpp \
			  $(SRC_DIR)/client/client.cpp \
			  $(SRC_DIR)/channel/channel.cpp \
			  $(SRC_DIR)/message/message.cpp \
			  $(SRC_DIR)/server/utils.cpp \
			  $(SRC_DIR)/server/commands/operator_cmds/invite.cpp \
			  $(SRC_DIR)/server/commands/communication/join.cpp \
			  $(SRC_DIR)/server/commands/operator_cmds/kick.cpp \
			  $(SRC_DIR)/server/commands/log.cpp \
			  $(SRC_DIR)/server/commands/operator_cmds/mode.cpp \
			  $(SRC_DIR)/server/commands/auth/nick.cpp \
			  $(SRC_DIR)/server/commands/communication/part.cpp \
			  $(SRC_DIR)/server/commands/auth/pass.cpp \
			  $(SRC_DIR)/server/commands/communication/privMSG.cpp \
			  $(SRC_DIR)/server/commands/communication/quit.cpp \
			  $(SRC_DIR)/server/commands/operator_cmds/topic.cpp \
			  $(SRC_DIR)/server/commands/auth/user.cpp

# Fichiers objets correspondants
OBJS        = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Fichiers sources du bot
BOT_SRCS    = $(BOT_DIR)/main.cpp \
			  $(BOT_DIR)/bot.cpp

# Fichiers objets du bot
BOT_OBJS    = $(BOT_SRCS:$(BOT_DIR)/%.cpp=$(OBJ_DIR)/bot/%.o)

# Commandes
RM          = rm -rf
MKDIR       = mkdir -p

# **************************************************************************** #

# Règle principale
all: $(NAME)

# Compilation du serveur
$(NAME): $(OBJS)
	@echo "🔧 Linking $(NAME)..."
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "✅ Build complete!"

# Compilation du bot (bonus)
bonus: $(BONUS_NAME)

$(BONUS_NAME): $(BOT_OBJS)
	@echo "🤖 Linking $(BONUS_NAME)..."
	$(CXX) $(CXXFLAGS) $(BOT_OBJS) -o $(BONUS_NAME)
	@echo "✨ Bot build complete!"

# Compilation générique des .o (serveur)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(MKDIR) $(dir $@)
	@echo "🧱 Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilation générique des .o (bot)
$(OBJ_DIR)/bot/%.o: $(BOT_DIR)/%.cpp
	@$(MKDIR) $(dir $@)
	@echo "🤖 Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage des objets
clean:
	@echo "🧹 Cleaning object files..."
	$(RM) $(OBJ_DIR)

# Nettoyage complet
fclean: clean
	@echo "🗑️ Removing executables..."
	$(RM) $(NAME) $(BONUS_NAME)

# Rebuild
re: fclean all

# **************************************************************************** #

.PHONY: all clean fclean re bonus


# # Nom de l'exécutable
# NAME        = irc_server

# # Compilateur et flags
# CXX         = c++
# CXXFLAGS    = -Wall -Wextra -Werror -std=c++98

# # Dossiers
# SRC_DIR     = requirements
# OBJ_DIR     = obj

# # Fichiers sources (récursif dans requirements)
# SRCS        = $(SRC_DIR)/main.cpp \
# 			  $(SRC_DIR)/server/server.cpp \
# 			  $(SRC_DIR)/client/client.cpp \
# 			  $(SRC_DIR)/channel/channel.cpp \
# 			  $(SRC_DIR)/message/message.cpp \
# 			  $(SRC_DIR)/server/utils.cpp \
# 			  $(SRC_DIR)/server/commands/operator_cmds/invite.cpp $(SRC_DIR)/server/commands/communication/join.cpp $(SRC_DIR)/server/commands/operator_cmds/kick.cpp \
# 			  $(SRC_DIR)/server/commands/log.cpp $(SRC_DIR)/server/commands/operator_cmds/mode.cpp $(SRC_DIR)/server/commands/auth/nick.cpp \
# 			  $(SRC_DIR)/server/commands/communication/part.cpp $(SRC_DIR)/server/commands/auth/pass.cpp \
# 			  $(SRC_DIR)/server/commands/communication/privMSG.cpp $(SRC_DIR)/server/commands/communication/quit.cpp $(SRC_DIR)/server/commands/operator_cmds/topic.cpp \
# 			  $(SRC_DIR)/server/commands/auth/user.cpp

# # Fichiers objets correspondants
# OBJS        = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# # Commandes
# RM          = rm -rf
# MKDIR       = mkdir -p

# # **************************************************************************** #

# # Règle principale
# all: $(NAME)

# # Compilation de l'exécutable
# $(NAME): $(OBJS)
# 	@echo "🔧 Linking $(NAME)..."
# 	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
# 	@echo "✅ Build complete!"

# # Compilation des .o
# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
# 	@$(MKDIR) $(dir $@)
# 	@echo "🧱 Compiling $<..."
# 	$(CXX) $(CXXFLAGS) -c $< -o $@

# # Nettoyage des objets
# clean:
# 	@echo "🧹 Cleaning object files..."
# 	$(RM) $(OBJ_DIR)

# # Nettoyage complet
# fclean: clean
# 	@echo "🗑️ Removing executable..."
# 	$(RM) $(NAME)

# # Rebuild
# re: fclean all

# # **************************************************************************** #

# .PHONY: all clean fclean re
