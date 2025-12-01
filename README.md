# Sokoban 


<div align="center">

![Sokoban](https://img.shields.io/badge/Jeu-Sokoban-blue?style=for-the-badge)
![Langage](https://img.shields.io/badge/Langage-C-00599C?style=for-the-badge&logo=c)
![Terminal](https://img.shields.io/badge/Interface-Terminal-black?style=for-the-badge)
![Version](https://img.shields.io/badge/Version-2.3.7-green?style=for-the-badge)

**Jeu du Sokoban réalisé en C jouable dans le terminal**

<!--  [🎮 Fonctionnalités](#-fonctionnalités) • [🕹️ Commandes](#️-commandes) • [🚀 Installation](#-installation) • [📖 Usage](#-usage)
-->
</div>
<!--
<img width="378" height="483" alt="image du jeu" src="https://github.com/user-attachments/assets/7a1c55fa-0154-4056-82de-c6d4ace259de" />
-->
---


## 📝 Description

Ce projet est une implémentation complète du jeu **Sokoban**, codé en **C** et jouable directement dans le **terminal**.  

**Objectif du jeu :** Pousser toutes les caisses (`$`) sur les cibles (`.`) en un minimum de déplacements. Simple en apparence, mais diablement addictif !

---

## Commandes

| Touche | Action |
|:------:|:-------|
| `Z` | ⬆️ Monter |
| `S` | ⬇️ Descendre |
| `Q` | ⬅️ Aller à gauche |
| `D` | ➡️ Aller à droite |
| `U` | ↩️ **Annuler** le dernier mouvement |
| `+` | 🔍 **Zoom** (agrandir le plateau) |
| `-` | 🔎 **Dézoom** (rétrécir le plateau) |
| `R` | 🔄 Recommencer le niveau |
| `X` | 🚪 Quitter (avec option de sauvegarde) |


> **Note :** Les touches ZQSD correspondent à la disposition AZERTY française.
---

## Installation

### Prérequis
- GCC (GNU Compiler Collection)
- Terminal Unix/Linux ou macOS

### Compilation

```bash
gcc sokoban(version).c -o sokoban(version)
```
---

## Lancer une partie

```bash
./sokoban
```

---
Titouan Moquet Info 1D2
IUT Lannion 2025-2026
