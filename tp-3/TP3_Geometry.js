class Node {
  constructor(parentNode) {
    this.parentNode = parentNode; //Noeud parent
    this.childNode = []; //Noeud enfants

    this.p0 = null; //Position de depart de la branche
    this.p1 = null; //Position finale de la branche

    this.a0 = null; //Rayon de la branche a p0
    this.a1 = null; //Rayon de la branche a p1

    this.sections = null; //Liste contenant une liste de points representant les segments circulaires du cylindre generalise

    this.originalDirection = null; // Direction initial de la branche
    this.transform = new THREE.Matrix4();

    this.branchVertexIndices = [];
    this.leafVertexIndices = [];
    this.appleVertexIndices = [];
  }
}

TP3.Geometry = {
  simplifySkeleton: function (rootNode, rotationThreshold = 0.0001) {
    nodeStack = [rootNode];

    // Iterate over the tree
    while (true) {
      // If the stack is empty, we are done
      if (nodeStack.length == 0) {
        break;
      }

      // If there is more than a node in the stack
      currentNode = nodeStack[0];
      nodeStack.shift();
      if (currentNode.childNode.length > 1) {
        for (i = 0; i < currentNode.childNode.length; i++) {
          nodeStack.push(currentNode.childNode[i]);
        }
        continue;
      }

      // If there is only one child node
      if (currentNode.childNode.length == 1) {
        p0 = currentNode.p0.clone();
        p1 = currentNode.p1.clone();
        p0c = currentNode.childNode[0].p0.clone();
        p1c = currentNode.childNode[0].p1.clone();
        vec1 = p1.sub(p0);
        vec2 = p1c.sub(p0c);
        angle = this.findRotation(vec1, vec2)[1];

        // If the angle is smaller than the threshold, we simplify the tree
        if (angle < rotationThreshold) {
          currentNode.p1 = currentNode.childNode[0].p1;
          currentNode.a1 = currentNode.childNode[0].a1;
          currentNode.childNode = currentNode.childNode[0].childNode;
          currentNode.childNode[0].parentNode = currentNode;
          nodeStack.push(currentNode);
          continue;
        } else {
          nodeStack.push(currentNode.childNode[0]);
        }
      }
    }

    // Return the rootNode
    return rootNode;
  },

  generateSegmentsHermite: function (
    rootNode,
    lengthDivisions = 4,
    radialDivisions = 8,
  ) {
    const nodes = [rootNode];

    // Iterate over the tree
    while (nodes.length > 0) {
      const currentNode = nodes.shift();

      // Add children to the list of branches to process
      for (child of currentNode.childNode) {
        nodes.push(child);
      }

      // Compute the direction of the branch
      const v0 = currentNode.parentNode
        ? new THREE.Vector3().subVectors(
            currentNode.parentNode.p1,
            currentNode.parentNode.p0,
          )
        : new THREE.Vector3(0, 1, 0); // Default root direction
      const v1 = new THREE.Vector3().subVectors(currentNode.p1, currentNode.p0);

      // Compute the sections of the branch
      currentNode.sections = [];
      for (let i = 0; i <= lengthDivisions; i++) {
        const t = i / lengthDivisions;

        // Compute the point and its tangent at t
        const [p, dp] = TP3.Geometry.hermite(
          currentNode.p0,
          currentNode.p1,
          v0,
          v1,
          t,
        );

        // Interpolate the radius of the branch
        const radius = THREE.MathUtils.lerp(currentNode.a0, currentNode.a1, t);

        // Check which vector to use for the cross product
        let ref = new THREE.Vector3(0, 0, 1);
        if (Math.abs(dp.clone().dot(ref)) > 0.99) {
          ref = new THREE.Vector3(1, 0, 0);
        }

        // Parameterize the branch section
        const u = new THREE.Vector3().crossVectors(dp, ref).normalize();
        const v = new THREE.Vector3().crossVectors(u, dp).normalize();

        // Generate the section of the branch
        const section = [];
        for (let j = 0; j < radialDivisions; j++) {
          // Get the offset of the vertex from the point
          const angle = (j / radialDivisions) * Math.PI * 2;
          const x = Math.cos(angle) * radius;
          const y = Math.sin(angle) * radius;
          const offset = u
            .clone()
            .multiplyScalar(x)
            .add(v.clone().multiplyScalar(y));

          // Add the vertex to the section
          const vertex = p.clone().add(offset);
          section.push(vertex);
        }

        currentNode.sections.push(section);
      }
    }

    return rootNode;
  },

  hermite: function (h0, h1, v0, v1, t) {
    // Use De Casteljau's algorithm to calculate the point and its tangent
    const p0 = h0.clone();
    const p1 = h0.clone().add(v0.clone().multiplyScalar(1 / 3));
    const p2 = h1.clone().sub(v1.clone().multiplyScalar(1 / 3));
    const p3 = h1.clone();

    const lerp = (a, b, t) => a.clone().lerp(b, t);

    const p01 = lerp(p0, p1, t);
    const p12 = lerp(p1, p2, t);
    const p23 = lerp(p2, p3, t);

    const p012 = lerp(p01, p12, t);
    const p123 = lerp(p12, p23, t);

    const p = lerp(p012, p123, t);
    const dp = p123.clone().sub(p012).normalize();

    return [p, dp];
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
