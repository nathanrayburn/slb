# SLB-2024 – Labo01 – ransTroj

## Auteurs

    - Amir Mouti
    - Ouweis Harun
    - Nathan Rayburn

## First Part

### Manipulation 3.1

On peut déchiffrer en exécutant de nouveau. C'est un simple xor, donc ça réinverse.

### Manipulation 3.2

SEEK_CUR = 1

---

## Second Part: Structure du code

### Manipulation 4.1

![alt text](image.png)
![alt text](image-1.png)

### Quesiton 4.1 Que fait le code au debut de main avant l'invocation de la fonction is_prime ? Quel est le but du malware, d'apres vous ?

Le malware lance l'application calculator de gnome afin de faire passer son vrai attaque malicieux en arriere plan sur le main thread. Le thread qui est lancé contient l'application calculator. La synchronisation des threads se fait à la fin de l'attaque du chiffrement du directory.

### Question 4.2 Que fait la fonction is_prime invoquée au döbut du main ? Quel est le but du malware, d'après vous ?

La fonction prime permet de savoir si les deux valeurs entrant sont premier entre les deux param qui sont envoyés dans la fonction.

### Question 4.3 Quelles sont les valeurs passées aux appels à is_prime?

Ce sont deux valeurs qui sont premiers entre eux.

### Question 4.4 : Que fait le code C selon les différentes valeurs de retour de la fonction `is_prime` ?

_En attente de réponse et analyse._

### Question 4.5 : Que fait la fonction `encrypt_dir` invoquée après l’appel à `is_prime` dans `main` ?

_En attente de réponse et analyse._

### Question 4.6 : Quelles sous-fonctions sont appelées par la fonction `encrypt_dir` et à quelle condition ?

_En attente de réponse et analyse._

### Question 4.7 : Quelles exceptions prévoit le code de `encrypt_dir` ? Pourquoi ?

_En attente de réponse et analyse._

### Manipulation 4.2

Dans cette manipulation, nous avons commenté le code C dans Ghidra pour répondre aux questions 4.1 à 4.7. Nous avons renommé et/ou "retypé" les variables locales selon nécessité.

### Question 4.8 : Analysez le code assembleur correspondant à la fonction `encrypt_dir` :

- **Parties de gestion de la pile** : _En attente d'identification._
- **Instructions de chiffrement** : _En attente d'explication._

---
