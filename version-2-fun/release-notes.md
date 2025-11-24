# Sokoban Release notes

Mise à jour du Sokoban  

---

## Release Notes
<!--
> [!WARNING]
> Le projet est toujours en cours de réalisation. Certains éléments sont donc susceptibles de ne pas fonctionner.

-->

## Version V2.3.4
- modification deplacer(), jeu(), sauvegarde_jeu(),  memoriser_deplacement(), annuler_deplacement()
- creation de get_touche_y_n(), sauvegarde_deplacements(), deplacer_joueur()
- suppr affichage du tab de déplacements
- enregistremement meme si gagner

- add gif (beta)



## Version V2.3.3
- ajout procedure enrigistrer déplacements 
- modification des méssages de fin de partie 
- ajout de couleurs sur d'autres textes 
- commentaire sur certaines procedures 


## Version V2.3.2 
- modification du main :
  - création d'une procédure "rejouer" pour alléger la main.
  - création d'une procédure "affiche" erreur qui affiche un message d'erreur si le nombre de déplacements est trop grand.

- modification des procdures : 
  - rejouer -> plus besoin d'appuyer sur entrer pour valider
  - sauvegarde_jeu -> plus besoin d'appuyer sur entrer pour valider



## Version V2.3.1

- Correction du problème de suppression des cibles lors de l'undo avec caisse
- modification de la procedure remplace_carac_deplacement
  - changement du nom remplace_carac_deplacement -> remplace_caractere
  - modification des noms de variables 
  

## Version V2.3.0
- Correction du problème de suppression des cibles lors de l'undo sans caisse
- fix affichage du tableau de débug des déplacements 

