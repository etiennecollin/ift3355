TP3.Render = {
  drawTreeRough: function (
    rootNode,
    scene,
    alpha,
    radialDivisions = 8,
    leavesCutoff = 0.1,
    leavesDensity = 10,
    applesProbability = 0.05,
    matrix = new THREE.Matrix4(),
  ) {
    branches = [rootNode];
    branchBuffers = [];
    leafBuffers = [];
    appleBuffers = [];

    // Iterate over the tree
    while (branches.length > 0) {
      currentBranch = branches[0];
      branches.shift();

      // Push child nodes to the stack
      for (i = 0; i < currentBranch.childNode.length; i++) {
        branches.push(currentBranch.childNode[i]);
      }

      // Create a cylinder geometry
      wood = new THREE.CylinderBufferGeometry(
        currentBranch.a1,
        currentBranch.a0,
        currentBranch.p0.distanceTo(currentBranch.p1),
        radialDivisions,
      );

      // Create the translation matrix
      translate = new THREE.Matrix4().set(
        1,
        0,
        0,
        (currentBranch.p0.x + currentBranch.p1.x) / 2,
        0,
        1,
        0,
        (currentBranch.p0.y + currentBranch.p1.y) / 2,
        0,
        0,
        1,
        (currentBranch.p0.z + currentBranch.p1.z) / 2,
        0,
        0,
        0,
        1,
      );

      // Get the direction of the branch
      branchDirection = new THREE.Vector3()
        .subVectors(currentBranch.p1, currentBranch.p0)
        .normalize();

      // Get angle between the UP and the direction of the branch
      quat = new THREE.Quaternion().setFromUnitVectors(
        new THREE.Vector3().set(0, 1, 0),
        branchDirection,
      );

      // Create the rotation matrix
      rotation = new THREE.Matrix4().makeRotationFromQuaternion(quat);

      // Apply the transformations
      wood.applyMatrix4(rotation);
      wood.applyMatrix4(translate);
      wood.applyMatrix4(matrix);
      branchBuffers.push(wood);

      // Check if branch needs leaves
      if (currentBranch.a0 < alpha * leavesCutoff) {
        // Add leavesDensity leaves to the branch
        for (i = 0; i < leavesDensity; i++) {
          leaf = new THREE.PlaneBufferGeometry(alpha, alpha);

          // Random translation within alpha/2 radius
          randomOffset = new THREE.Vector3(
            (Math.random() - 0.5) * alpha,
            (Math.random() - 0.5) * alpha,
            (Math.random() - 0.5) * alpha,
          );

          // Random rotation for the leaf
          randomQuat = new THREE.Quaternion().setFromEuler(
            new THREE.Euler(
              Math.random() * Math.PI,
              Math.random() * Math.PI,
              Math.random() * Math.PI,
            ),
          );

          leafRotation = new THREE.Matrix4().makeRotationFromQuaternion(
            randomQuat,
          );

          leafTranslation = new THREE.Matrix4().makeTranslation(
            currentBranch.p1.x + randomOffset.x,
            currentBranch.p1.y + randomOffset.y,
            currentBranch.p1.z + randomOffset.z,
          );

          // Apply transformations to the leaf
          leaf.applyMatrix4(leafRotation);
          leaf.applyMatrix4(leafTranslation);
          leaf.applyMatrix4(matrix);
          leafBuffers.push(leaf);
        }
      }

      // Add apples if branch width meets criteria
      if (
        currentBranch.a0 < alpha * leavesCutoff &&
        Math.random() < applesProbability
      ) {
        apple = new THREE.BoxBufferGeometry(alpha, alpha, alpha);

        // Random translation within alpha/2 radius
        randomOffset = new THREE.Vector3(
          (Math.random() - 0.5) * alpha,
          (Math.random() - 0.5) * alpha,
          (Math.random() - 0.5) * alpha,
        );

        appleTranslation = new THREE.Matrix4().makeTranslation(
          currentBranch.p1.x + randomOffset.x,
          currentBranch.p1.y + randomOffset.y,
          currentBranch.p1.z + randomOffset.z,
        );

        // Apply transformations to the apple
        apple.applyMatrix4(appleTranslation);
        apple.applyMatrix4(matrix);
        appleBuffers.push(apple);
      }
    }

    // Merge the geometries and create the mesh
    branchesMerged =
      THREE.BufferGeometryUtils.mergeBufferGeometries(branchBuffers);
    branchesMesh = new THREE.Mesh(
      branchesMerged,
      new THREE.MeshLambertMaterial({ color: 0x8b5a2b }),
    );
    scene.add(branchesMesh);

    leavesMerged = THREE.BufferGeometryUtils.mergeBufferGeometries(leafBuffers);
    leavesMesh = new THREE.Mesh(
      leavesMerged,
      new THREE.MeshPhongMaterial({ color: 0x3a5f0b }),
    );
    scene.add(leavesMesh);

    applesMerged =
      THREE.BufferGeometryUtils.mergeBufferGeometries(appleBuffers);
    applesMesh = new THREE.Mesh(
      applesMerged,
      new THREE.MeshPhongMaterial({ color: 0x5f0b0b }),
    );
    scene.add(applesMesh);
  },

  drawTreeHermite: function (
    rootNode,
    scene,
    alpha,
    leavesCutoff = 0.1,
    leavesDensity = 10,
    applesProbability = 0.05,
    matrix = new THREE.Matrix4(),
  ) {
    //TODO
  },

  updateTreeHermite: function (
    trunkGeometryBuffer,
    leavesGeometryBuffer,
    applesGeometryBuffer,
    rootNode,
  ) {
    //TODO
  },

  drawTreeSkeleton: function (
    rootNode,
    scene,
    color = 0xffffff,
    matrix = new THREE.Matrix4(),
  ) {
    var stack = [];
    stack.push(rootNode);

    var points = [];

    while (stack.length > 0) {
      var currentNode = stack.pop();

      for (var i = 0; i < currentNode.childNode.length; i++) {
        stack.push(currentNode.childNode[i]);
      }

      points.push(currentNode.p0);
      points.push(currentNode.p1);
    }

    var geometry = new THREE.BufferGeometry().setFromPoints(points);
    var material = new THREE.LineBasicMaterial({ color: color });
    var line = new THREE.LineSegments(geometry, material);
    line.applyMatrix4(matrix);
    scene.add(line);

    return line.geometry;
  },

  updateTreeSkeleton: function (geometryBuffer, rootNode) {
    var stack = [];
    stack.push(rootNode);

    var idx = 0;
    while (stack.length > 0) {
      var currentNode = stack.pop();

      for (var i = 0; i < currentNode.childNode.length; i++) {
        stack.push(currentNode.childNode[i]);
      }
      geometryBuffer[idx * 6] = currentNode.p0.x;
      geometryBuffer[idx * 6 + 1] = currentNode.p0.y;
      geometryBuffer[idx * 6 + 2] = currentNode.p0.z;
      geometryBuffer[idx * 6 + 3] = currentNode.p1.x;
      geometryBuffer[idx * 6 + 4] = currentNode.p1.y;
      geometryBuffer[idx * 6 + 5] = currentNode.p1.z;

      idx++;
    }
  },

  drawTreeNodes: function (
    rootNode,
    scene,
    color = 0x00ff00,
    size = 0.05,
    matrix = new THREE.Matrix4(),
  ) {
    var stack = [];
    stack.push(rootNode);

    var points = [];

    while (stack.length > 0) {
      var currentNode = stack.pop();

      for (var i = 0; i < currentNode.childNode.length; i++) {
        stack.push(currentNode.childNode[i]);
      }

      points.push(currentNode.p0);
      points.push(currentNode.p1);
    }

    var geometry = new THREE.BufferGeometry().setFromPoints(points);
    var material = new THREE.PointsMaterial({ color: color, size: size });
    var points = new THREE.Points(geometry, material);
    points.applyMatrix4(matrix);
    scene.add(points);
  },

  drawTreeSegments: function (
    rootNode,
    scene,
    lineColor = 0xff0000,
    segmentColor = 0xffffff,
    orientationColor = 0x00ff00,
    matrix = new THREE.Matrix4(),
  ) {
    var stack = [];
    stack.push(rootNode);

    var points = [];
    var pointsS = [];
    var pointsT = [];

    while (stack.length > 0) {
      var currentNode = stack.pop();

      for (var i = 0; i < currentNode.childNode.length; i++) {
        stack.push(currentNode.childNode[i]);
      }

      const segments = currentNode.sections;
      for (var i = 0; i < segments.length - 1; i++) {
        points.push(TP3.Geometry.meanPoint(segments[i]));
        points.push(TP3.Geometry.meanPoint(segments[i + 1]));
      }
      for (var i = 0; i < segments.length; i++) {
        pointsT.push(TP3.Geometry.meanPoint(segments[i]));
        pointsT.push(segments[i][0]);
      }

      for (var i = 0; i < segments.length; i++) {
        for (var j = 0; j < segments[i].length - 1; j++) {
          pointsS.push(segments[i][j]);
          pointsS.push(segments[i][j + 1]);
        }
        pointsS.push(segments[i][0]);
        pointsS.push(segments[i][segments[i].length - 1]);
      }
    }

    var geometry = new THREE.BufferGeometry().setFromPoints(points);
    var geometryS = new THREE.BufferGeometry().setFromPoints(pointsS);
    var geometryT = new THREE.BufferGeometry().setFromPoints(pointsT);

    var material = new THREE.LineBasicMaterial({ color: lineColor });
    var materialS = new THREE.LineBasicMaterial({ color: segmentColor });
    var materialT = new THREE.LineBasicMaterial({ color: orientationColor });

    var line = new THREE.LineSegments(geometry, material);
    var lineS = new THREE.LineSegments(geometryS, materialS);
    var lineT = new THREE.LineSegments(geometryT, materialT);

    line.applyMatrix4(matrix);
    lineS.applyMatrix4(matrix);
    lineT.applyMatrix4(matrix);

    scene.add(line);
    scene.add(lineS);
    scene.add(lineT);
  },
};
