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
  "Right Arm",
  "Right Forearm",

  "Left Leg",
  "Left Thigh",
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
        robot.rotateArm(-0.1, "x", true);
        break;
      case "Right Arm":
        robot.rotateArm(-0.1, "x", false);
        break;
      case "Left Forearm":
        robot.rotateForearm(0.1, true);
        break;
      case "Right Forearm":
        robot.rotateForearm(0.1, false);
        break;
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
        robot.rotateArm(0.1, "x", true);
        break;
      case "Right Arm":
        robot.rotateArm(0.1, "x", false);
        break;
      case "Left Forearm":
        robot.rotateForearm(-0.1, true);
        break;
      case "Right Forearm":
        robot.rotateForearm(-0.1, false);
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
        robot.rotateArm(0.1, "z", true);
        break;
      case "Right Arm":
        robot.rotateArm(0.1, "z", false);
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
        robot.rotateArm(-0.1, "z", true);
        break;
      case "Right Arm":
        robot.rotateArm(-0.1, "z", false);
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
