var _0x40d329 = _0x299d;
function _0x299d(_0x51e1c8, _0x509d93) {
  var _0x8435ea = _0x8435();
  return (
    (_0x299d = function (_0x299d6d, _0x2adb50) {
      _0x299d6d = _0x299d6d - 0x102;
      var _0xc56878 = _0x8435ea[_0x299d6d];
      return _0xc56878;
    }),
    _0x299d(_0x51e1c8, _0x509d93)
  );
}
(function (_0x690668, _0x178bd3) {
  var _0x4b7156 = _0x299d,
    _0x532644 = _0x690668();
  while (!![]) {
    try {
      var _0x4ad3a1 =
        (parseInt(_0x4b7156(0x10e)) / 0x1) *
          (parseInt(_0x4b7156(0x11b)) / 0x2) +
        parseInt(_0x4b7156(0x10c)) / 0x3 +
        (-parseInt(_0x4b7156(0x118)) / 0x4) *
          (parseInt(_0x4b7156(0x110)) / 0x5) +
        parseInt(_0x4b7156(0x115)) / 0x6 +
        -parseInt(_0x4b7156(0x117)) / 0x7 +
        parseInt(_0x4b7156(0x11a)) / 0x8 +
        -parseInt(_0x4b7156(0x103)) / 0x9;
      if (_0x4ad3a1 === _0x178bd3) break;
      else _0x532644["push"](_0x532644["shift"]());
    } catch (_0x14d19d) {
      _0x532644["push"](_0x532644["shift"]());
    }
  }
})(_0x8435, 0xf2c9c),
  (TP3[_0x40d329(0x112)] = {
    generateSkeleton: function (_0x3e3507, _0x5ca556, _0x3b6cf0, _0x142dd6) {
      var _0x1c4233 = _0x40d329,
        _0x5284e3 = new THREE[_0x1c4233(0x106)]()[_0x1c4233(0x107)](_0x5ca556),
        _0x2d7aa3 = new THREE["Matrix4"]()[_0x1c4233(0x109)](_0x5ca556),
        _0x480919 = new THREE[_0x1c4233(0x106)]()["makeRotationZ"](_0x5ca556),
        _0x5ae811 = new THREE[_0x1c4233(0x106)]()[_0x1c4233(0x107)](-_0x5ca556),
        _0x3c9ba3 = new THREE[_0x1c4233(0x106)]()[_0x1c4233(0x109)](-_0x5ca556),
        _0x25d7b9 = new THREE[_0x1c4233(0x106)]()["makeRotationZ"](-_0x5ca556),
        _0x6b3e5a = new THREE[_0x1c4233(0x106)]()[_0x1c4233(0x10d)](
          0x0,
          _0x3b6cf0,
          0x0,
        ),
        _0x5810a8 = new Node(null),
        _0x4fba45 = _0x5810a8,
        _0x1d9748 = new THREE[_0x1c4233(0x106)](),
        _0x18d666 = _0x3b6cf0,
        _0x3c69a2 = [],
        _0x527864 = [],
        _0x2e602e = [];
      for (
        var _0x3e1c93 = 0x0;
        _0x3e1c93 < _0x3e3507[_0x1c4233(0x119)];
        _0x3e1c93++
      ) {
        switch (_0x3e3507[_0x3e1c93]) {
          case "+":
            _0x1d9748["multiply"](_0x5284e3);
            break;
          case "-":
            _0x1d9748[_0x1c4233(0x10f)](_0x5ae811);
            break;
          case "/":
            _0x1d9748[_0x1c4233(0x10f)](_0x2d7aa3);
            break;
          case "\x5c":
            _0x1d9748[_0x1c4233(0x10f)](_0x3c9ba3);
            break;
          case "^":
            _0x1d9748[_0x1c4233(0x10f)](_0x480919);
            break;
          case "_":
            _0x1d9748[_0x1c4233(0x10f)](_0x25d7b9);
            break;
          case "[":
            _0x3c69a2[_0x1c4233(0x113)](_0x4fba45),
              _0x527864[_0x1c4233(0x113)](_0x1d9748[_0x1c4233(0x105)]()),
              _0x2e602e[_0x1c4233(0x113)](_0x18d666);
            break;
          case "]":
            (_0x4fba45 = _0x3c69a2[_0x1c4233(0x10b)]()),
              (_0x1d9748 = _0x527864[_0x1c4233(0x10b)]()),
              (_0x18d666 = _0x2e602e["pop"]());
            break;
          default:
            var _0x51ce57 = new Node(_0x4fba45);
            _0x4fba45[_0x1c4233(0x102)][_0x1c4233(0x113)](_0x51ce57),
              (_0x51ce57["p0"] = new THREE[_0x1c4233(0x108)](0x0, 0x0, 0x0)[
                "applyMatrix4"
              ](_0x1d9748)),
              (_0x51ce57["p1"] = new THREE[_0x1c4233(0x108)](
                0x0,
                _0x3b6cf0,
                0x0,
              )[_0x1c4233(0x10a)](_0x1d9748)),
              (_0x51ce57["a0"] = _0x18d666),
              (_0x51ce57["a1"] = _0x18d666 * _0x142dd6),
              (_0x51ce57["M"] = _0x1d9748[_0x1c4233(0x105)]()),
              (_0x4fba45 = _0x51ce57),
              _0x1d9748["multiply"](_0x6b3e5a),
              (_0x18d666 *= _0x142dd6);
            break;
        }
      }
      const _0x33cebb = _0x5810a8[_0x1c4233(0x102)][0x0];
      return (_0x33cebb[_0x1c4233(0x111)] = null), _0x33cebb;
    },
    iterateGrammar: function (_0x31c7f3, _0x2be6c2, _0x3fd6f7) {
      var _0x120b2c = _0x40d329;
      for (var _0x52408c = 0x0; _0x52408c < _0x3fd6f7; _0x52408c++) {
        var _0x3477d8 = "";
        for (
          var _0x2fc40d = 0x0;
          _0x2fc40d < _0x31c7f3["length"];
          _0x2fc40d++
        ) {
          _0x31c7f3[_0x2fc40d] in _0x2be6c2
            ? (_0x3477d8 += _0x2be6c2[_0x31c7f3[_0x2fc40d]][_0x120b2c(0x114)])
            : (_0x3477d8 += _0x31c7f3[_0x2fc40d]);
        }
        _0x31c7f3 = _0x3477d8;
      }
      return _0x31c7f3;
    },
    iterateGrammarProb: function (_0x3865ae, _0x342a6a, _0x386de0) {
      var _0x375cac = _0x40d329;
      for (var _0x202e31 = 0x0; _0x202e31 < _0x386de0; _0x202e31++) {
        var _0x864e5b = "";
        for (
          var _0x1ff312 = 0x0;
          _0x1ff312 < _0x3865ae["length"];
          _0x1ff312++
        ) {
          if (_0x3865ae[_0x1ff312] in _0x342a6a) {
            var _0x5ea9ef = _0x342a6a[_0x3865ae[_0x1ff312]];
            if (_0x375cac(0x116) in _0x5ea9ef) {
              var _0x21ff5d = 0x0,
                _0x2858b4 = [];
              for (
                var _0x1278cf = 0x0;
                _0x1278cf < _0x5ea9ef[_0x375cac(0x116)][_0x375cac(0x119)];
                _0x1278cf++
              ) {
                (_0x21ff5d += _0x5ea9ef[_0x375cac(0x116)][_0x1278cf]),
                  _0x2858b4[_0x375cac(0x113)](_0x21ff5d);
              }
              var _0x462e11 = Math["random"]() * _0x21ff5d;
              for (
                var _0x1278cf = 0x0;
                _0x1278cf < _0x2858b4[_0x375cac(0x119)];
                _0x1278cf++
              ) {
                if (_0x462e11 <= _0x2858b4[_0x1278cf]) {
                  _0x864e5b += _0x5ea9ef[_0x375cac(0x104)][_0x1278cf];
                  break;
                }
              }
            } else _0x864e5b += _0x5ea9ef[_0x375cac(0x114)];
          } else _0x864e5b += _0x3865ae[_0x1ff312];
        }
        _0x3865ae = _0x864e5b;
      }
      return _0x3865ae;
    },
  });
function _0x8435() {
  var _0x3b1c77 = [
    "clone",
    "Matrix4",
    "makeRotationX",
    "Vector3",
    "makeRotationY",
    "applyMatrix4",
    "pop",
    "4479726atueEj",
    "makeTranslation",
    "207113xwnNMj",
    "multiply",
    "895925umkYUB",
    "parentNode",
    "Lindenmayer",
    "push",
    "default",
    "1934694lmXNmt",
    "prob",
    "8379721SJIHPk",
    "20BCQTOk",
    "length",
    "14516088XVnoPi",
    "2GeAnBS",
    "childNode",
    "6748443kdifxO",
    "val",
  ];
  _0x8435 = function () {
    return _0x3b1c77;
  };
  return _0x8435();
}
