# Nome do executável final
BINARY = programa

# Lista de todos os arquivos .c (o wildcard pega todos automaticamente)
SOURCES = $(wildcard *.c)

# Transforma a lista de .c em uma lista de .o (objetos)
OBJECTS = $(SOURCES:.c=.o)

# Compilador e flags
CC = gcc
FLAGS = -Wall -Werror -g # -Wall mostra avisos, -g permite usar o GDB/Valgrind

# Regra principal: Gera o binário
all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) -o $(BINARY) -lm

# Regra para compilar os arquivos de objeto
%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@

# Regra para limpar os arquivos temporários
clean:
	rm -f $(OBJECTS) $(BINARY)

# Regra para rodar o programa (ajuste conforme necessário)
run: all
	./$(BINARY)