class Node {
  constructor(parentNode) {
    this.parentNode = parentNode; //Noeud parent
    this.childNode = []; //Noeud enfants

    this.p0 = null; //Position de depart de la branche
    this.p1 = null; //Position finale de la branche

    this.a0 = null; //Rayon de la branche a p0
    this.a1 = null; //Rayon de la branche a p1

    this.sections = null; //Liste contenant une liste de points representant les segments circulaires du cylindre generalise
  }
}

TP3.Geometry = {
  simplifySkeleton: function (rootNode, rotationThreshold = 0.0001) {
    nodeStack = [rootNode];
    while (true) {
      if (length(nodeStack) == 0) {
        break;
      }
      nowNode = nodeStack.shift();
      if (length(nowNode.childNode) > 1) {
        for (i = 0; i < length(nowNode.childNode); i++) {
          nodeStack += nowNode.childNode[i];
        }
        continue;
      } else if (length(nowNode.childNode) == 1) {
        vec1 = nowNode.p0 - nowNode.p1;
        vec2 = nowNode.childNode[0].p0 - nowNode.childNode[0].p1;
        angle = this.findRotation(vec1, vec2)[1];
        if (angle < rotationThreshold) {
          nowNode.p1 = nowNode.childNode[0].p1;
          nowNode.a1 = nowNode.childNode[0].a1;
          nowNode.childNode = nowNode.childNode[0].childNode;
          nodeStack += nowNode;
          continue;
        }
        nodeStack += nowNode.childNode[0];
      }
    }
    return rootNode;
  },

  generateSegmentsHermite: function (
    rootNode,
    lengthDivisions = 4,
    radialDivisions = 8,
  ) {
    //TODO
  },

  hermite: function (h0, h1, v0, v1, t) {
    //TODO
  },

  // Trouver l'axe et l'angle de rotation entre deux vecteurs
  findRotation: function (a, b) {
    const axis = new THREE.Vector3().crossVectors(a, b).normalize();
    var c = a.dot(b) / (a.length() * b.length());

    if (c < -1) {
      c = -1;
    } else if (c > 1) {
      c = 1;
    }

    const angle = Math.acos(c);

    return [axis, angle];
  },

  // Projeter un vecter a sur b
  project: function (a, b) {
    return b.clone().multiplyScalar(a.dot(b) / b.lengthSq());
  },

  // Trouver le vecteur moyen d'une liste de vecteurs
  meanPoint: function (points) {
    var mp = new THREE.Vector3();

    for (var i = 0; i < points.length; i++) {
      mp.add(points[i]);
    }

    return mp.divideScalar(points.length);
  },
};
