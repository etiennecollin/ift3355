class Robot {
  constructor() {
    // Geometry
    this.torsoHeight = 1.5;
    this.torsoRadius = 0.75;
    this.headRadius = 0.32;
    // Add parameters for parts
    // TODO
    this.armLengthMultiplier = 2.3;
    this.armWidthMultiplier = 0.8;
    this.armRadius = 0.2;
    this.forearmLengthMultiplier = 1.2;
    this.forearmRadius = 0.12;
    this.forearmWidthMultiplier = 0.8;

    // Animation
    this.walkDirection = new THREE.Vector3(0, 0, 1);

    // Material
    this.material = new THREE.MeshNormalMaterial();

    // Initial pose
    this.initialize();
  }

  initialTorsoMatrix() {
    var initialTorsoMatrix = idMat4();
    initialTorsoMatrix = translateMat(
      initialTorsoMatrix,
      0,
      this.torsoHeight / 2,
      0,
    );

    return initialTorsoMatrix;
  }

  initialHeadMatrix() {
    var initialHeadMatrix = idMat4();
    initialHeadMatrix = translateMat(
      initialHeadMatrix,
      0,
      this.torsoHeight / 2 + this.headRadius,
      0,
    );

    return initialHeadMatrix;
  }

  initialArmMatrix(isLeft) {
    var initialArmMatrix = idMat4();

    // Rescale arm to be longer
    initialArmMatrix = rescaleMat(
      initialArmMatrix,
      this.armWidthMultiplier,
      this.armLengthMultiplier,
      this.armWidthMultiplier,
    );

    var translationX;
    if (isLeft) {
      translationX =
        this.torsoRadius + this.armRadius * this.armWidthMultiplier;
    } else {
      translationX = -(
        this.torsoRadius +
        this.armRadius * this.armWidthMultiplier
      );
    }

    // Translate arm to the side of the torso
    initialArmMatrix = translateMat(
      initialArmMatrix,
      translationX,
      this.torsoHeight * 0.1,
      0,
    );

    return initialArmMatrix;
  }

  initialForearmMatrix() {
    var initialForearmMatrix = idMat4();

    // Rescale arm to be longer
    initialForearmMatrix = rescaleMat(
      initialForearmMatrix,
      this.forearmWidthMultiplier,
      this.forearmLengthMultiplier,
      this.forearmWidthMultiplier,
    );

    // Translate arm down from the elbow
    var translationY =
      this.forearmRadius * this.forearmLengthMultiplier + this.armRadius;

    initialForearmMatrix = translateMat(
      initialForearmMatrix,
      0,
      -translationY,
      0,
    );

    return initialForearmMatrix;
  }

  initialize() {
    // Torso
    var torsoGeometry = new THREE.CubeGeometry(
      2 * this.torsoRadius,
      this.torsoHeight,
      this.torsoRadius,
      64,
    );
    this.torso = new THREE.Mesh(torsoGeometry, this.material);

    // Head
    var headGeometry = new THREE.CubeGeometry(
      2 * this.headRadius,
      this.headRadius,
      this.headRadius,
    );
    this.head = new THREE.Mesh(headGeometry, this.material);

    // Left Arm
    var armGeometry = new THREE.SphereGeometry(this.armRadius, 32, 32);
    this.leftArm = new THREE.Mesh(armGeometry, this.material);
    this.rightArm = new THREE.Mesh(armGeometry, this.material);
    var forearmGeometry = new THREE.SphereGeometry(this.forearmRadius, 32, 32);
    this.leftForearm = new THREE.Mesh(forearmGeometry, this.material);
    this.rightForearm = new THREE.Mesh(forearmGeometry, this.material);

    // TODO

    // =========================================================================
    // =========================================================================

    // Torse transformation
    this.torsoInitialMatrix = this.initialTorsoMatrix();
    this.torsoMatrix = idMat4();
    this.torso.setMatrix(this.torsoInitialMatrix);

    // Head transformation
    this.headInitialMatrix = this.initialHeadMatrix();
    this.headMatrix = idMat4();
    this.head.setMatrix(multMat(this.torso.matrix, this.headInitialMatrix));

    // Arm transformations
    this.leftArmInitialMatrix = this.initialArmMatrix(true);
    this.rightArmInitialMatrix = this.initialArmMatrix(false);
    this.leftArmMatrix = idMat4();
    this.rightArmMatrix = idMat4();
    this.leftArm.setMatrix(
      multMat(this.torso.matrix, this.leftArmInitialMatrix),
    );
    this.rightArm.setMatrix(
      multMat(this.torso.matrix, this.rightArmInitialMatrix),
    );

    // Forearms transformations
    this.leftForearmInitialMatrix = this.initialForearmMatrix();
    this.rightForearmInitialMatrix = this.initialForearmMatrix();
    this.leftForearmMatrix = idMat4();
    this.leftForearm.setMatrix(
      multMat(this.leftArm.matrix, this.leftForearmInitialMatrix),
    );

    this.rightForearmMatrix = idMat4();
    this.rightForearm.setMatrix(
      multMat(this.rightArm.matrix, this.rightForearmInitialMatrix),
    );
    // TODO
    //
    // =========================================================================
    // =========================================================================

    // Add robot to scene
    scene.add(this.torso);
    scene.add(this.head);
    scene.add(this.leftArm);
    scene.add(this.rightArm);
    scene.add(this.leftForearm);
    scene.add(this.rightForearm);
    // TODO
  }

  updateTorso() {
    var torsoFinalMatrix = multMat(this.torsoMatrix, this.torsoInitialMatrix);
    this.torso.setMatrix(torsoFinalMatrix);

    // Update dependent parts
    this.updateHead();
    this.updateArm(true);
    this.updateArm(false);
  }

  updateHead() {
    var headMultMatrix = multMat(this.headMatrix, this.headInitialMatrix);
    var headFinalMatrix = multMat(this.torso.matrix, headMultMatrix);
    this.head.setMatrix(headFinalMatrix);
  }

  updateArm(isLeft) {
    if (isLeft) {
      var leftArmMultMatrix = multMat(
        this.leftArmMatrix,
        this.leftArmInitialMatrix,
      );
      var leftArmFinalMatrix = multMat(this.torso.matrix, leftArmMultMatrix);
      this.leftArm.setMatrix(leftArmFinalMatrix);
    } else {
      var rightArmMultMatrix = multMat(
        this.rightArmMatrix,
        this.rightArmInitialMatrix,
      );
      var rightArmFinalMatrix = multMat(this.torso.matrix, rightArmMultMatrix);
      this.rightArm.setMatrix(rightArmFinalMatrix);
    }
    // Update dependent parts
    this.updateForearm(isLeft);
  }

  updateForearm(isLeft) {
    // FIXME: The issue with the forearm rotation is that once rotated by the first matrix multiplication (relative line 2 down), the forearm is multiplied with the left arm matrix (relative line 6 down). That matrix applies a y-scaling that is part of the initial arm matrix. Because the scaling is vertical, then the forearm changes length depending on its angle.
    if (isLeft) {
      var leftForearmMultMatrix = multMat(
        this.leftForearmMatrix,
        this.leftForearmInitialMatrix,
      );
      var leftForearmFinalMatrix = multMat(
        this.leftArm.matrix,
        leftForearmMultMatrix,
      );
      this.leftForearm.setMatrix(leftForearmFinalMatrix);
    } else {
      var rightForearmMultMatrix = multMat(
        this.rightForearmMatrix,
        this.rightForearmInitialMatrix,
      );
      var rightForearmFinalMatrix = multMat(
        this.rightArm.matrix,
        rightForearmMultMatrix,
      );
      this.rightForearm.setMatrix(rightForearmFinalMatrix);
    }
  }

  rotateTorso(angle) {
    var torsoMatrix = this.torsoMatrix;

    this.torsoMatrix = idMat4();
    this.torsoMatrix = rotateMat(this.torsoMatrix, angle, "y");
    this.torsoMatrix = multMat(torsoMatrix, this.torsoMatrix);

    this.updateTorso();

    this.walkDirection = rotateVec3(this.walkDirection, angle, "y");
  }

  moveTorso(speed) {
    this.torsoMatrix = translateMat(
      this.torsoMatrix,
      speed * this.walkDirection.x,
      speed * this.walkDirection.y,
      speed * this.walkDirection.z,
    );
    this.updateTorso();
  }

  rotateHead(angle) {
    var headMatrix = this.headMatrix;

    this.headMatrix = idMat4();
    this.headMatrix = rotateMat(this.headMatrix, angle, "y");
    this.headMatrix = multMat(headMatrix, this.headMatrix);

    this.updateHead();
  }

  rotateArm(angle, axis, isLeft) {
    var translationX =
      this.torsoRadius + this.armRadius * this.armWidthMultiplier;
    var translationY =
      this.torsoHeight * 0.1 + this.armRadius * this.armLengthMultiplier;

    var armMatrix;
    if (isLeft) {
      armMatrix = this.leftArmMatrix;
    } else {
      armMatrix = this.rightArmMatrix;
      translationX = -translationX;
    }

    var newArmMatrix = idMat4();
    newArmMatrix = translateMat(newArmMatrix, -translationX, -translationY, 0);
    newArmMatrix = rotateMat(newArmMatrix, angle, axis);
    newArmMatrix = translateMat(newArmMatrix, translationX, translationY, 0);

    if (isLeft) {
      this.leftArmMatrix = multMat(armMatrix, newArmMatrix);
    } else {
      this.rightArmMatrix = multMat(armMatrix, newArmMatrix);
    }

    this.updateArm(isLeft);
  }

  rotateForearm(angle, isLeft) {
    var translationY = this.armRadius; // this.forearmRadius * this.forearmLengthMultiplier is cancelled in the equation

    var forearmMatrix;
    if (isLeft) {
      forearmMatrix = this.leftForearmMatrix;
    } else {
      forearmMatrix = this.rightForearmMatrix;
    }

    var newForearmMatrix = idMat4();
    newForearmMatrix = translateMat(newForearmMatrix, 0, translationY, 0);
    newForearmMatrix = rotateMat(newForearmMatrix, angle, "x");
    newForearmMatrix = translateMat(newForearmMatrix, 0, -translationY, 0);

    if (isLeft) {
      this.leftForearmMatrix = multMat(forearmMatrix, newForearmMatrix);
    } else {
      this.rightForearmMatrix = multMat(forearmMatrix, newForearmMatrix);
    }

    this.updateForearm(isLeft);
  }

  // Add methods for other parts
  // TODO

  look_at(point) {
    // Compute and apply the correct rotation of the head and the torso for the robot to look at @point
    //TODO
  }
}
