const appleMass = 0.075;

TP3.Physics = {
  initTree: function (rootNode) {
    this.computeTreeMass(rootNode);

    var stack = [];
    stack.push(rootNode);

    while (stack.length > 0) {
      var currentNode = stack.pop();
      for (var i = 0; i < currentNode.childNode.length; i++) {
        stack.push(currentNode.childNode[i]);
      }

      currentNode.vel = new THREE.Vector3();
      currentNode.strength = currentNode.a0;
    }
  },

  computeTreeMass: function (node) {
    var mass = 0;

    for (var i = 0; i < node.childNode.length; i++) {
      mass += this.computeTreeMass(node.childNode[i]);
    }
    mass += node.a1;
    if (node.appleIndices !== null) {
      mass += appleMass;
    }
    node.mass = mass;

    return mass;
  },

  applyForces: function (node, dt, time) {
    var u = Math.sin(1 * time) * 4;
    u += Math.sin(2.5 * time) * 2;
    u += Math.sin(5 * time) * 0.4;

    var v = Math.cos(1 * time + 56485) * 4;
    v += Math.cos(2.5 * time + 56485) * 2;
    v += Math.cos(5 * time + 56485) * 0.4;

    // Ajouter le vent
    node.vel.add(
      new THREE.Vector3(
        u / Math.sqrt(node.mass),
        0,
        v / Math.sqrt(node.mass),
      ).multiplyScalar(dt),
    );
    // Ajouter la gravite
    node.vel.add(new THREE.Vector3(0, -node.mass, 0).multiplyScalar(dt));

    // Calcul de la nouvelle position p1
    const p1New = node.p1.clone().add(node.vel.clone().multiplyScalar(dt));

    // Conserver la longueur de la branche
    const originalLength = node.p1.clone().sub(node.p0).length();
    const originalDirection = node.p1.clone().sub(node.p0).normalize();

    // Stocker la direction originale
    if (!node.originalDirection) {
      node.originalDirection = originalDirection.clone();
    }

    const newDirection = p1New.clone().sub(node.p0).normalize();

    // Trouver la matrice de rotation entre les directions
    const rotationQuat = new THREE.Quaternion().setFromUnitVectors(
      originalDirection,
      newDirection,
    );
    const rotationMatrix = new THREE.Matrix4().makeRotationFromQuaternion(
      rotationQuat,
    );

    // Appliquer la matrice de rotation pour conserver la longueur
    const p1Direction = originalDirection
      .clone()
      .applyMatrix4(rotationMatrix)
      .multiplyScalar(originalLength);
    const p1NewScaled = node.p0.clone().add(p1Direction);

    // Calculer la nouvelle vélocité après projection
    const trueVelocity = p1NewScaled.clone().sub(node.p1).divideScalar(dt);

    node.vel = trueVelocity;

    // Calculer l'angle pour la force de restitution
    // Nous utilisons l'angle par rapport à la direction à t=0 de la branche.
    // Nous devons donc changer le coefficient a0*1000 par a0*33.
    const angle = node.originalDirection.angleTo(newDirection);
    const normal = newDirection.clone().cross(originalDirection).normalize();
    const restitutionDirection = node.originalDirection
      .clone()
      .applyAxisAngle(normal, Math.pow(angle, 2));
    const restitutionVelocity = restitutionDirection.multiplyScalar(
      node.a0 * 33,
    );
    node.vel.add(restitutionVelocity);

    // Appliquer l'amortissement
    node.vel.multiplyScalar(0.7);

    // Mettre à jour la position de p1
    node.p1.copy(p1NewScaled);

    // Appel recursif sur les enfants
    for (var i = 0; i < node.childNode.length; i++) {
      // Appliquer la matrice de transformation au noeud enfant
      const child = node.childNode[i];

      // Bring p0 to origin
      const translation = new THREE.Matrix4().makeTranslation(
        -child.p0.x,
        -child.p0.y,
        -child.p0.z,
      );

      // Bring p0 back to its place
      const translation_inverse = new THREE.Matrix4().makeTranslation(
        child.p0.x,
        child.p0.y,
        child.p0.z,
      );

      // Translate child's p0 to its parent's p1
      const translation_final = new THREE.Matrix4().makeTranslation(
        node.p1.x - child.p0.x,
        node.p1.y - child.p0.y,
        node.p1.z - child.p0.z,
      );

      // Store transformation matrix
      child.transform = new THREE.Matrix4()
        .multiply(translation_final)
        .multiply(translation_inverse)
        .multiply(rotationMatrix)
        .multiply(translation);

      // Apply transformation matrix
      child.p0.applyMatrix4(child.transform);
      child.p1.applyMatrix4(child.transform);
      this.applyForces(child, dt, time);
    }
  },
};
