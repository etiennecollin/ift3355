# IFT3355 | Raytracer

Vos noms et matricules:

- Etienne Collin | 20237904
- Louis Malenfant-Poulin | 20120851

## Notes

Certaines fonctionnalités ont été ajoutées en plus de celles demandées.

- Notre réfraction utilise un _stack_ afin d'obtenir une réfraction réaliste. Le _stack_ contient des paires `(id, IOR)` où l'`id` est l'index de l'objet et l'`IOR` est son indice de réfraction. Les résultats obtenus sont donc différents des références qui utilisent un `eta=1/mat.refractive_index`.
  - `./src/basic.h`: Ajout d'un `ior_stack` dans la classe `RAY` qui contient le _stack_ décrit au point précédent.
  - `./src/basic.h`: Ajout d'un `#define WORLD_IOR` dans la classe `RAY` qui définit l'indice de réfraction de base du monde (1.0 pour de l'air).
  - `./src/object.h`: Ajout d'un `obj_id` dans la classe `Intersection` qui définit l'index de l'objet frappé.
  - `./src/container.cpp`: Calcul de l'`obj_id` de l'objet frappé dans `BVH::intersect` et `Naive::intersect`.
  - `./src/raytracer.cpp`: Gestion du `ior_stack` et calculs de réfraction dans `Raytracer::trace`.
- Notre réfraction supporte la réfraction interne totale.
- Notre raytracer est _multi-threaded_. Chaque colonne de pixels est générée en parallèle, et la génération de `BVH` et de `AABB` avant le _render_ traite la liste d'objets en parallèle.
  - `./src/basic.h`: Importation de `#include <future>` pour le parallélisme.
  - `./src/raytracer.cpp`: Ajout de parallélisme à la génération des colonnes de pixels dans `Raytracer::render`.
  - `./src/container.h`: Traitement parallèle des objets dans les constructeurs des classes `BVH` et `AABB`.
