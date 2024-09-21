// ASSIGNMENT-SPECIFIC API EXTENSION
THREE.Object3D.prototype.setMatrix = function (a) {
  this.matrix = a;
  this.matrix.decompose(this.position, this.quaternion, this.scale);
};

var start = Date.now();
// SETUP RENDERER AND SCENE
var scene = new THREE.Scene();
var renderer = new THREE.WebGLRenderer();
renderer.setClearColor(0xffffff); // white background colour
document.body.appendChild(renderer.domElement);

// SETUP CAMERA
var camera = new THREE.PerspectiveCamera(30, 1, 0.1, 1000); // view angle, aspect ratio, near, far
camera.position.set(10, 5, 10);
camera.lookAt(scene.position);
scene.add(camera);

// SETUP ORBIT CONTROL OF THE CAMERA
var controls = new THREE.OrbitControls(camera);
controls.damping = 0.2;

// ADAPT TO WINDOW RESIZE
function resize() {
  renderer.setSize(window.innerWidth, window.innerHeight);
  camera.aspect = window.innerWidth / window.innerHeight;
  camera.updateProjectionMatrix();
}

window.addEventListener("resize", resize);
resize();

// FLOOR WITH CHECKERBOARD
var floorTexture = new THREE.ImageUtils.loadTexture("images/tile.jpg");
floorTexture.wrapS = floorTexture.wrapT = THREE.MirroredRepeatWrapping;
floorTexture.repeat.set(4, 4);

var floorMaterial = new THREE.MeshBasicMaterial({
  map: floorTexture,
  side: THREE.DoubleSide,
});
var floorGeometry = new THREE.PlaneBufferGeometry(15, 15);
var floor = new THREE.Mesh(floorGeometry, floorMaterial);
floor.rotation.x = Math.PI / 2;
floor.position.y = 0.0;
scene.add(floor);

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

    var angle;
    var position;
    if (isLeft) {
      angle = Math.PI / 2;
      position = this.torsoRadius;
    } else {
      angle = -Math.PI / 2;
      position = -this.torsoRadius;
    }
    initialArmMatrix = rotateMat(initialArmMatrix, angle, "y");

    // Translate arm to the side of the torso
    initialArmMatrix = translateMat(
      initialArmMatrix,
      position,
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
    var position = -(
      this.forearmRadius * this.forearmLengthMultiplier +
      this.armRadius
    );
    initialForearmMatrix = translateMat(initialForearmMatrix, 0, position, 0);

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
    var armGeometry = new THREE.SphereGeometry(
      this.armRadius,
      32,
      32,
      0,
      Math.PI,
      0,
      Math.PI,
    );
    this.leftArm = new THREE.Mesh(armGeometry, this.material);
    this.rightArm = new THREE.Mesh(armGeometry, this.material);
    var forearmGeometry = new THREE.SphereGeometry(
      this.forearmRadius,
      32,
      32,
      0,
      Math.PI,
      0,
      Math.PI,
    );
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

      // Update dependent parts
      this.updateForearm(true);
    } else {
      var rightArmMultMatrix = multMat(
        this.rightArmMatrix,
        this.rightArmInitialMatrix,
      );
      var rightArmFinalMatrix = multMat(this.torso.matrix, rightArmMultMatrix);
      this.rightArm.setMatrix(rightArmFinalMatrix);

      // Update dependent parts
      this.updateForearm(false);
    }
  }

  updateForearm(isLeft) {
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

  rotateLeftArm(angle, axis) {
    var leftArmMatrix = this.leftArmMatrix;

    

    this.leftArmMatrix = idMat4();
    if(axis=="z"){
      this.leftArmMatrix = translateMat(this.leftArmMatrix, -this.torsoRadius, -0.61,0)
    }
    if(axis=="x"){
      this.leftArmMatrix = translateMat(this.leftArmMatrix, -this.torsoRadius, -0.61,0)
    }
    this.leftArmMatrix = rotateMat(this.leftArmMatrix, angle, axis);
    if(axis=="z"){
      this.leftArmMatrix = translateMat(this.leftArmMatrix, this.torsoRadius, 0.61,0)
    }
    if(axis=="x"){
      this.leftArmMatrix = translateMat(this.leftArmMatrix, this.torsoRadius, 0.61,0)
    }
    console.log(this.leftArmMatrix)
    this.leftArmMatrix = multMat(leftArmMatrix, this.leftArmMatrix);

    this.updateArm(true);
  }

  rotateRightArm(angle, axis) {
    var leftArmMatrix = this.leftArmMatrix;

    this.leftArmMatrix = idMat4();
    this.leftArmMatrix = rotateMat(this.leftArmMatrix, angle, axis);
    this.leftArmMatrix = multMat(leftArmMatrix, this.leftArmMatrix);

    this.updateArm(false);
  }

  // Add methods for other parts
  // TODO

  look_at(point) {
    // Compute and apply the correct rotation of the head and the torso for the robot to look at @point
    //TODO
  }
}

var robot = new Robot();

// LISTEN TO KEYBOARD
var keyboard = new THREEx.KeyboardState();

var selectedRobotComponent = 0;
var components = [
  "Torso",
  "Head",
  // Add parts names
  // TODO
  "Left Arm",
  "Left Forearm",
  "Left Leg",
  "Left Thigh",

  "Right Arm",
  "Right Forearm",
  "Right Leg",
  "Right Thigh",

  "Left Eye",
  "Right Eye",
];
var numberComponents = components.length;

//MOUSE EVENTS
var raycaster = new THREE.Raycaster();
var mouse = new THREE.Vector2();
var sphere = null;

document.addEventListener("mousemove", onMouseMove, false);

var isRightButtonDown = false;

function checkKeyboard() {
  // Next element
  if (keyboard.pressed("e")) {
    selectedRobotComponent = selectedRobotComponent + 1;

    if (selectedRobotComponent < 0) {
      selectedRobotComponent = numberComponents - 1;
    }

    if (selectedRobotComponent >= numberComponents) {
      selectedRobotComponent = 0;
    }

    window.alert(components[selectedRobotComponent] + " selected");
  }

  // Previous element
  if (keyboard.pressed("q")) {
    selectedRobotComponent = selectedRobotComponent - 1;

    if (selectedRobotComponent < 0) {
      selectedRobotComponent = numberComponents - 1;
    }

    if (selectedRobotComponent >= numberComponents) {
      selectedRobotComponent = 0;
    }

    window.alert(components[selectedRobotComponent] + " selected");
  }

  // UP
  if (keyboard.pressed("w")) {
    switch (components[selectedRobotComponent]) {
      case "Torso":
        robot.moveTorso(0.1);
        break;
      case "Head":
        break;
      case "Left Arm":
        robot.rotateLeftArm(0.1, "x");
      case "Right Arm":
        robot.rotateRightArm(0.1, "x");
      // Add more cases
      // TODO
    }
  }

  // DOWN
  if (keyboard.pressed("s")) {
    switch (components[selectedRobotComponent]) {
      case "Torso":
        robot.moveTorso(-0.1);
        break;
      case "Head":
        break;
      case "Left Arm":
        robot.rotateLeftArm(-0.1, "x");
        break;
      case "Right Arm":
        robot.rotateRightArm(-0.1, "x");
        break;
      // Add more cases
      // TODO
    }
  }

  // LEFT
  if (keyboard.pressed("a")) {
    switch (components[selectedRobotComponent]) {
      case "Torso":
        robot.rotateTorso(0.1);
        break;
      case "Head":
        robot.rotateHead(0.1);
        break;
      case "Left Arm":
        robot.rotateLeftArm(0.1, "z");
        break;
      case "Right Arm":
        robot.rotateRightArm(0.1, "z");
        break;
      // Add more cases
      // TODO
    }
  }

  // RIGHT
  if (keyboard.pressed("d")) {
    switch (components[selectedRobotComponent]) {
      case "Torso":
        robot.rotateTorso(-0.1);
        break;
      case "Head":
        robot.rotateHead(-0.1);
        break;
      case "Left Arm":
        robot.rotateLeftArm(-0.1, "z");
        break;
      case "Right Arm":
        robot.rotateLeftArm(-0.1, "z");
        break;
      // Add more cases
      // TODO
    }
  }

  if (keyboard.pressed("f")) {
    isRightButtonDown = true;

    var vector = new THREE.Vector3(mouse.x, mouse.y, 0.5);

    vector.unproject(camera);

    var dir = vector.sub(camera.position).normalize();

    raycaster.ray.origin.copy(camera.position);
    raycaster.ray.direction.copy(dir);

    var intersects = raycaster.intersectObjects(scene.children, true);

    if (intersects.length > 0) {
      if (!sphere) {
        var geometry = new THREE.SphereGeometry(0.1, 32, 32);
        var material = new THREE.MeshBasicMaterial({ color: 0xff0000 });
        sphere = new THREE.Mesh(geometry, material);
        scene.add(sphere);
      }
    }

    updateLookAtPosition();
  } else {
    isRightButtonDown = false;

    if (sphere) {
      scene.remove(sphere);
      sphere.geometry.dispose();
      sphere.material.dispose();
      sphere = null;
    }
  }
}

function onMouseMove(event) {
  mouse.x = (event.clientX / window.innerWidth) * 2 - 1;
  mouse.y = -(event.clientY / window.innerHeight) * 2 + 1;

  if (isRightButtonDown) {
    updateLookAtPosition();
  }
}

function updateLookAtPosition() {
  var vector = new THREE.Vector3(mouse.x, mouse.y, 0.5);

  vector.unproject(camera);

  var dir = vector.sub(camera.position).normalize();

  raycaster.ray.origin.copy(camera.position);
  raycaster.ray.direction.copy(dir);

  var intersects = raycaster.intersectObjects(
    scene.children.filter((obj) => obj !== sphere),
    true,
  );

  if (intersects.length > 0) {
    var intersect = intersects[0];
    sphere.position.copy(intersect.point);
    robot.look_at(intersect.point);
  }
}

// SETUP UPDATE CALL-BACK
function update() {
  checkKeyboard();
  requestAnimationFrame(update);
  renderer.render(scene, camera);
}

update();
