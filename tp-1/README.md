# Le Robot

## Équipe

- Etienne Collin | 20237904
- Louis Malenfant-Poulin | 20120851

## Notes

- Le code du projet a été divisé en trois fichiers `.js` afin de se retrouver plus facilement dans le code.
  - `./A1.js` contient la logique principale du programme.
    - Création de la scène
    - Gestion des touche pour controller le robot
    - etc...
  - `./robot.js` contient la classe `Robot`
    - Création des membres du robot
    - Gestion de la hiérarchie
    - Fonctions pour déplacer le robot
    - Animation du robot
    - etc...
  - `./linalg.js` contient toutes les fonctions des transformations et opérations matricielles

ainsi, nous avons importé ces fichiers `.js` dans `./A1.html`:

```diff
<html>
  <head>
    <meta charset="UTF-8" />
    <title>Assignment 1</title>
    <style>
      body {
        margin: 0;
      }
      canvas {
        width: 100%;
        height: 100%;
      }
    </style>
  </head>
  <body>
    <script src="js/three.min.js"></script>
    <script src="js/OBJLoader.js"></script>
    <script src="js/SourceLoader.js"></script>
    <script src="js/OrbitControls.js"></script>
    <script src="js/KeyboardState.js"></script>
    <!-- By Jerome Etienne: http://jetienne.com/ -->
+   <script src="linalg.js"></script>
+   <script src="robot.js"></script>
    <script src="A1.js"></script>
  </body>
</html>
```
