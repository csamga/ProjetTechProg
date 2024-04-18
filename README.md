# Projet Gestion de Commerce
### Auteurs : Lohan Fleury et Gabin Mascioli

## TODO
- [x] Implémenter modes
- [x] Implémenter actions base
- [x] Implémenter actions modes
- [x] Implémenter sélection modes
- [x] Implémenter sélection actions
- [x] Gérer le mode vente et actions de base
- [x] Définir structures
    - [x] Client
    - [x] Fournisseur
    - [x] Produit
    - [x] Commande
    - [x] Livraison
    - [x] Transaction
- [x] Implémenter bases de données
    - [x] Client
    - [x] Produit
    - [x] Fournisseur
    - [x] Implémenter base de données par structure
        - [x] Historique achat client
        - [x] Historique commandes fournisseur
        - [x] Commandes en attente
        - [x] Inventaire
- [x] Implémenter confirmation suppression
- [ ] Implémenter recherche top level
- [ ] Implémenter choix recherche
- [ ] Implémenter résolution conflits recherche
- [x] Nom rue peut contenir chiffres
- [x] Remonter curseur quand input invalide
- [ ] Modifier champs adresse
- [ ] Simplifier fonctions market
- [x] Ajouter retour ligne après indication mode
- [ ] Reduction bon client
- [ ] Gérer fonction employer/client

## Description des modes

Le programme est divisé en 4 modes :
- Client
- Fournisseur
- Produit
- Vente

Chaque mode propose différentes actions contextuelles qui sont, au même titre
que les modes, sélectionnées avec le pavé numérique.

Les modes **Client**, **Fournisseur** et **Produit** proposent 4 actions de base
opérant sur l'item du mode en question :
- Enregistrer : Permet de créer les informations de l'item dans la base de données correspondante.
- Modifier : Permet de modifier les information de l'item dans la base de données correspondante.
- Consulter : Permet de consulter les informations de l'item dans la base de données correspondante. 
- Supprimer : Permet de supprimer l'enregistrement de l'item dans la base de données correspondante.

En plus de ces 4 actions de bases, les 4 modes permettent d'exécuter des actions
spécifiques.

### Client
- Consulter l'historique d'achat : Permet de consulter toute les ventes faites à ce client en accedant a sa base de données personnelle.

### Fournisseur
- Passer une commande : Permet d'enregistrer une commande dans le fichier du fournisseur concerné.
- Consulter les commandes en attente : Permet de consulter les commandes en attente de reception.
- Enregistrer la réception de commande : Permet de réceptionner la commande et de mettre à jour le stock des produits commandés.

### Produit
- Consulter l'inventaire des produits : Permet d'afficher le stock du magasin. Affiche les produits ainsi que leurs informations personnelles.

### Vente
- Enregistrer une transaction : Permet de vendre un produit a un client en l'enregistrant cette information dans la base de données du client   concerné.

Il existe aussi deux actions qui permettent de revenir au choix des modes et de quitter le programme.

## Compilation

Il est préférable d'utiliser l'outil ```make``` pour construire le programme.

Le fichier ```Makefile``` est conçu pour détecter automatiquement les fichiers
```*.c``` situés dans le dossier ```src/```. Le résultat de la compilation est
placé dans le dossier ```tmp/```. Enfin, les objets sont liés et l'exécutable
est placé dans le dossier ```bin/```.

En parallèle, l'arborescence de dossiers de bases de données située dans le
dossier ```db/``` est créée. Seuls les dossiers sont créés. Le programme est
responsable de l'ouverture/fermeture des fichiers. 

[!CAUTION]
Il faut absolument éviter de supprimer le dossier ```db/``` afin de ne pas influencer le bon fonctionnement du programme.

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
