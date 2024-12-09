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
      currentBranch = branches.shift();

      // Add children to the list of branches to process
      for (child of currentBranch.childNode) {
        branches.push(child);
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
      new THREE.MeshPhongMaterial({ color: 0x3a5f0b, side: THREE.DoubleSide }),
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
    const branches = [rootNode];
    const geometry = new THREE.BufferGeometry();
    const vertices = [];
    const indices = [];

    while (branches.length > 0) {
      const currentBranch = branches.shift();

      // Add child branches to the queue
      for (const child of currentBranch.childNode) {
        branches.push(child);
      }

      // Connect adjacent sections
      const sections = currentBranch.sections;
      for (let i = 0; i < sections.length - 1; i++) {
        const sectionA = sections[i];
        const sectionB = sections[i + 1];

        // Create faces between sectionA and sectionB
        for (let j = 0; j < sectionA.length; j++) {
          const nextJ = (j + 1) % sectionA.length;

          // Triangle 1
          vertices.push(...sectionA[j].toArray());
          vertices.push(...sectionA[nextJ].toArray());
          vertices.push(...sectionB[j].toArray());
          indices.push(
            vertices.length / 3 - 1,
            vertices.length / 3 - 2,
            vertices.length / 3 - 3,
          );

          // Triangle 2
          vertices.push(...sectionA[nextJ].toArray());
          vertices.push(...sectionB[nextJ].toArray());
          vertices.push(...sectionB[j].toArray());
          indices.push(
            vertices.length / 3 - 1,
            vertices.length / 3 - 2,
            vertices.length / 3 - 3,
          );
        }
      }

      // Cap the tree branches
      if (currentBranch.childNode.length == 0 || !currentBranch.parentNode) {
        let section = null;
        if (currentBranch.childNode.length == 0) {
          section = sections[sections.length - 1];
        } else {
          section = sections[0];
        }

        // Get the center of the last section
        const center = section
          .reduce((sum, point) => sum.add(point), new THREE.Vector3())
          .divideScalar(section.length);
        const centerIndex = vertices.length / 3;
        vertices.push(...center.toArray());

        for (let j = 0; j < section.length; j++) {
          const nextJ = (j + 1) % section.length;

          vertices.push(...section[j].toArray());
          vertices.push(...section[nextJ].toArray());
          indices.push(
            centerIndex,
            vertices.length / 3 - 1,
            vertices.length / 3 - 2,
          );
        }
      }
    }

    // Build geometry and mesh
    geometry.setAttribute(
      "position",
      new THREE.Float32BufferAttribute(vertices, 3),
    );
    geometry.setIndex(indices);
    geometry.computeVertexNormals();

    const material = new THREE.MeshLambertMaterial({
      color: 0x8b4513,
      // side: THREE.DoubleSide,
    });
    const mesh = new THREE.Mesh(geometry, material);

    scene.add(mesh);

    return [geometry];
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
