{
    "targets": [
        {
            "target_name": "ps-native",

            "sources": [
                "sources/process.h",
                "sources/wrapper.h",
                "sources/wrapper.cc",
                "sources/binding.cc"
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
                            "PS_WIN32",
							"UNICODE",
							"_UNICODE"
                        ],

                        "sources": [
                            "sources/process_win32.cc"
                        ],

                        "msvs_settings": {
                            "VCCLCompilerTool": {
								"WarningLevel": 3,
                                "ExceptionHandling": 2,  # /EHsc
                            },
                        }
                    }
                 ],

                ["OS == 'linux'",
                    {
                        "defines": [
                            "PS_LINUX"
                        ],

                        "sources": [
                            "sources/process_linux.cc"
                        ]
                    }
                 ]
            ]
        }
    ]
}
