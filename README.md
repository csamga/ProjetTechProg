# Projet Gestion de Commerce
### Auteurs : Lohan Fleury et Gabin Mascioli

## TODO
- [x] Implémenter modes
- [x] Implémenter actions base
- [x] Implémenter actions modes
- [x] Implémenter sélection modes
- [x] Implémenter sélection actions
- [ ] Gérer le mode vente et actions de base
- [ ] Définir structures
    - [x] Client
    - [x] Fournisseur
    - [x] Produit
    - [ ] Commande
    - [ ] Livraison
    - [ ] Transaction
- [ ] Implémenter bases de données
    - [x] Client
    - [x] Produit
    - [x] Fournisseur
    - [ ] Implémenter base de données par structure
        - [x] Historique achat client
        - [ ] Historique comandes fournisseur
        - [ ] Commandes en attente
        - [ ] Historique ventes par produit
        - [x] Inventaire
- [x] Implémenter confirmation suppression
- [ ] Implémenter recherche top level
- [ ] Implémenter choix recherche
- [ ] Implémenter résolution conflits recherche
- [ ] Nom rue peut contenir chiffres
- [ ] Remonter curseur quand input invalide
- [ ] Modifier champs adresse
- [ ] Simplifier fonctions market
- [ ] Ajouter retour ligne après indication mode
- [ ] Ristourne

## Description des modes

Le programme est divisé en 4 modes :
- Client
- Fournisseur
- Produit
- Vente

Chaque mode propose différentes actions contextuelles qui sont, au même titre
que les modes, sélectionnées avec le pavé numérique.

Les modes **Client**, **Fournisseur** et **Produit** proposent 4 actions de base
opérant sur l'objet du mode en question :
- Enregistrer
- Modifier
- Consulter
- Supprimer

En plus de ces 4 actions de bases, les 4 modes permettent d'exécuter des actions
spécifiques.

### Client
- Consulter l'historique d'achat

### Fournisseur
- Passer une commande
- Enregistrer la réception de commande

### Produit
- Consulter l'inventaire des produits

### Vente
- Enregistrer une transaction
- Consulter l'historique des ventes

## Compilation

Il est préférable d'utiliser l'outil ```make``` pour construire le programme.

Le fichier ```Makefile``` est conçu pour détecter automatiquement les fichiers
```*.c``` situés dans le dossier ```src/```. Le résultat de la compilation est
placé dans le dossier ```tmp/```. Enfin, les objets sont liés et l'exécutable
est placé dans le dossier ```bin/```.

En parallèle, l'arborescence de dossiers de bases de données située dans le
dossier ```db/``` est créée. Seuls les dossiers sont créés. Le programme est
responsable de l'oberture/fermeture des fichiers.

### Pour compiler le programme

```
$ make
```

### Pour lancer le programme

Deux options :

```
(1) $ make run

(2) $ ./bin/projet
```
