#!/bin/sh
# faire chmod +x run.sh pour autoriser l'execution ! puis executer comme programme.


EXEC="sokoban"

echo "Compilation en cours..."

if gcc sokoban.c -o "$EXEC"; then
    echo "Compilation réussie !"
else
    echo "Erreur : la compilation a échoué."
    exit 1
fi

echo "Lancement du jeu..."
./"$EXEC"

