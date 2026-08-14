# Sokoban  

<div align="center">

![Sokoban](https://img.shields.io/badge/Jeu-Sokoban-blue?style=for-the-badge)
![Langage](https://img.shields.io/badge/Language-C-00599C?style=for-the-badge&logo=c)
![Terminal](https://img.shields.io/badge/Interface-Terminal-black?style=for-the-badge)
![Version](https://img.shields.io/badge/Version-2.3.7-green?style=for-the-badge)

**Jeu Sokoban réalisé en C, jouable directement dans le terminal**

[🕹️ Commandes](#-commandes) • [🚀 Installation](#-installation) • [📖 Lancer-une-partie](#-lancer-une-partie) • [🖼️ Aperçu](#-aperçu)

</div>


<br>
<br>  

[Sokoban autonome](https://github.com/TMoq22/Sokoban-autonome)

<br>


## 📝 Description

Ce projet est une implémentation complète du **jeu Sokoban**, codée en **C** et jouable directement dans un **terminal** compatible ANSI.

Il s'agit d'un projet réalisé dans le cadre de ma formation au **BUT Informatique** à l’**IUT de Lannion**.

**Objectif du jeu :**  
Déplacer le personnage pour **pousser toutes les caisses (`$`) sur les cibles (`.`)**.  
Impossible de tirer une caisse : la stratégie est essentielle !

---

## 🕹️ Commandes

| Touche | Action |
|:------:|:-------|
| `Z` | ⬆️ Monter |
| `S` | ⬇️ Descendre |
| `Q` | ⬅️ Aller à gauche |
| `D` | ➡️ Aller à droite |
| `U` | ↩️ Annuler le dernier mouvement |
| `+` | 🔍 Zoomer |
| `-` | 🔎 Dézoomer |
| `R` | 🔄 Recommencer le niveau |
| `X` | 🚪 Quitter (avec option de sauvegarde) |

> Les touches ZQSD correspondent à la disposition AZERTY.

---

## 🚀 Installation

### Prérequis
- GCC  
- Terminal Unix/Linux ou macOS (fonctionne aussi sur Windows via WSL)

### Compilation

```bash
gcc sokoban.c -o sokoban
```
## 📖 Lancer une partie
```bash
./sokoban
```

Sinon utiliser le script run.sh
```bash
chmod +x run.sh
```
puis 
```bash
./run.sh
```

Le programme affiche la liste des fichiers .sok disponibles, puis demande le niveau :

Entrer un niveau : niveau1.sok

<img width="715" height="253" alt="Capture d’écran du 2025-12-01 10-45-24" src="https://github.com/user-attachments/assets/4e389109-70de-4ed9-9783-8b53f0958e79" />


---

## ✨ Aperçu 
<img width="348" height="502" alt="image" src="https://github.com/user-attachments/assets/77e127cb-7024-44bb-b1af-7cf0c6bd46a3" />
<img width="348" height="502" alt="image" src="https://github.com/user-attachments/assets/8afadedc-9202-4ed0-a7ca-d202e1ffd717" />
<img width="348" height="502" alt="image" src="https://github.com/user-attachments/assets/a3d7ec40-81ae-4fc6-8f6d-dab6e4662185" />

---
## ⚠️ Limitations connues
- Plateau fixe de 12×12 cases
- Historique limité à 5000 déplacements 
- Nécessite un terminal unix compatible ANSI
---
## 📄 Licence

Projet sous licence MIT — libre d’utilisation, modification et redistribution.  
[Voir licence](/LICENSE)

---
### 👤 Auteur

Titouan Moquet
BUT Informatique,   
Promo 2025–2026 IUT de Lannion  
<img width="114" height="37" alt="image" src="https://github.com/user-attachments/assets/59097d28-f71c-4109-a4bb-8964ddbde6bd" />

---
<div align="center">
  
 **Bon jeu ! 🎮** 
</div>
