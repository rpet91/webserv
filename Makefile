NAME =			webserv

CC =			clang++
CFLAGS =		-Wall -Werror -Wextra -std=c++98 -pedantic

SDIR =			src
ODIR =			obj
IDIR =			inc

CLASSES =		Server Parser ServerConfig WebServer Task Client

_OBJS =			main $(CLASSES)
OBJS =			$(addsuffix .o, $(addprefix $(ODIR)/, $(_OBJS)))
HEADERS =		$(addsuffix .hpp, $(addprefix $(IDIR)/, $(CLASSES)))

ifeq ($(DEBUG),1)
	CFLAGS += -fsanitize=address -g3
endif

.PHONY: all clean fclean re debug

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(ODIR)/%.o: $(SDIR)/%.cpp $(HEADERS)
	@mkdir -p $(ODIR)
	$(CC) $(CFLAGS) -I$(IDIR) -c $< -o $@

clean:
	$(RM) $(OBJS)
	$(RM) *.dSYM

fclean: clean
	$(RM) $(NAME)

re: fclean all

debug:
	@make DEBUG=1