# First Part

## Manipulation 3.1

On peut dechiffrer en executant de nouveau. C'est un simple xor, donc ça reinverse.

## Manipulation 3.2

SEEK_CUR = 1

# Second Part

## Manipulation 4.1

![alt text](image.png)
![alt text](image-1.png)

## Quesiton 4.1

Que fait le code au debut de main avant l'invocation de la fonction is_prime ? Quel est le but du malware, d'apres vous ?

Le malware lance l'application calculator de gnome afin de faire passer son vrai attaque malicieux en arriere plan sur le main thread. Le thread qui est lancé contient l'application calculator. La synchronisation des threads se fait à la fin de l'attaque du chiffrement du directory.

## Question 4.2

Que fait la fonction is_prime invoquée au döbut du main ? Quel est le but du malware, d'après vous ?

La fonction prime permet de savoir si les deux valeurs entrant sont premier entre les deux param qui sont envoyés dans la fonction.

## Question 4.3

Quelles sont les valeurs passées aux appels à is_prime?

Ce sont deux valeurs premier entre eux.