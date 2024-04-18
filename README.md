# Projet Gestion de Commerce
### Auteurs : Lohan Fleury et Gabin Mascioli

## Description des modes

Le programme est divisé en 4 modes :
- Client
- Fournisseur
- Produit
- Vente

Chaque mode propose différentes actions contextuelles qui sont, au même titre
que les modes, sélectionnées avec le pavé numérique.

> On définit **item** comme étant un produit, un client ou un fournisseur.

Les modes **Client**, **Fournisseur** et **Produit** proposent 4 actions de base
opérant sur l'item du mode en question :

- **Enregistrer**
    - Permet de créer les informations de l'item dans la base de données
    correspondante.
- **Modifier**
    - Permet de modifier les information de l'item dans la base de données
    correspondante.
- **Consulter**
    - Permet d'afficher les informations de l'item dans la base de données
    correspondante. 
- **Supprimer**
    - Permet de supprimer l'enregistrement de l'item dans la base de données
    correspondante ainsi que la base de données qui y est rattachée le cas
    échéant.

> [!NOTE]
> En plus de ces 4 actions de bases, les modes permettent d'exécuter des
actions spécifiques.

### Client
- **Consulter l'historique d'achat**
    - Permet de consulter touts les achats d'un client en affichant les
    produits, le sous-total et le total. Il est nécessaire que le client existe
    dans la base de données.

### Fournisseur
- **Passer une commande**
    - Permet d'enregistrer une commande à un fournisseur en saisissant chaque
    produit et sa quantité. Il est nécessaire que le fournisseur et le produit
    existent dans la base de données.
- **Consulter les commandes en attente**
    - Permet de consulter les commandes en attente de reception pour un
    fournisseur. Il est nécessaire que le fournisseur existe et qu'il ait au
    moins une commande à livrer.
- **Enregistrer la réception de commande**
    - Permet de réceptionner une commande en la supprimant des commandes en
    attente du fournisseur et de mettre à jour le stock des produits concernés.

### Produit
- **Consulter l'inventaire des produits**
    - Permet d'afficher le stock du magasin. Affiche les informations et la
    quantité disponible de chaque produit sous forme de tableau.

### Vente
- **Enregistrer une transaction**
    - Enregistre la vente d'un ou plusieurs produits à un client en mettant à
    jour le stock des articles vendus. Il est nécessaire que le client existe et
    que le stock de chaque produit demandé ne soit pas nul.

> [!NOTE]
> Il existe également les actions **Retour** et **Quitter** qui permettent
respectivement de revenir au choix des modes et de quitter le programme.

## Bases de données

Les bases de données sont stockées dans le dossier ```db/```. Le programme fait
usage d'une base de données globale par item (produit, client, fournisseur).

- La base de données article ```db/product_db.dat``` contient tous les
produits et leurs informations (identifiant, libellé, marque, provenance, masse,
prix, stock disponible).
- La base de données client ```db/client_db.dat``` contient tous les clients
et leurs informations (identifiant, nom, prénom, téléphone, email, adresse, nom
du fichier de la base de données dédiée).
- La base de données fournisseur ```db/supplier_db.dat``` contient tous les
fournisseurs et leurs informations (identifiant, nom, prénom, téléphone, email,
adresse, nom du fichier de la base de données dédiée).

Par ailleurs, les items **Client** et **Fournisseur** possèdent chacun une base
de données respective.

- L'historique d'achat de chaque client ```db/clients/client_<ID>_db.dat```
conserve chaque achat (produit, quantité, prix sous-total, prix total).
- L'historique des commandes passés à chaque fournisseur
```db/suppliers/supplier_<ID>_db.dat```
conserve chaque commande passée (identifiant commande, tableau d'identifiants
produit, tableau de quantité par produit, nombre de produits, prix total).

## Interface

L'outil de gestion est divisé en "pages" qui se succèdent.

1. On choisit en premier le mode d'action parmi les modes présentés ci-dessus.
2. Une nouvelle page permet ensuite de sélectioner une action contextuelle à
réaliser.
3. Enfin, l'action demande de saisir certaines informations dans une dernière
page.
Après achèvement de l'action, on retourne à l'étape du choix de l'action dans le
même mode.

> [!NOTE]
> Les couleurs, nouvelles pages et positionnement du curseur ont été rendus
possibles grâce à l'utilisation de
[séquences d'échappenent ANSI](https://en.wikipedia.org/wiki/ANSI_escape_code)
(ANSI Escape codes).

## Remarques

- Nous avons opté pour la polyvalence des fournisseurs. C'est-à-dire que
n'importe quel fournisseur peut livrer n'importe quel produit.
- La marge réalisée est de **25%** par défaut.

## Compilation

> [!TIP]
> Il est préférable d'utiliser l'outil ```make``` pour construire le programme.

Le fichier ```Makefile``` est conçu pour détecter automatiquement les fichiers
```*.c``` situés dans le dossier ```src/```. Le résultat de la compilation est
placé dans le dossier ```tmp/```. Enfin, les objets ```*.o```sont liés et
l'exécutable est placé dans le dossier ```bin/```.

En parallèle, l'arborescence de dossiers des bases de données située dans le
dossier ```db/``` est créée. Seuls les dossiers sont créés. Le programme est
responsable de la création/ouverture/fermeture des fichiers. 

> [!CAUTION]
> Il faut absolument éviter de supprimer le dossier ```db/``` afin de ne pas
influencer le bon fonctionnement du programme.

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

### Pour nettoyer l'arborescence

Supprime le contenu des dossiers ```bin/``` et ```tmp/```

```
$ make clean
```

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
- [x] Implémenter bases de données
    - [x] Client
    - [x] Produit
    - [x] Fournisseur
    - [x] Implémenter base de données par structure
        - [x] Historique achat client
        - [ ] Historique commandes fournisseur
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
- [ ] Implémenter dégradations tarifaires (toutes les n ventes, n à définir)
- [ ] Gérer sessions employé/gérant
- [ ] Proposer enregistrement/commande quand pas de produits/clients/stock vide
