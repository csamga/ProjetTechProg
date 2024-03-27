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
    - [ ] Fournisseur
    - [x] Produit
    - [ ] Commande
    - [ ] Livraison
    - [ ] Transaction
- [ ] Implémenter bases de données
    - [x] Client
    - [ ] Produits
    - [x] Fournisseurs
    - [ ] Historique achat par client
    - [ ] Historique ventes par produit


Le programme est divisé en 4 modes :
- Client
- Fournisseur
- Produit
- Vente

Chaque mode propose différentes actions contextuelles qui sont, au même titre
que les modes, sélectionnées avec le pavé numérique.

## Description des modes

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
