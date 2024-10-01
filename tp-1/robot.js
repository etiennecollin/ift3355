class Robot {
  constructor() {
    // Torso
    this.torsoHeight = 1.5;
    this.torsoRadius = 0.75;
    this.torsoTranslationX = 0;
    this.torsoTranslationY = this.torsoHeight / 2;
    this.torsoRotationY = 0;

    // Head
    this.headRadius = 0.32;
    this.headTranslationX = 0;
    this.headTranslationY = this.torsoTranslationY + this.headRadius / 2;
    this.headRotationX = 0;
    this.headRotationY = 0;
    this.headRotationZ = 0;
    this.headLimitRotation = Math.PI / 2;

    // Arms
    this.armLengthMultiplier = 2.3;
    this.armWidthMultiplier = 0.8;
    this.armRadius = 0.2;
    this.armTranslationX =
      this.torsoRadius + this.armRadius * this.armWidthMultiplier;
    this.armTranslationY = this.torsoHeight * 0.1;
    this.leftArmRotationX = 0;
    this.rightArmRotationX = 0;
    this.leftArmRotationZ = 0;
    this.rightArmRotationZ = 0;
    this.armLimitRotationMinX = Math.PI / 4;
    this.armLimitRotationMaxX = -Math.PI;
    this.armLimitRotationMinZ = 0;
    this.armLimitRotationMaxZ = -Math.PI;

    // Forearms
    this.forearmLengthMultiplier = 2.75;
    this.forearmRadius = 0.12;
    this.forearmWidthMultiplier = 0.64;
    this.forearmTranslationX = 0;
    this.forearmTranslationY = -(
      this.forearmRadius * this.forearmLengthMultiplier +
      this.armRadius * this.armLengthMultiplier
    );
    this.leftForearmRotationX = 0;
    this.rightForearmRotationX = 0;
    this.forearmLimitRotationMin = 0;
    this.forearmLimitRotationMax = -(3 * Math.PI) / 4;

    // Thighs
    this.thighWidthMultiplier = 1;
    this.thighLengthMultiplier = 2;
    this.thighRadius = 0.2;
    this.thighTranslationX = (2 / 3) * this.torsoRadius;
    this.thighTranslationY = -(
      this.torsoTranslationY +
      this.thighRadius * this.thighLengthMultiplier
    );
    this.leftThighRotationX = 0;
    this.rightThighRotationX = 0;
    this.thighLimitRotationMin = Math.PI / 2;
    this.thighLimitRotationMax = -Math.PI / 2;

    // Legs
    this.legWidthMultiplier = 0.8;
    this.legLengthMultiplier = 2.5;
    this.legRadius = 0.15;
    this.legTranslationX = 0;
    this.legTranslationY = -(
      this.legRadius * this.legLengthMultiplier +
      this.thighRadius * this.thighLengthMultiplier
    );
    this.leftLegRotationX = 0;
    this.rightLegRotationX = 0;
    this.legLimitRotationMax = -Math.PI / 8;
    this.legLimitRotationMin = (3 * Math.PI) / 4;

    // Eyes
    this.eyeRadius = 0.05;
    this.eyeTranslationX = this.headRadius / 3;
    this.eyeTranslationY = 0;
    this.eyeTranslationZ = (3 * this.headRadius) / 5;

    // Animation
    this.walkDirection = new THREE.Vector3(0, 0, 1);
    this.lookDirection = new THREE.Vector3(0, 0, 1);
    this.maxAnimationAngle = 0.1;
    this.walkCurrentFrame = 0;
    this.walkFrames = [
      {
        leftThigh: 0,
        leftLeg: 0,
        rightThigh: 0,
        rightLeg: 0,
      },
      {
        leftThigh: Math.PI / 4,
        leftLeg: 0,
        rightThigh: -Math.PI / 4,
        rightLeg: (3 * Math.PI) / 16,
      },
      {
        leftThigh: Math.PI / 8,
        leftLeg: (3 * Math.PI) / 32,
        rightThigh: -Math.PI / 8,
        rightLeg: (3 * Math.PI) / 32,
      },
      {
        leftThigh: 0,
        leftLeg: 0,
        rightThigh: 0,
        rightLeg: 0,
      },
      {
        leftThigh: -Math.PI / 4,
        leftLeg: (3 * Math.PI) / 16,
        rightThigh: Math.PI / 4,
        rightLeg: 0,
      },
      {
        leftThigh: -Math.PI / 8,
        leftLeg: (3 * Math.PI) / 32,
        rightThigh: Math.PI / 8,
        rightLeg: (3 * Math.PI) / 32,
      },
    ];

    // Material
    this.material = new THREE.MeshNormalMaterial();

    // Initial pose
    this.initialize();
  }

  initialTorsoMatrix() {
    var initialTorsoMatrix = idMat4();
    initialTorsoMatrix = translateMat(
      initialTorsoMatrix,
      this.torsoTranslationX,
      this.torsoTranslationY,
      0,
    );

    return initialTorsoMatrix;
  }

  initialHeadMatrix() {
    var initialHeadMatrix = idMat4();
    initialHeadMatrix = translateMat(
      initialHeadMatrix,
      this.headTranslationX,
      this.headTranslationY,
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

    // Store the inverse of the scaling matrix
    this.inverseArmScaling = rescaleMat(
      idMat4(),
      1 / this.armWidthMultiplier,
      1 / this.armLengthMultiplier,
      1 / this.armWidthMultiplier,
    );

    // Translate arm to the side of the torso
    initialArmMatrix = translateMat(
      initialArmMatrix,
      isLeft ? this.armTranslationX : -this.armTranslationX,
      this.armTranslationY,
      0,
    );

    return initialArmMatrix;
  }

  initialEyeMatrix(isLeft) {
    var initialEyeMatrix = idMat4();

    initialEyeMatrix = translateMat(
      initialEyeMatrix,
      isLeft ? this.eyeTranslationX : -this.eyeTranslationX,
      this.eyeTranslationY,
      this.eyeTranslationZ,
    );

    return initialEyeMatrix;
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
    initialForearmMatrix = translateMat(
      initialForearmMatrix,
      this.forearmTranslationX,
      this.forearmTranslationY,
      0,
    );

    return initialForearmMatrix;
  }

  initialThighMatrix(isLeft) {
    var initialThighMatrix = idMat4();

    // Rescale thigh to be longer
    initialThighMatrix = rescaleMat(
      initialThighMatrix,
      this.thighWidthMultiplier,
      this.thighLengthMultiplier,
      this.thighWidthMultiplier,
    );

    // Store the inverse of the scaling matrix
    this.inverseThighScaling = rescaleMat(
      idMat4(),
      1 / this.thighWidthMultiplier,
      1 / this.thighLengthMultiplier,
      1 / this.thighWidthMultiplier,
    );

    // Translate thigh to the side of the torso
    initialThighMatrix = translateMat(
      initialThighMatrix,
      isLeft ? this.thighTranslationX : -this.thighTranslationX,
      this.thighTranslationY,
      0,
    );

    return initialThighMatrix;
  }

  initialLegMatrix() {
    var initialLegMatrix = idMat4();

    // Rescale leg to be longer
    initialLegMatrix = rescaleMat(
      initialLegMatrix,
      this.legWidthMultiplier,
      this.legLengthMultiplier,
      this.legWidthMultiplier,
    );

    // Translate leg down from the thigh
    initialLegMatrix = translateMat(
      initialLegMatrix,
      this.legTranslationX,
      this.legTranslationY,
      0,
    );

    return initialLegMatrix;
  }

  initialize() {
    // Torso
    var torsoGeometry = new THREE.BoxGeometry(
      2 * this.torsoRadius,
      this.torsoHeight,
      this.torsoRadius,
      64,
    );
    this.torso = new THREE.Mesh(torsoGeometry, this.material);

    // Head
    var headGeometry = new THREE.BoxGeometry(
      2 * this.headRadius,
      this.headRadius,
      this.headRadius,
    );
    this.head = new THREE.Mesh(headGeometry, this.material);

    // Arms
    var armGeometry = new THREE.SphereGeometry(this.armRadius, 32, 32);
    this.leftArm = new THREE.Mesh(armGeometry, this.material);
    this.rightArm = new THREE.Mesh(armGeometry, this.material);

    // Forearms
    var forearmGeometry = new THREE.SphereGeometry(this.forearmRadius, 32, 32);
    this.leftForearm = new THREE.Mesh(forearmGeometry, this.material);
    this.rightForearm = new THREE.Mesh(forearmGeometry, this.material);

    // Thighs
    var thighGeometry = new THREE.SphereGeometry(this.thighRadius, 32, 32);
    this.leftThigh = new THREE.Mesh(thighGeometry, this.material);
    this.rightThigh = new THREE.Mesh(thighGeometry, this.material);

    // Legs
    var legGeometry = new THREE.SphereGeometry(this.legRadius, 32, 32);
    this.leftLeg = new THREE.Mesh(legGeometry, this.material);
    this.rightLeg = new THREE.Mesh(legGeometry, this.material);

    // Eyes
    var eyeGeometry = new THREE.SphereGeometry(this.eyeRadius, 32, 32);
    this.leftEye = new THREE.Mesh(eyeGeometry, this.material);
    this.rightEye = new THREE.Mesh(eyeGeometry, this.material);

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
    this.rightForearmMatrix = idMat4();
    this.leftForearm.setMatrix(
      multMat(
        multMat(this.leftArm.matrix, this.inverseArmScaling),
        this.leftForearmInitialMatrix,
      ),
    );
    this.rightForearm.setMatrix(
      multMat(
        multMat(this.rightArm.matrix, this.inverseArmScaling),
        this.rightForearmInitialMatrix,
      ),
    );

    // Thigh transformations
    this.leftThighInitialMatrix = this.initialThighMatrix(true);
    this.rightThighInitialMatrix = this.initialThighMatrix(false);
    this.leftThighMatrix = idMat4();
    this.rightThighMatrix = idMat4();
    this.leftThigh.setMatrix(
      multMat(this.torso.matrix, this.leftThighInitialMatrix),
    );
    this.rightThigh.setMatrix(
      multMat(this.torso.matrix, this.rightThighInitialMatrix),
    );

    // Leg transformations
    this.leftLegInitialMatrix = this.initialLegMatrix();
    this.rightLegInitialMatrix = this.initialLegMatrix();
    this.leftLegMatrix = idMat4();
    this.rightLegMatrix = idMat4();
    this.leftLeg.setMatrix(
      multMat(
        multMat(this.leftThigh.matrix, this.inverseThighScaling),
        this.leftLegInitialMatrix,
      ),
    );
    this.rightLeg.setMatrix(
      multMat(
        multMat(this.rightThigh.matrix, this.inverseThighScaling),
        this.rightLegInitialMatrix,
      ),
    );

    // Eye transformation
    this.leftEyeInitialMatrix = this.initialEyeMatrix(true);
    this.rightEyeInitialMatrix = this.initialEyeMatrix(false);
    this.leftEyeMatrix = idMat4();
    this.rightEyeMatrix = idMat4();
    this.leftEye.setMatrix(
      multMat(this.head.matrix, this.leftEyeInitialMatrix),
    );
    this.rightEye.setMatrix(
      multMat(this.head.matrix, this.rightEyeInitialMatrix),
    );

    // =========================================================================
    // =========================================================================

    // Add robot to scene
    scene.add(this.torso);
    scene.add(this.head);
    scene.add(this.leftArm);
    scene.add(this.rightArm);
    scene.add(this.leftForearm);
    scene.add(this.rightForearm);
    scene.add(this.leftThigh);
    scene.add(this.rightThigh);
    scene.add(this.leftLeg);
    scene.add(this.rightLeg);
    scene.add(this.leftEye);
    scene.add(this.rightEye);

    // Make sure the robot touches the ground
    this.groundRobot();
  }

  getEffectiveRotationAngle(angle) {
    if (Math.abs(angle) < this.maxAnimationAngle) {
      return angle;
    } else {
      return angle > 0 ? this.maxAnimationAngle : -this.maxAnimationAngle;
    }
  }

  walk() {
    var currentFrame = this.walkFrames[this.walkCurrentFrame];
    var rotated = false;
    var rotation;

    // Check if we need to update the walk frame
    if (currentFrame.leftThigh != this.leftThighRotationX) {
      rotation = this.getEffectiveRotationAngle(
        currentFrame.leftThigh - this.leftThighRotationX,
      );
      this.rotateThigh(rotation, true);
      rotated = true;
    }
    if (currentFrame.leftLeg != this.leftLegRotationX) {
      rotation = this.getEffectiveRotationAngle(
        currentFrame.leftLeg - this.leftLegRotationX,
      );
      this.rotateLeg(rotation, true);
      rotated = true;
    }
    if (currentFrame.rightThigh != this.rightThighRotationX) {
      rotation = this.getEffectiveRotationAngle(
        currentFrame.rightThigh - this.rightThighRotationX,
      );
      this.rotateThigh(rotation, false);
      rotated = true;
    }
    if (currentFrame.rightLeg != this.rightLegRotationX) {
      rotation = this.getEffectiveRotationAngle(
        currentFrame.rightLeg - this.rightLegRotationX,
      );
      this.rotateLeg(rotation, false);
      rotated = true;
    }

    if (!rotated) {
      this.walkCurrentFrame =
        (this.walkCurrentFrame + 1) % this.walkFrames.length;
    }
  }

  groundRobot() {
    // Get min height of legs
    var leftLegPosition = getPosition(this.leftLeg.matrix, "y");
    var rightLegPosition = getPosition(this.rightLeg.matrix, "y");

    // Get the effective rotation angle of the legs
    var rotationAngleLeft = this.leftLegRotationX + this.leftThighRotationX;
    var rotationAngleRight = this.rightLegRotationX + this.rightThighRotationX;

    // Get the position of the tip of the legs
    var trueLeftLegPosition =
      leftLegPosition -
      Math.cos(rotationAngleLeft) * this.legRadius * this.legLengthMultiplier;
    var trueRightLegPosition =
      rightLegPosition -
      Math.cos(rotationAngleRight) * this.legRadius * this.legLengthMultiplier;

    var minLegPosition = Math.min(trueLeftLegPosition, trueRightLegPosition);

    // Move the robot up by minLegPosition
    this.torsoMatrix = translateMat(this.torsoMatrix, 0, -minLegPosition, 0);
    this.updateTorso();
  }

  updateTorso() {
    var torsoFinalMatrix = multMat(this.torsoMatrix, this.torsoInitialMatrix);
    this.torso.setMatrix(torsoFinalMatrix);

    // Update dependent parts
    this.updateHead();
    this.updateArm(true);
    this.updateArm(false);
    this.updateThigh(true);
    this.updateThigh(false);
  }

  updateHead() {
    var headMultMatrix = multMat(this.headMatrix, this.headInitialMatrix);
    var headFinalMatrix = multMat(this.torso.matrix, headMultMatrix);
    this.head.setMatrix(headFinalMatrix);

    this.updateEye(true);
    this.updateEye(false);
  }

  updateEye(isLeft) {
    var matrix;
    if (isLeft) {
      matrix = multMat(this.leftEyeMatrix, this.leftEyeInitialMatrix);
      matrix = multMat(this.head.matrix, matrix);
      this.leftEye.setMatrix(matrix);
    } else {
      matrix = multMat(this.rightEyeMatrix, this.rightEyeInitialMatrix);
      matrix = multMat(this.head.matrix, matrix);
      this.rightEye.setMatrix(matrix);
    }
  }

  updateArm(isLeft) {
    var matrix;
    if (isLeft) {
      matrix = multMat(this.leftArmMatrix, this.leftArmInitialMatrix);
      matrix = multMat(this.torso.matrix, matrix);
      this.leftArm.setMatrix(matrix);
    } else {
      matrix = multMat(this.rightArmMatrix, this.rightArmInitialMatrix);
      matrix = multMat(this.torso.matrix, matrix);
      this.rightArm.setMatrix(matrix);
    }

    this.updateForearm(isLeft);
  }

  updateForearm(isLeft) {
    var matrix, parentMatrix;
    if (isLeft) {
      matrix = multMat(this.leftForearmMatrix, this.leftForearmInitialMatrix);
      parentMatrix = multMat(this.leftArm.matrix, this.inverseArmScaling);
      matrix = multMat(parentMatrix, matrix);
      this.leftForearm.setMatrix(matrix);
    } else {
      matrix = multMat(this.rightForearmMatrix, this.rightForearmInitialMatrix);
      parentMatrix = multMat(this.rightArm.matrix, this.inverseArmScaling);
      matrix = multMat(parentMatrix, matrix);
      this.rightForearm.setMatrix(matrix);
    }
  }

  updateThigh(isLeft) {
    var matrix;
    if (isLeft) {
      matrix = multMat(this.leftThighMatrix, this.leftThighInitialMatrix);
      matrix = multMat(this.torso.matrix, matrix);
      this.leftThigh.setMatrix(matrix);
    } else {
      matrix = multMat(this.rightThighMatrix, this.rightThighInitialMatrix);
      matrix = multMat(this.torso.matrix, matrix);
      this.rightThigh.setMatrix(matrix);
    }

    this.updateLeg(isLeft);
  }

  updateLeg(isLeft) {
    var matrix, parentMatrix;
    if (isLeft) {
      matrix = multMat(this.leftLegMatrix, this.leftLegInitialMatrix);
      parentMatrix = multMat(this.leftThigh.matrix, this.inverseThighScaling);
      matrix = multMat(parentMatrix, matrix);
      this.leftLeg.setMatrix(matrix);
    } else {
      matrix = multMat(this.rightLegMatrix, this.rightLegInitialMatrix);
      parentMatrix = multMat(this.rightThigh.matrix, this.inverseThighScaling);
      matrix = multMat(parentMatrix, matrix);
      this.rightLeg.setMatrix(matrix);
    }
  }

  rotateTorso(angle) {
    var torsoMatrix = this.torsoMatrix;
    this.torsoMatrix = idMat4();
    this.torsoMatrix = rotateMat(this.torsoMatrix, angle, "y");
    this.torsoMatrix = multMat(torsoMatrix, this.torsoMatrix);

    this.updateTorso();

    this.torsoRotationY += angle;
    this.walkDirection = rotateVec3(this.walkDirection, angle, "y");
    this.lookDirection = rotateVec3(this.lookDirection, angle, "y");
  }

  moveTorso(speed) {
    this.torsoMatrix = translateMat(
      this.torsoMatrix,
      speed * this.walkDirection.x,
      speed * this.walkDirection.y,
      speed * this.walkDirection.z,
    );

    this.updateTorso();
    this.walk();
  }

  rotateHead(angle, axis) {
    // Rotate the head round its center
    // The axis of the rotation can be given as a string "x", "y" or "z"
    // The axis can also be given as a normalized THREE.Vector3

    // Check if the angle is within the limits
    if (
      (axis == "x" &&
        Math.abs(this.headRotationX + angle) > this.headLimitRotation) ||
      (axis == "y" &&
        Math.abs(this.headRotationY + angle) > this.headLimitRotation) ||
      (axis == "z" &&
        Math.abs(this.headRotationZ + angle) > this.headLimitRotation)
    ) {
      return;
    }

    var headMatrix = this.headMatrix;
    var translationX = this.headTranslationX;
    var translationY = this.headTranslationY;

    // Translate head to the origin
    var newHeadMatrix = idMat4();
    newHeadMatrix = translateMat(
      newHeadMatrix,
      -translationX,
      -translationY,
      0,
    );

    // Get the rotation axis
    var vec;
    if (axis == "x") {
      vec = new THREE.Vector3(1, 0, 0);
      this.lookDirection = rotateVec3(this.lookDirection, angle, "x");
      this.headRotationX += angle;
    } else if (axis == "y") {
      vec = new THREE.Vector3(0, 1, 0);
      this.lookDirection = rotateVec3(this.lookDirection, angle, "y");
      this.headRotationY += angle;
    } else if (axis == "z") {
      vec = new THREE.Vector3(0, 0, 1);
      this.lookDirection = rotateVec3(this.lookDirection, angle, "z");
      this.headRotationZ += angle;
    } else {
      vec = axis;
      this.lookDirection = rotateVec3Vec(this.lookDirection, angle, vec);
    }

    // Rotate head around the axis
    newHeadMatrix = rotateMatVec(newHeadMatrix, angle, vec);

    // Translate head back to its original position
    newHeadMatrix = translateMat(newHeadMatrix, translationX, translationY, 0);
    this.headMatrix = multMat(headMatrix, newHeadMatrix);

    // Update head and its children
    this.updateHead();
  }

  rotateArm(angle, axis, isLeft) {
    // Make sure the angle is within the limits
    if (axis == "x") {
      if (isLeft) {
        if (this.leftArmRotationX + angle < this.armLimitRotationMaxX) {
          angle = this.armLimitRotationMaxX - this.leftArmRotationX;
        } else if (this.leftArmRotationX + angle > this.armLimitRotationMinX) {
          angle = this.armLimitRotationMinX - this.leftArmRotationX;
        }
      } else {
        if (this.rightArmRotationX + angle < this.armLimitRotationMaxX) {
          angle = this.armLimitRotationMaxX - this.rightArmRotationX;
        } else if (this.rightArmRotationX + angle > this.armLimitRotationMinX) {
          angle = this.armLimitRotationMinX - this.rightArmRotationX;
        }
      }
    } else if (axis == "z") {
      if (isLeft) {
        if (this.leftArmRotationZ + angle < this.armLimitRotationMaxZ) {
          angle = this.armLimitRotationMaxZ - this.leftArmRotationZ;
        } else if (this.leftArmRotationZ + angle > this.armLimitRotationMinZ) {
          angle = this.armLimitRotationMinZ - this.leftArmRotationZ;
        }
      } else {
        if (this.rightArmRotationZ + angle < this.armLimitRotationMaxZ) {
          angle = this.armLimitRotationMaxZ - this.rightArmRotationZ;
        } else if (this.rightArmRotationZ + angle > this.armLimitRotationMinZ) {
          angle = this.armLimitRotationMinZ - this.rightArmRotationZ;
        }
      }
    }

    var translationX = isLeft ? this.armTranslationX : -this.armTranslationX;
    var translationY =
      this.armTranslationY + this.armRadius * this.armLengthMultiplier;
    var armMatrix = isLeft ? this.leftArmMatrix : this.rightArmMatrix;

    var newArmMatrix = idMat4();
    newArmMatrix = translateMat(newArmMatrix, -translationX, -translationY, 0);
    newArmMatrix = rotateMat(newArmMatrix, angle, axis);
    newArmMatrix = translateMat(newArmMatrix, translationX, translationY, 0);

    if (isLeft) {
      this.leftArmMatrix = multMat(armMatrix, newArmMatrix);
      if (axis == "x") {
        this.leftArmRotationX += angle;
      } else if (axis == "z") {
        this.leftArmRotationZ += angle;
      }
    } else {
      this.rightArmMatrix = multMat(armMatrix, newArmMatrix);
      if (axis == "x") {
        this.rightArmRotationX += angle;
      } else if (axis == "z") {
        this.rightArmRotationZ += angle;
      }
    }

    this.updateArm(isLeft);
  }

  rotateForearm(angle, isLeft) {
    // Make sure the angle is within the limits
    if (isLeft) {
      if (this.leftForearmRotationX + angle < this.forearmLimitRotationMax) {
        angle = this.forearmLimitRotationMax - this.leftForearmRotationX;
      } else if (
        this.leftForearmRotationX + angle >
        this.forearmLimitRotationMin
      ) {
        angle = this.forearmLimitRotationMin - this.leftForearmRotationX;
      }
    } else {
      if (this.rightForearmRotationX + angle < this.forearmLimitRotationMax) {
        angle = this.forearmLimitRotationMax - this.rightForearmRotationX;
      } else if (
        this.rightForearmRotationX + angle >
        this.forearmLimitRotationMin
      ) {
        angle = this.forearmLimitRotationMin - this.rightForearmRotationX;
      }
    }

    var translationX = isLeft
      ? this.forearmTranslationX
      : -this.forearmTranslationX;
    var translationY =
      this.forearmTranslationY +
      this.forearmRadius * this.forearmLengthMultiplier;
    var forearmMatrix = isLeft
      ? this.leftForearmMatrix
      : this.rightForearmMatrix;

    var newForearmMatrix = idMat4();
    newForearmMatrix = translateMat(
      newForearmMatrix,
      -translationX,
      -translationY,
      0,
    );
    newForearmMatrix = rotateMat(newForearmMatrix, angle, "x");
    newForearmMatrix = translateMat(
      newForearmMatrix,
      translationX,
      translationY,
      0,
    );

    if (isLeft) {
      this.leftForearmMatrix = multMat(forearmMatrix, newForearmMatrix);
      this.leftForearmRotationX += angle;
    } else {
      this.rightForearmMatrix = multMat(forearmMatrix, newForearmMatrix);
      this.rightForearmRotationX += angle;
    }

    this.updateForearm(isLeft);
  }

  rotateThigh(angle, isLeft) {
    // Make sure the angle is within the limits
    if (isLeft) {
      if (this.leftThighRotationX + angle < this.thighLimitRotationMax) {
        angle = this.thighLimitRotationMax - this.leftThighRotationX;
      } else if (this.leftThighRotationX + angle > this.thighLimitRotationMin) {
        angle = this.thighLimitRotationMin - this.leftThighRotationX;
      }
    } else {
      if (this.rightThighRotationX + angle < this.thighLimitRotationMax) {
        angle = this.thighLimitRotationMax - this.rightThighRotationX;
      } else if (
        this.rightThighRotationX + angle >
        this.thighLimitRotationMin
      ) {
        angle = this.thighLimitRotationMin - this.rightThighRotationX;
      }
    }

    var translationX = isLeft
      ? this.thighTranslationX
      : -this.thighTranslationX;
    var translationY =
      this.thighTranslationY + this.thighRadius * this.thighLengthMultiplier;
    var thighMatrix = isLeft ? this.leftThighMatrix : this.rightThighMatrix;

    var newThighMatrix = idMat4();
    newThighMatrix = translateMat(
      newThighMatrix,
      -translationX,
      -translationY,
      0,
    );
    newThighMatrix = rotateMat(newThighMatrix, angle, "x");
    newThighMatrix = translateMat(
      newThighMatrix,
      translationX,
      translationY,
      0,
    );

    if (isLeft) {
      this.leftThighMatrix = multMat(thighMatrix, newThighMatrix);
      this.leftThighRotationX += angle;
    } else {
      this.rightThighMatrix = multMat(thighMatrix, newThighMatrix);
      this.rightThighRotationX += angle;
    }

    this.updateThigh(isLeft);
    this.groundRobot();
  }

  rotateLeg(angle, isLeft) {
    // Make sure the angle is within the limits
    if (isLeft) {
      if (this.leftLegRotationX + angle < this.legLimitRotationMax) {
        angle = this.legLimitRotationMax - this.leftLegRotationX;
      } else if (this.leftLegRotationX + angle > this.legLimitRotationMin) {
        angle = this.legLimitRotationMin - this.leftLegRotationX;
      }
    } else {
      if (this.rightLegRotationX + angle < this.legLimitRotationMax) {
        angle = this.legLimitRotationMax - this.rightLegRotationX;
      } else if (this.rightLegRotationX + angle > this.legLimitRotationMin) {
        angle = this.legLimitRotationMin - this.rightLegRotationX;
      }
    }

    var translationX = isLeft ? this.legTranslationX : -this.legTranslationX;
    var translationY =
      this.legTranslationY + this.legRadius * this.legLengthMultiplier;
    var legMatrix = isLeft ? this.leftLegMatrix : this.rightLegMatrix;

    var newLegMatrix = idMat4();
    newLegMatrix = translateMat(newLegMatrix, -translationX, -translationY, 0);
    newLegMatrix = rotateMat(newLegMatrix, angle, "x");
    newLegMatrix = translateMat(newLegMatrix, translationX, translationY, 0);

    if (isLeft) {
      this.leftLegMatrix = multMat(legMatrix, newLegMatrix);
      this.leftLegRotationX += angle;
    } else {
      this.rightLegMatrix = multMat(legMatrix, newLegMatrix);
      this.rightLegRotationX += angle;
    }

    this.updateLeg(isLeft);
    this.groundRobot();
  }

  look_at(point) {
    var angle, direction;
    var vertical = new THREE.Vector3(0, 1, 0);

    // =================================
    // TORSO
    // =================================

    // Compute the direction vector between the torso and the point
    var torso2point = subtractVec3(point, getPoint(this.torso.matrix));
    // Project it on the ground
    torso2point.y = 0;

    // Find the direction the rotation should have
    direction = crossVec3(torso2point, this.walkDirection).dot(vertical);

    // Compute the rotation angle
    angle = Math.acos(
      dotVec3(this.walkDirection, torso2point) /
        (getNormVec3(this.walkDirection) * getNormVec3(torso2point)),
    );

    // If the two vectors are parallel, the ratio is 1 and acos is not defined
    // In this case, we set the target rotation to 0
    angle = isNaN(angle) ? 0 : angle;

    // Use a rotation of at most maxAnimationAngle
    angle = Math.min(Math.abs(angle), this.maxAnimationAngle);
    angle = direction < 0 ? angle : -angle;
    this.rotateTorso(angle);

    // =================================
    // HEAD
    // =================================

    // This uses spherical coordinates to calculate the head rotation
    var headPoint = getPoint(this.head.matrix);
    var head2point = subtractVec3(point, headPoint);
    var height = headPoint.y - point.y;
    angle = Math.asin(height / getNormVec3(head2point)) - this.headRotationX;
    angle = this.getEffectiveRotationAngle(angle);
    this.rotateHead(angle, "x");
  }
}
