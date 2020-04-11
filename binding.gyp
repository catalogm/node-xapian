{
  "targets": [
    {
      "target_name": "xapian",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "cflags": ["-fexceptions", "-g"],
      "cflags_cc": [ "-std=c++17", "-fexceptions", "-g" ],
      "sources": [
        "src/module.cc",
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
      ],
      "link_settings": {
        "ldflags": [
          "<!(pkg-config --libs-only-other --libs-only-L xapian-core)",
        ],
        "libraries": [
          "<!(pkg-config --libs-only-l xapian-core)",
        ],
      },
      "defines": [
        "NAPI_CPP_EXCEPTIONS",
        "NAPI_EXPERIMENTAL",
      ],
    },
  ]
}
