{
    "targets": [
        {
            "target_name": "ps-native",

            "sources": [
                "source/process.h",
                "source/wrapper.h",
                "source/wrapper.cc",
                "source/binding.cc"
            ],

            "cflags": [
                "-std=c++11",
                "-stdlib=libc++"
            ],

            "include_dirs": [
                "<!(node -e \"require('nan')\")"
            ],

            "conditions": [
                ["OS == 'win'",
                    {
                        "defines": [
                            "PS_WIN32"
                        ],

                        "sources": [
                            "source/process_win32.cc"
                        ],

                        "libraries": [
                            "-lwtsapi32"
                        ],

                        "msvs_settings": {
                            "VCCLCompilerTool": {
                                "ExceptionHandling": "2",  # /EHsc
                            },
                        }
                    }
                 ],

                ["OS == 'linux'",
                    {
                        "defines": [
                            "PS_UNIX"
                        ],

                        "sources": [
                            "source/process_unix.cc"
                        ]
                    }
                 ]
            ]
        }
    ]
}
