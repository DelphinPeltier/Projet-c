# Nom de l'exécutable
EXEC = prog

# Liste des fichiers objets
OBJS = main.o

# Règle par défaut
all: $(EXEC)

# Édition de liens
$(EXEC): $(OBJS)
	gcc $(OBJS) -o $(EXEC)

# Compilation des .c (situés dans src/) en .o
%.o: src/%.c
	gcc -Iinclude -c $< -o $@

# Nettoyage
clean:
	rm -f $(OBJS) $(EXEC)

# Exécution
run: $(EXEC)
	./$(EXEC)