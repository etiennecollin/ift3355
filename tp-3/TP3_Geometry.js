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
      if (nodeStack.length == 0) {
        break;
      }
      nowNode = nodeStack[0] ;
      nodeStack.shift();
      if (nowNode.childNode.length > 1) {
        for (i = 0; i < nowNode.childNode.length; i++) {
          nodeStack.push(nowNode.childNode[i]);
        }
        continue;
      } else if (nowNode.childNode.length == 1) {
        p0 = nowNode.p0.clone()
        p1 = nowNode.p1.clone()
        p0c = nowNode.childNode[0].p0.clone()
        p1c = nowNode.childNode[0].p1.clone()
        vec1 = p1.sub(p0)
        vec2 = p1c.sub(p0c)
        angle = this.findRotation(vec1, vec2)[1];
        if (angle < rotationThreshold) {
          nowNode.p1 = nowNode.childNode[0].p1;
          nowNode.a1 = nowNode.childNode[0].a1;
          nowNode.childNode = nowNode.childNode[0].childNode;
          nowNode.childNode[0].parentNode=nowNode
          nodeStack.push(nowNode);
          continue;
        }
        else{nodeStack.push(nowNode.childNode[0]);}
        
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
    h00 = 2 * t ** 3 - 3 * t ** 2 + 1;
    h10 = t ** 3 - 2 * t ** 2 + t;
    h01 = -2 * t ** 3 + 3 * t ** 2;
    h11 = t ** 3 - t ** 2;


    p = [
      h00 * h0[0] + h01 * h1[0] + h10 * v0[0] + h11 * v1[0],
      h00 * h0[1] + h01 * h1[1] + h10 * v0[1] + h11 * v1[1],
      ];


    h00Prime = 6 * t ** 2 - 6 * t;
    h10Prime = 3 * t ** 2 - 4 * t + 1;
    h01Prime = -6 * t ** 2 + 6 * t;
    h11Prime = 3 * t ** 2 - 2 * t;


    dp = [
      h00Prime * h0[0] + h01Prime * h1[0] + h10Prime * v0[0] + h11Prime * v1[0],
      h00Prime * h0[1] + h01Prime * h1[1] + h10Prime * v0[1] + h11Prime * v1[1],
      ];


    dpLength = Math.sqrt(dp[0] ** 2 + dp[1] ** 2);
    dp = dp.map((component) => component / dpLength);

    return [p, dp];
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
