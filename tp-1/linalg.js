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

function addMat4(m1, m2) {
  // Add two matrices
  // m1, m2: THREE.Matrix4
  var m = new THREE.Matrix4();
  for (var i = 0; i < 12; i++) {
    m.elements[i] = m1.elements[i] + m2.elements[i];
  }
  return m;
}

function multiplyScalarMat4(matrix, scalar) {
  // Multiply matrix by scalar
  // m: THREE.Matrix4
  // scalar: float
  var m = new THREE.Matrix4();
  for (var i = 0; i < 12; i++) {
    m.elements[i] = matrix.elements[i] * scalar;
  }
  return m;
}

function rotateMatVec(matrix, angle, vec) {
  // Apply a rotation by @angle with respect to @vec to @matrix
  // This uses the Rodrigues' rotation formula
  // https://en.wikipedia.org/wiki/Rodrigues'_rotation_formula

  var K = new THREE.Matrix4();
  K.set(
    0,
    -vec.z,
    vec.y,
    0,
    vec.z,
    0,
    -vec.x,
    0,
    -vec.y,
    vec.x,
    0,
    0,
    0,
    0,
    0,
    1,
  );

  var cos_val = Math.cos(angle);
  var sin_val = Math.sin(angle);

  var m = idMat4();
  m = addMat4(m, multiplyScalarMat4(K, sin_val));
  m = addMat4(m, multiplyScalarMat4(multMat(K, K), 1 - cos_val));
  return multMat(m, matrix);
}

function rotateVec3Vec(v, angle, vec) {
  // Apply a rotation by @angle with respect to @vec to @v
  // This uses the Rodrigues' rotation formula
  // https://en.wikipedia.org/wiki/Rodrigues'_rotation_formula

  return v.applyMatrix4(rotateMatVec(idMat4(), angle, vec));
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

function getPoint(matrix) {
  // Get point from @matrix
  return new THREE.Vector3(
    matrix.elements[12],
    matrix.elements[13],
    matrix.elements[14],
  );
}

function getNormVec3(v) {
  return Math.sqrt(v.x ** 2 + v.y ** 2 + v.z ** 2);
}

function normalizeVec3(v) {
  // Normalize vector @v
  var norm = getNormVec3(v);
  return new THREE.Vector3(v.x / norm, v.y / norm, v.z / norm);
}

function subtractVec3(v1, v2) {
  // Substract vector @v2 from @v1
  return new THREE.Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

function dotVec3(v1, v2) {
  // Dot product between @v1 and @v2
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

function crossVec3(v1, v2) {
  // Cross product between @v1 and @v2
  return new THREE.Vector3(
    v1.y * v2.z - v1.z * v2.y,
    v1.z * v2.x - v1.x * v2.z,
    v1.x * v2.y - v1.y * v2.x,
  );
}
