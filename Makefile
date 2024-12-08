# Makefile

# Alvo padrão
all: x

# Compilação
x: main.o game.o bomb.o map.o player.o screens.o
	gcc -o x main.o game.o bomb.o map.o player.o screens.o -lraylib -lm -lpthread -ldl -lrt -lX11

# Regras para cada arquivo-fonte
main.o: main.c
	gcc -c main.c -o main.o

game.o: game.c
	gcc -c game.c -o game.o

bomb.o: bomb.c
	gcc -c bomb.c -o bomb.o

map.o: map.c
	gcc -c map.c -o map.o

player.o: player.c
	gcc -c player.c -o player.o

screens.o: screens.c
	gcc -c screens.c -o screens.o

# Limpeza
clean:
	rm -f x main.o game.o bomb.o map.o player.o screens.o
