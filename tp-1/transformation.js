// TRANSFORMATIONS

function multMat(m1, m2) {
  return new THREE.Matrix4().multiplyMatrices(m1, m2);
}

function inverseMat(m) {
  return new THREE.Matrix4().getInverse(m, true);
}

function idMat4() {
  // Create Identity matrix
  var m = new THREE.Matrix4();
  m.set(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
  return m;
}

function translateMat(matrix, x, y, z) {
  // Apply translation [x, y, z] to @matrix
  // matrix: THREE.Matrix4
  // x, y, z: float

  var m = new THREE.Matrix4();
  m.set(1, 0, 0, x, 0, 1, 0, y, 0, 0, 1, z, 0, 0, 0, 1);
  return multMat(m, matrix);
}

function rotateMat(matrix, angle, axis) {
  // Apply rotation by @angle with respect to @axis to @matrix
  // matrix: THREE.Matrix4
  // angle: float
  // axis: string "x", "y" or "z"

  // Store cos and sin evaluations for better performance
  var cos_val = Math.cos(angle);
  var sin_val = Math.sin(angle);

  var m = new THREE.Matrix4();
  switch (axis) {
    case "x":
      m.set(
        1,
        0,
        0,
        0,
        0,
        cos_val,
        -sin_val,
        0,
        0,
        sin_val,
        cos_val,
        0,
        0,
        0,
        0,
        1,
      );
      break;
    case "y":
      m.set(
        cos_val,
        0,
        sin_val,
        0,
        0,
        1,
        0,
        0,
        -sin_val,
        0,
        cos_val,
        0,
        0,
        0,
        0,
        1,
      );
      break;
    case "z":
      m.set(
        cos_val,
        -sin_val,
        0,
        0,
        sin_val,
        cos_val,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
      );
      break;
    default:
      throw new Error("Invalid axis for rotation matrix");
  }

  return multMat(m, matrix);
}

function rotateVec3(v, angle, axis) {
  // Apply rotation by @angle with respect to @axis to vector @v
  // v: THREE.Vector3
  // angle: float
  // axis: string "x", "y" or "z"

  return v.applyMatrix4(rotateMat(idMat4(), angle, axis));
}

function rescaleMat(matrix, x, y, z) {
  // Apply scaling @x, @y and @z to @matrix
  // matrix: THREE.Matrix3
  // x, y, z: float

  var m = new THREE.Matrix4();
  m.set(x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1);

  return multMat(m, matrix);
}

function getPosition(matrix, coordinate) {
  // Get value of @coordinate in @matrix
  var position;
  switch (coordinate) {
    case "x":
      position = matrix.elements[12];
      break;
    case "y":
      position = matrix.elements[13];
      break;
    case "z":
      position = matrix.elements[14];
      break;
    default:
      throw new Error("Invalid coordinate");
  }
  return position;
}
